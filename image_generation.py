
import os
import subprocess
from pathlib import Path

CPP_FOLDER = "./CppFiles"
BIN_FOLDER = "./BinFiles"
IMG_FOLDER = "./Images"

os.makedirs(BIN_FOLDER, exist_ok=True)
os.makedirs(IMG_FOLDER, exist_ok=True)

def binary_to_image(bin_path, output_path):
    from PIL import Image

    binary_value = []

    with open(bin_path, "rb") as file:
        data = file.read(1)
        while data:
            binary_value.append(ord(data))
            data = file.read(1)

    size = len(binary_value)

    if size < 10240:
        width = 64
    elif 10240 <= size <= 10240 * 3:
        width = 128
    elif 10240 * 50 <= size <= 10240 * 100:
        width = 768
    else:
        width = 1024

    height = (size // width) + 1

    image = Image.new('L', (width, height))
    image.putdata(binary_value)

   
    image.save(output_path)
    image.show()


for cpp_file in Path(CPP_FOLDER).glob("*.cpp"):
    filename = cpp_file.stem
    exe_file = Path(BIN_FOLDER) / f"{filename}.exe"
    bin_file = Path(BIN_FOLDER) / f"{filename}.bin"
    img_file = Path(IMG_FOLDER) / f"{filename}.png"

    print(f"[+] Processing {cpp_file.name}")

    try:
        obj_file = Path(BIN_FOLDER) / f"{filename}.o"
        subprocess.run(["g++", "-c", cpp_file, "-o", obj_file], check=True)


        subprocess.run([
            "objcopy",
            "-O", "binary",
            "--only-section=.text",
            obj_file,
            bin_file
        ], check=True)

        binary_to_image(bin_file, img_file)

    except subprocess.CalledProcessError as e:
        print(f"[!] Error processing {cpp_file.name}: {e}")