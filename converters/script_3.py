import sys
import cv2
import time


def convert(input_path, output_path):
    image = cv2.imread(input_path)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    edges = cv2.adaptiveThreshold(
        gray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 9, 9
    )
    color = cv2.bilateralFilter(image, 9, 300, 300)
    cartoon = cv2.bitwise_and(color, color, mask=edges)
    cv2.imwrite(output_path, cartoon)
    return output_path


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: python3 <script> <input_path> <output_path>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    start = time.time()
    convert(input_path, output_path)
    print(f"complete in {time.time() - start:.2f}s")
