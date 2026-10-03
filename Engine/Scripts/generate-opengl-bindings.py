import requests
import os
import shutil
import xml.etree.ElementTree as ET

headers_dir = os.path.join(os.path.dirname(__file__), "..", "Source", "Graphics", "OpenGL", "Public", "MyGameEngine", "Graphics", "OpenGL")
templates_dir = os.path.join(os.path.dirname(__file__), "..", "Source", "Graphics", "OpenGL", "Templates")

GL_HEADER_PATH = os.path.join(headers_dir, "OpenGL.hpp")
WGL_HEADER_PATH = os.path.join(headers_dir, "WGL.hpp")

GL_API_HEADER_TEMPLATE_PATH = os.path.join(templates_dir, "OpenGLAPI.hpp")
GL_API_HEADER_PATH = os.path.join(headers_dir, "OpenGLAPI.hpp")

WGL_API_HEADER_TEMPLATE_PATH = os.path.join(templates_dir, "WGLAPI.hpp")
WGL_API_HEADER_PATH = os.path.join(headers_dir, "WGLAPI.hpp")

KHR_DIR_PATH = os.path.join(os.path.dirname(__file__), "..", "Source", "Graphics", "OpenGL", "Public", "KHR")
KHR_PLATFORM_HEADER_PATH = os.path.join(os.path.dirname(__file__), "..", "Source", "Graphics", "OpenGL", "Public", "KHR", "khrplatform.h")

if not os.path.exists(KHR_DIR_PATH):
  os.mkdir(KHR_DIR_PATH)

temp_dir_path = os.path.join(os.path.dirname(__file__), "generate-opengl-bindings.dir")

if os.path.exists(temp_dir_path):
  shutil.rmtree(temp_dir_path)

os.mkdir(temp_dir_path)
with open(os.path.join(temp_dir_path, ".gitignore"), "w") as file:
  file.write("*")

def download_file(url: str, filepath: str):
  block_size = 1024

  response = requests.get(url)
  with open(filepath, "wb") as file:
    for data in response.iter_content(block_size):
      file.write(data)

print("Downloading KHR/khrplatform.h")
download_file("https://registry.khronos.org/EGL/api/KHR/khrplatform.h", KHR_PLATFORM_HEADER_PATH)

print("Downloading gl.xml")
download_file("https://github.com/KhronosGroup/OpenGL-Registry/raw/refs/heads/main/xml/gl.xml",
              os.path.join(temp_dir_path, "gl.xml"))

print("Downloading wgl.xml")
download_file("https://github.com/KhronosGroup/OpenGL-Registry/raw/refs/heads/main/xml/wgl.xml",
              os.path.join(temp_dir_path, "wgl.xml"))

def get_c_type(parent_element):
    """
    Extracts the C type from a <proto> or <param> tag by ignoring <name> tags.
    Stitches together text and tail properties.
    """
    type_str = ""
    if parent_element.text:
        type_str += parent_element.text

    for child in parent_element:
        if child.tag != "name": # Keep <ptype>, skip <name>
            if child.text:
                type_str += child.text
        if child.tail:
            type_str += child.tail

    return " ".join(type_str.split())

