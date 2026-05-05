import sys
import cv2
import numpy as np
import time


def convert(input_path, output_path):
    image = cv2.imread(input_path)

    # усиление деталей (edge-preserving, тяжелый проход)
    enhanced = cv2.detailEnhance(image, sigma_s=12, sigma_r=0.15)

    # доп. edge-preserving filter для "гладких" областей
    smoothed = cv2.edgePreservingFilter(
        enhanced, flags=cv2.RECURS_FILTER, sigma_s=60, sigma_r=0.4
    )

    # контурная маска через адаптивный порог по градациям серого
    gray = cv2.cvtColor(smoothed, cv2.COLOR_BGR2GRAY)
    gray = cv2.medianBlur(gray, 7)
    edges = cv2.adaptiveThreshold(
        gray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 9, 2
    )
    edges_bgr = cv2.cvtColor(edges, cv2.COLOR_GRAY2BGR)

    # перемножение - темные линии поверх изображения (эффект cartoon/ink)
    result = cv2.bitwise_and(smoothed, edges_bgr)

    # легкое увеличение насыщенности в HSV
    hsv = cv2.cvtColor(result, cv2.COLOR_BGR2HSV).astype(np.float32)
    hsv[..., 1] = np.clip(hsv[..., 1] * 1.25, 0, 255)
    result = cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)

    cv2.imwrite(output_path, result)
    return output_path


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: python3 <script> <input_path> <output_path>")
        sys.exit(1)

    start = time.time()
    convert(sys.argv[1], sys.argv[2])
    print(f"complete in {time.time() - start:.2f}s")
