# Usage:
#  python <the-script> <Path/To/Output/archive.gamefiles> <Path/To/Archive/definition.json>

import sys
import json

archive_path = sys.argv[1]
definition_path = sys.argv[2]

definition = json.load(open(definition_path))

output = open(archive_path, "wb+")

output.write(b"A4PF") # magic bytes
output.write(b"\x01") # format version

assets = definition["assets"]

output.write(len(assets).to_bytes(4, "little"))

strigns_table_offset_offset = output.tell()
output.write((0).to_bytes(8, "little")) # reserve the space for "strigns_table_offset"

files_descs_begin = output.tell()

file_desc_size = 4 + 8 + 8
content_begin = files_descs_begin + (file_desc_size * len(assets))

# Allocate the space
output.write(b"\x00" * (content_begin - output.tell()))

strings: list[str] = []

next_asset_begin = content_begin
for i,asset in enumerate(assets):
  name = asset["name"]
  source = asset["source"]

  match source:
    case "file":
      file_path = asset["file-path"]
      content = open(file_path, "rb").read()
    case _:
      raise RuntimeError("unknown asset source %s" % source)

  asset_begin = next_asset_begin
  output.seek(asset_begin)
  output.write(content)
  next_asset_begin = output.tell()

  assert (next_asset_begin - asset_begin) == len(content)

  name_index = len(strings)
  strings.append(name)

  # Write the description
  output.seek(files_descs_begin + (file_desc_size * i))
  output.write(name_index.to_bytes(4, "little"))
  output.write(len(content).to_bytes(8, "little"))
  output.write(asset_begin.to_bytes(8, "little"))

# Write strings table

strings_table_begin = next_asset_begin
output.seek(next_asset_begin)

output.write(len(strings).to_bytes(4, "little"))
for string in strings:
  output.write(len(string).to_bytes(4, "little"))
  output.write(string.encode("utf-8"))

# Set strings table offset in the header

output.seek(strigns_table_offset_offset)
output.write(strings_table_begin.to_bytes(8, "little"))

output.flush()
output.close()
