#!/usr/bin/python3

import sys
import cv2
import numpy as np
import time


def process_image_from_stdin():
    # чтение бинарных данных STDIN
    image_bytes = sys.stdin.buffer.read()

    # получение изображения
    nparr = np.frombuffer(image_bytes, np.uint8)
    image = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

    if image is None:
        print("Error: Failed to decode image", file=sys.stderr)
        sys.exit(1)

    # обработка (оригинальный код)
    denoised = cv2.fastNlMeansDenoisingColored(image, None, 10, 10, 7, 21)

    lab = cv2.cvtColor(denoised, cv2.COLOR_BGR2LAB)
    l, a, b = cv2.split(lab)
    clahe = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(8, 8))
    l = clahe.apply(l)
    lab = cv2.merge((l, a, b))
    result = cv2.cvtColor(lab, cv2.COLOR_LAB2BGR)

    result = cv2.bilateralFilter(result, 9, 75, 75)

    # сохранение изображения STDOUT
    _, encoded_result = cv2.imencode(".jpg", result, [cv2.IMWRITE_JPEG_QUALITY, 95])
    sys.stdout.buffer.write(encoded_result.tobytes())


if __name__ == "__main__":
    start = time.time()
    process_image_from_stdin()
    print(f"\nComplete in {time.time() - start:.2f}s", file=sys.stderr)
