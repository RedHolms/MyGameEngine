# Usage:
#  python3 generate-opengl-headers.py <cmake binary dir> <destination dir> <specification url> <target api> <target profile>

import requests
import os
import sys
import xml.etree.ElementTree as ET
import urllib.parse
from packaging.version import Version

binary_dir = sys.argv[1]
dest_dir = sys.argv[2]
spec_xml_url = sys.argv[3]
target_api = sys.argv[4]
target_profile = sys.argv[5]

print(f" Target API: {target_api}")
print(f" Target profile: {target_profile}")

opengl_comp_dir = os.path.join(os.path.dirname(__file__), "..")

def download_file(file_path: str, url: str):
    response = requests.get(url, stream=True)
    block_size = 1024

    print(f"Downloading {url}...")
    with open(file_path, "wb") as file:
        for data in response.iter_content(block_size):
            file.write(data)

print(f"{binary_dir=}")
specs_dir = os.path.join(binary_dir, "OpenGlSpecs")
print(f"{specs_dir=}")
os.makedirs(specs_dir, exist_ok=True)

spec_file_name = urllib.parse.urlparse(spec_xml_url)[2].rpartition("/")[2]
print(f"{spec_file_name=}")
spec_xml_path = os.path.join(specs_dir, spec_file_name)
print(f"{spec_xml_path=}")

if not os.path.exists(spec_xml_path):
    download_file(spec_xml_path, spec_xml_url)

spec_xml_tree = ET.parse(spec_xml_path)
spec_xml_root = spec_xml_tree.getroot()

class InterfaceElementsCollection:
    types: set[str]
    enums: set[str]
    commands: set[str]

class FeatureInfo:
    name: str
    number: Version # used for sorting
    requires: InterfaceElementsCollection
    removes: InterfaceElementsCollection

class ExtensionInfo:
    name: str
    requires: InterfaceElementsCollection
    removes: InterfaceElementsCollection

enabled_features: list[FeatureInfo] = []
enabled_extensions: list[ExtensionInfo] = []

def collect_interface_elements_collection(elements: list[ET.Element]) -> InterfaceElementsCollection:
    result = InterfaceElementsCollection()
    result.types = set()
    result.enums = set()
    result.commands = set()

    for element in elements:
        attribs = element.attrib

        if "api" in attribs and attribs.get("api") != target_api:
            continue
        if "profile" in attribs and attribs.get("profile") != target_profile:
            continue

        def extract_name(el: ET.Element) -> str:
            name = el.attrib.get("name")
            assert name is not None
            return name

        result.types = result.types.union(map(extract_name, element.findall("./type")))
        result.enums = result.enums.union(map(extract_name, element.findall("./enum")))
        result.commands = result.commands.union(map(extract_name, element.findall("./command")))

    return result

print(f" Checking available features...")
feature_elements = spec_xml_root.findall(".//feature")
for feature_element in feature_elements:
    attribs = feature_element.attrib

    api = attribs.get("api")
    name = attribs.get("name")
    number = attribs.get("number")

    assert api is not None and name is not None and number is not None

    if api == target_api:
        info = FeatureInfo()
        info.name = name
        info.number = Version(number)
        info.requires = collect_interface_elements_collection(feature_element.findall("./require"))
        info.removes = collect_interface_elements_collection(feature_element.findall("./remove"))

        enabled_features.append(info)

# Sorting features by number (they're already sorted in XML but let's be really safe)
enabled_features.sort(key=lambda i: i.number)

print(f"  {len(enabled_features)} features enabled")

print(f" Checking available extensions...")
extension_elements = spec_xml_root.findall(".//extensions/extension")
for extension_element in extension_elements:
    attribs = extension_element.attrib

    name = attribs.get("name")
    supported = attribs.get("supported")

    assert name is not None and supported is not None

    if target_api not in supported.split("|"):
        continue

    info = ExtensionInfo()
    info.name = name
    info.requires = collect_interface_elements_collection(extension_element.findall("./require"))
    info.removes = collect_interface_elements_collection(extension_element.findall("./remove"))

    enabled_extensions.append(info)

print(f"  {len(extension_elements)} extensions enabled")

required_enums: set[str] = set()
required_commands: set[str] = set()

for feature in enabled_features:
    for enum in feature.requires.enums:
        required_enums.add(enum)
    for command in feature.requires.commands:
        required_commands.add(command)

    for enum in feature.removes.enums:
        required_enums.remove(enum)
    for command in feature.removes.commands:
        required_commands.remove(command)

for extension in enabled_extensions:
    for enum in extension.requires.enums:
        required_enums.add(enum)
    for command in extension.requires.commands:
        required_commands.add(command)

    for enum in extension.removes.enums:
        required_enums.remove(enum)
    for command in extension.removes.commands:
        required_commands.remove(command)

