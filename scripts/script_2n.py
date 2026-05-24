#!/usr/bin/python3

import sys
import cv2
import numpy as np
import time


def process_image_from_stdin():
    # чтение бинарных данных STDIN
    image_bytes = sys.stdin.buffer.read()

    # декодирование изображения
    nparr = np.frombuffer(image_bytes, np.uint8)
    image = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

    if image is None:
        print("Error: Failed to decode image", file=sys.stderr)
        sys.exit(1)

    # эффект масляной живописи
    oil = cv2.xphoto.oilPainting(image, 7, 1)

    # кодирование результата в JPEG
    success, encoded_result = cv2.imencode(".jpg", oil, [cv2.IMWRITE_JPEG_QUALITY, 95])

    if not success:
        print("Error: Failed to encode image", file=sys.stderr)
        sys.exit(1)

    # вывод результата в STDOUT
    sys.stdout.buffer.write(encoded_result.tobytes())


if __name__ == "__main__":
    start = time.time()
    process_image_from_stdin()
    print(f"\nComplete in {time.time() - start:.2f}s", file=sys.stderr)
