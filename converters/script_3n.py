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

    # усиление деталей
    enhanced = cv2.detailEnhance(image, sigma_s=12, sigma_r=0.15)

    # сглаживание с сохранением границ
    smoothed = cv2.edgePreservingFilter(
        enhanced, flags=cv2.RECURS_FILTER, sigma_s=60, sigma_r=0.4
    )

    # контурная маска
    gray = cv2.cvtColor(smoothed, cv2.COLOR_BGR2GRAY)
    gray = cv2.medianBlur(gray, 7)

    edges = cv2.adaptiveThreshold(
        gray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 9, 2
    )

    edges_bgr = cv2.cvtColor(edges, cv2.COLOR_GRAY2BGR)

    # cartoon / ink эффект
    result = cv2.bitwise_and(smoothed, edges_bgr)

    # увеличение насыщенности
    hsv = cv2.cvtColor(result, cv2.COLOR_BGR2HSV).astype(np.float32)
    hsv[..., 1] = np.clip(hsv[..., 1] * 1.25, 0, 255)

    result = cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)

    # кодирование результата
    success, encoded_result = cv2.imencode(
        ".jpg", result, [cv2.IMWRITE_JPEG_QUALITY, 95]
    )

    if not success:
        print("Error: Failed to encode image", file=sys.stderr)
        sys.exit(1)

    # вывод результата
    sys.stdout.buffer.write(encoded_result.tobytes())


if __name__ == "__main__":
    start = time.time()
    process_image_from_stdin()
    print(f"\nComplete in {time.time() - start:.2f}s", file=sys.stderr)
