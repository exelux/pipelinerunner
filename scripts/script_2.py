import sys
import cv2
import time


def convert(input_path, output_path):
    image = cv2.imread(input_path)

    # эффект масляной живописи из xphoto: size=7 - радиус окна, dynRatio=1 - квантование интенсивности
    oil = cv2.xphoto.oilPainting(image, 7, 1)

    cv2.imwrite(output_path, oil)
    return output_path


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: python3 <script> <input_path> <output_path>")
        sys.exit(1)

    start = time.time()
    convert(sys.argv[1], sys.argv[2])
    print(f"complete in {time.time() - start:.2f}s")
