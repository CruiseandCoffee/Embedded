import os
from PIL import Image, ImageSequence

# Create output folder if it doesn't exist
output_folder = "frames"
os.makedirs(output_folder, exist_ok=True)

# Open the GIF
im = Image.open("Eyes2.gif")
frame_num = 0

for frame in ImageSequence.Iterator(im):
    frame = frame.convert("RGB").resize((64, 32))
    pixels = frame.tobytes()
    output_path = os.path.join(output_folder, f"frame{frame_num}.raw")
    with open(output_path, "wb") as f:
        f.write(pixels)
    frame_num += 1
