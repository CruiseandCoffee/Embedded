import os
import cv2

# Create output folder if it doesn't exist
output_folder = "frames"
os.makedirs(output_folder, exist_ok=True)

# Open the video file
video_path = "Robot_Eyes.mp4"
cap = cv2.VideoCapture(video_path)

frame_num = 0

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break  # No more frames

    # Resize the frame to 64x32
    frame = cv2.resize(frame, (64, 32))

    # Convert from BGR (OpenCV default) to RGB
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Convert frame to bytes
    pixels = frame_rgb.tobytes()

    # Save to .raw file
    output_path = os.path.join(output_folder, f"frame{frame_num}.raw")
    with open(output_path, "wb") as f:
        f.write(pixels)

    frame_num += 1

cap.release()
print(f"Extracted {frame_num} frames.")