def parse_khronos_xml(xml_path, file):
    """
    Parses the XML, writes types/enums/signatures to 'file',
    and returns a list of successfully generated command names.
    """
    tree = ET.parse(xml_path)
    root = tree.getroot()

    # 1. Parse Enums globally
    enums_dict = {}
    for enums_group in root.findall('./enums'):
        for child in enums_group.findall('enum'):
            name = child.attrib.get('name')
            val = child.attrib.get('value')
            if name and val:
                enums_dict[name] = val

    # 2. Parse Commands globally to get their signatures
    command_signatures = {}

    # First pass: Commands with explicit <proto> and <param>s
    for cmd in root.findall('./commands/command'):
        proto = cmd.find('proto')
        if proto is not None:
            name_elem = proto.find('name')
            if name_elem is not None:
                name = name_elem.text
                ret_type = get_c_type(proto)
                params = [get_c_type(p) for p in cmd.findall('param')]
                command_signatures[name] = (ret_type, params)

    # Second pass: Commands defined purely as aliases
    for cmd in root.findall('./commands/command'):
        name = cmd.attrib.get('name')
        alias = cmd.attrib.get('alias')
        if name and alias and name not in command_signatures:
            if alias in command_signatures:
                command_signatures[name] = command_signatures[alias]

    # 3. Filter features to skip OS-provided basics
    active_enums = set()
    active_commands = set()

    for feature in root.findall('./feature'):
        api = feature.attrib.get('api', '')
        version = feature.attrib.get('number', '')

        # Skip versions natively provided by the Windows SDK
        if api == 'gl' and version in ['1.0', '1.1']:
            continue
        if api == 'wgl' and version == '1.0':
            continue

        for req in feature.findall('./require'):
            for enum in req.findall('enum'):
                active_enums.add(enum.attrib.get('name'))
            for cmd in req.findall('command'):
                active_commands.add(cmd.attrib.get('name'))

    # Include all extensions
    for ext in root.findall('./extensions/extension'):
        for req in ext.findall('./require'):
            for enum in req.findall('enum'):
                active_enums.add(enum.attrib.get('name'))
            for cmd in req.findall('command'):
                active_commands.add(cmd.attrib.get('name'))

    # --- CODE GENERATION ---

    file.write("//\n")
    file.write("// Types\n")
    file.write("//\n\n")

    for type_node in root.findall('./types/type'):
        type_str = "".join(type_node.itertext()).strip()
        if "typedef" in type_str or type_str.startswith("DECLARE_HANDLE"):
            file.write(type_str + "\n")

    file.write("\n//\n")
    file.write("// Enums (Safe Macros)\n")
    file.write("//\n\n")

    for name in sorted(active_enums):
        if name in enums_dict:
            val = enums_dict[name]
            file.write(f"#ifndef {name}\n")
            file.write(f"#define {name} {val}\n")
            file.write(f"#endif\n")

    file.write("\n//\n")
    file.write("// Commands\n")
    file.write("//\n\n")

    generated_commands = []
    for name in sorted(active_commands):
        if name in command_signatures:
            ret_type, params = command_signatures[name]
            param_str = ", ".join(params) if params else "void"
            file.write(f"using {name}_t = {ret_type}(KHRONOS_APIENTRY*)({param_str});\n")
            generated_commands.append(name)

    return generated_commands

def process_api_template(template_path, output_path, placeholder, commands):
    """
    Reads the template, replaces the placeholder with class members,
    and writes to the output path.
    """
    with open(template_path, 'r') as file:
        content = file.read()

    # Generate the C++ class members with a 2-space indent for lines after the first one
    # (The first line naturally inherits the indentation of the placeholder in the template)
    members_list = [f"{cmd}_t {cmd} = nullptr;" for cmd in commands]
    members_string = "\n  ".join(members_list)

    content = content.replace(placeholder, members_string)

    with open(output_path, 'w') as file:
        file.write(content)

print("Generating %s" % GL_HEADER_PATH)
with open(GL_HEADER_PATH, "w") as file:
  file.write("#pragma once\n" +
             "// Auto-generated file\n\n")

  # Actually specified in XML file, but we will ignore it there
  file.write("#include <KHR/khrplatform.h>\n\n")
  gl_commands = parse_khronos_xml(os.path.join(temp_dir_path, "gl.xml"), file)

print("Generating %s" % GL_API_HEADER_PATH)
process_api_template(GL_API_HEADER_TEMPLATE_PATH, GL_API_HEADER_PATH, "<<$GLMETHODS$>>", gl_commands)

print("Generating %s" % WGL_HEADER_PATH)
with open(WGL_HEADER_PATH, "w") as file:
  file.write("#pragma once\n" +
             "// Auto-generated file\n\n")

  file.write("#include <KHR/khrplatform.h>\n")
  file.write("#include <MyGameEngine/Graphics/OpenGL/OpenGL.hpp>\n")
  file.write("#include <MyGameEngine/win32api/base.hpp>\n\n")
  wgl_commands = parse_khronos_xml(os.path.join(temp_dir_path, "wgl.xml"), file)

print("Generating %s" % WGL_API_HEADER_PATH)
process_api_template(WGL_API_HEADER_TEMPLATE_PATH, WGL_API_HEADER_PATH, "<<$WGLMETHODS$>>", wgl_commands)
