#!/usr/bin/env python3
import os
import time
from pathlib import Path

SRC = Path("/mnt/shared/storage/nifi_src")
DST = Path("/mnt/shared/storage/nifi_result")

# сбор имен файлов
files = {f.name for f in SRC.iterdir() if f.is_file()}
total = len(files)
print(f"Найдено файлов в {SRC}: {total}")

# ожидание пока файлы начнут исчезать
while all((SRC / f).exists() for f in files):
    time.sleep(0.5)

start = time.perf_counter()

remaining_src = set(files)
remaining_dst = set(files)
processed = 0

while remaining_dst:
    # проверка исчезновения из src
    for f in list(remaining_src):
        if not (SRC / f).exists():
            print(f"Исчез из src: {f}")
            remaining_src.remove(f)

    # проверка появления в dst
    for f in list(remaining_dst):
        if (DST / f).exists():
            processed += 1
            print(f"[{processed}/{total}] готово: {f}")
            remaining_dst.remove(f)

    time.sleep(0.5)


elapsed = time.perf_counter() - start
mins, secs = divmod(elapsed, 60)
print(f"Всего обработано: {total}")
print(f"Общее время выполнения: {int(mins)} мин {secs:.2f} сек ({elapsed:.2f} сек)")
