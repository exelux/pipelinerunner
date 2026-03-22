import pika
import json
import uuid
import pathlib


def collect_files(src_folder):
    src_path = pathlib.Path(src_folder)

    files = []
    for file_path in src_path.iterdir():
        if file_path.is_file():
            files.append(f"src/{file_path.name}")

    return files


def send(data):
    connection = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
    channel = connection.channel()
    channel.queue_declare(queue="src_queue", durable=True)

    channel.basic_publish(
        exchange="", routing_key="src_queue", body=json.dumps(data, ensure_ascii=False)
    )

    print(f"json sent: {json.dumps(data, ensure_ascii=False, indent=2)}")

    connection.close()


if __name__ == "__main__":
    files = collect_files("/home/smrnv/storage/src")

    for file in files:
        message_data = {"message_id": str(uuid.uuid4()), "input_file": file}
        send(message_data)
