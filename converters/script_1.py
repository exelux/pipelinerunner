import sys
import cv2
import time


def convert(input_path, output_path):
    image = cv2.imread(input_path)

    # тяжелый non-local means denoising (цветной)
    denoised = cv2.fastNlMeansDenoisingColored(image, None, 10, 10, 7, 21)

    # локальное выравнивание контраста через CLAHE на L-канале LAB
    lab = cv2.cvtColor(denoised, cv2.COLOR_BGR2LAB)
    l, a, b = cv2.split(lab)
    clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(8, 8))
    l = clahe.apply(l)
    lab = cv2.merge((l, a, b))
    result = cv2.cvtColor(lab, cv2.COLOR_LAB2BGR)

    # мягкий bilateral для выравнивания тонов перед следующими этапами
    result = cv2.bilateralFilter(result, 9, 75, 75)

    cv2.imwrite(output_path, result)
    return output_path


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: python3 <script> <input_path> <output_path>")
        sys.exit(1)

    start = time.time()
    convert(sys.argv[1], sys.argv[2])
    print(f"complete in {time.time() - start:.2f}s")
