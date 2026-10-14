import sys
import os

directory_path = sys.argv[1]
os.makedirs(directory_path, exist_ok=True)

with open(os.path.join(directory_path, ".gitignore"), "w+") as file:
  file.write(
    "# This directory contains generated files and should not be commited to your VCS\n" +
    "*\n")
