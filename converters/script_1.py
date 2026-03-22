import sys
import cv2
import time
import sklearn
import numpy as np


def convert(input_path, output_path):
    image = cv2.imread(input_path)
    h, w = image.shape[:2]
    pixels = image.reshape(-1, 3)
    kmeans = sklearn.cluster.KMeans(n_clusters=8, random_state=42, n_init=10)
    labels = kmeans.fit_predict(pixels)
    quantized = kmeans.cluster_centers_[labels].reshape(h, w, 3).astype(np.uint8)
    cv2.imwrite(output_path, quantized)
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
