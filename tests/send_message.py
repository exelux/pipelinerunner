import pika
import json
import uuid
import pathlib
import time

SRC_FOLDER = "/mnt/shared/storage/src"
SRC_QUEUE = "src_queue"
RESULT_QUEUE = "converter_3_queue"


def main():
    files = [f.name for f in pathlib.Path(SRC_FOLDER).iterdir() if f.is_file()]
    expected = {pathlib.Path(f).stem for f in files}
    total = len(expected)

    start = time.perf_counter()

    connection = pika.BlockingConnection(
        pika.ConnectionParameters("localhost", heartbeat=600)
    )
    channel = connection.channel()

    # отправка
    for file in files:
        data = {"message_id": str(uuid.uuid4()), "input_file": f"src/{file}"}
        channel.basic_publish(
            exchange="",
            routing_key=SRC_QUEUE,
            body=json.dumps(data, ensure_ascii=False),
        )
    print(f"Отправлено сообщений в {SRC_QUEUE}: {total}")

    # ожидание результатов
    print(f"Ожидание результатов из {RESULT_QUEUE} (всего: {total})...")
    processed = set()

    def callback(ch, method, _props, body):
        input_file = json.loads(body).get("input_file", "")
        match = next((name for name in expected if input_file.startswith(name)), None)
        if match and match not in processed:
            processed.add(match)
            print(f"[{len(processed)}/{total}] готово: {match}")
        ch.basic_ack(delivery_tag=method.delivery_tag)
        if len(processed) >= total:
            ch.stop_consuming()

    channel.basic_consume(queue=RESULT_QUEUE, on_message_callback=callback)
    channel.start_consuming()
    connection.close()

    elapsed = time.perf_counter() - start
    mins, secs = divmod(elapsed, 60)
    print(f"Общее время выполнения: {int(mins)} мин {secs:.2f} сек ({elapsed:.2f} сек)")


if __name__ == "__main__":
    main()
