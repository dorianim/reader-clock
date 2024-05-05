import os

Import("env")

os.chdir("web")
os.system("./compress.sh")