print(f" Total enums count: {len(required_enums)}")
print(f" Total commands count: {len(required_commands)}")

print(" Begin generation...")

generated_root_dir = os.path.join(opengl_comp_dir, "Public", "MyGameEngine", "Generated")

types_header_path = os.path.join(generated_root_dir, "OpenGL", "Types.generated.hpp")
features_header_path = os.path.join(generated_root_dir, "OpenGL", "Features.generated.hpp")
features_def_file_path = os.path.join(generated_root_dir, "OpenGL", "Features.generated.def")
extensions_header_path = os.path.join(generated_root_dir, "OpenGL", "Extensions.generated.hpp")
extensions_def_file_path = os.path.join(generated_root_dir, "OpenGL", "Extensions.generated.def")
enums_header_path = os.path.join(generated_root_dir, "OpenGL", "Enums.generated.hpp")
commands_def_file_path = os.path.join(generated_root_dir, "OpenGL", "Commands.generated.def")

os.makedirs(os.path.dirname(types_header_path), exist_ok=True)
with open(types_header_path, "w+") as file:
    print(f" Generating {os.path.normpath(types_header_path)}...")

    file.write("#pragma once\n")

    type_elements = spec_xml_root.findall(".//types/type")
    for type_element in type_elements:
        if type_element.text is not None:
            file.write(type_element.text)

        for sub_element in type_element:
            if sub_element.tag == "apientry":
                file.write("GL_APIENTRY")
            if sub_element.text is not None:
                file.write(sub_element.text)
            if sub_element.tail is not None:
                file.write(sub_element.tail)
        file.write("\n")

os.makedirs(os.path.dirname(features_header_path), exist_ok=True)
with open(features_header_path, "w+") as file:
    print(f" Generating {os.path.normpath(features_header_path)}...")

    file.write("#pragma once\n")

    for feature in enabled_features:
        file.write(f"#define {feature.name} 1\n")

os.makedirs(os.path.dirname(features_def_file_path), exist_ok=True)
with open(features_def_file_path, "w+") as file:
    print(f" Generating {os.path.normpath(features_def_file_path)}...")

    for feature in enabled_features:
        file.write(f"_FEATURE({feature.name})\n")

os.makedirs(os.path.dirname(extensions_header_path), exist_ok=True)
with open(extensions_header_path, "w+") as file:
    print(f" Generating {os.path.normpath(extensions_header_path)}...")

    file.write("#pragma once\n")

    for extension in enabled_extensions:
        file.write(f"#define {extension.name} 1\n")

os.makedirs(os.path.dirname(extensions_def_file_path), exist_ok=True)
with open(extensions_def_file_path, "w+") as file:
    print(f" Generating {os.path.normpath(extensions_def_file_path)}...")

    for extension in enabled_extensions:
        file.write(f"_EXTENSION({extension.name})\n")

os.makedirs(os.path.dirname(enums_header_path), exist_ok=True)
with open(enums_header_path, "w+") as file:
    print(f" Generating {os.path.normpath(enums_header_path)}...")

    file.write("#pragma once\n")

    enum_elements = spec_xml_root.findall(".//enums/enum")
    for enum_element in enum_elements:
        attribs = enum_element.attrib

        name = attribs.get("name")
        value = attribs.get("value")

        assert name is not None and value is not None

        if name not in required_enums:
            continue

        file.write(f"#define {name} {value}\n")

os.makedirs(os.path.dirname(commands_def_file_path), exist_ok=True)
with open(commands_def_file_path, "w+") as file:
    print(f" Generating {os.path.normpath(commands_def_file_path)}...")

    command_elements = spec_xml_root.findall(".//commands/command")
    for command_element in command_elements:
        attribs = command_element.attrib

        name_element = command_element.find(".//name")
        assert name_element is not None

        command_name = name_element.text
        if command_name not in required_commands:
            continue

        proto_element = command_element.find("./proto")
        assert proto_element is not None

        return_type_decl = proto_element.text or ""
        for proto_sub_element in proto_element:
            if proto_sub_element.tag == "name":
                break

            if proto_sub_element.text is not None:
                return_type_decl += proto_sub_element.text
            if proto_sub_element.tail is not None:
                return_type_decl += proto_sub_element.tail

        params_decls: list[str] = []

        param_elements = command_element.findall("./param")
        for param_element in param_elements:
            param_decl = param_element.text or ""

            for param_sub_element in param_element:
                if param_sub_element.text is not None:
                    param_decl += param_sub_element.text
                if param_sub_element.tail is not None:
                    param_decl += param_sub_element.tail

            params_decls.append(param_decl)

        file.write(f"_COMMAND({command_name.strip()},{return_type_decl.strip()}")
        for param_decl in params_decls:
            file.write(f",{param_decl.strip()}")

        file.write(")\n")
