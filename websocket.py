from coinbase.websocket import WSClient
import os
from dotenv import load_dotenv
import sys
import json

class Websocket:
    def __init__(self):
        self.sequence_num = 0
        load_dotenv()
        api_key = os.getenv("COINBASE_API_NAME")
        secret_key = os.getenv("COINBASE_PRIVATE_KEY")
        if secret_key:
            secret_key = secret_key.replace("\\n", "\n")

        self.ws_client = WSClient(api_key=api_key, api_secret=secret_key, on_message=self.on_message, verbose=False)

        self.pipe_path = "/tmp/datapipeline"
        if not os.path.exists(self.pipe_path):
            os.mkfifo(self.pipe_path)

        print("Waiting for C++ reader...")
        self.pipe = open(self.pipe_path, "w", buffering=1)
        print("Pipe opened for writing.")

    def on_message(self, msg):
        msg_json = json.loads(msg)
        update_seq_num = msg_json["sequence_num"]
        if self.sequence_num != update_seq_num:
            print("Sequence Number is Wrong")
            return
        self.sequence_num += 1

        try:
            self.pipe.write(json.dumps(msg_json) + "\n")
            self.pipe.flush()
        except Exception as e:
            print("Closing websocket and pipeline")
            self.ws_client.close()
            self.pipe.close()

    def run(self, symbol, duration):
        self.ws_client.open()
        self.ws_client.subscribe([symbol], ["level2"])
        self.ws_client.sleep_with_exception_check(duration)
        print("Closing websocket and pipeline")
        self.ws_client.close()
        self.pipe.close()


if __name__ == "__main__":
    print("Starting WebSocket...")
    if len(sys.argv) > 2:
        product_id = sys.argv[1]
        duration = int(sys.argv[2])
        socket = Websocket()
        socket.run(product_id, duration)
    else:
        print("No argument received.")
