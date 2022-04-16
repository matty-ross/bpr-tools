import socket
import struct
import importlib
import Handlers


HOST = "127.0.0.1"
PORT = 33333


class Analyzer:
    
    def __init__(self):
        self.mAnalyzerSocket = None

    def __del__(self):
        if self.mAnalyzerSocket:
            self.mAnalyzerSocket.close()

    def CreateAnalyzerSocket(self, host: str, port: int) -> None:
        self.mAnalyzerSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.mAnalyzerSocket.bind((host, port))
        self.mAnalyzerSocket.listen()
        print(f"analyzer socket created and listening at {host}:{port}")

    def ListenForData(self) -> None:
        client, _ = self.mAnalyzerSocket.accept()
        print("client connected to analyzer socket")
        while True:
            try:
                event_type = struct.unpack("<l", client.recv(4))[0]
                event_size = struct.unpack("<L", client.recv(4))[0]
                event_data = client.recv(event_size)
                importlib.reload(Handlers) # reload the Handlers module, so we can update it at runtime
                Handlers.HandleGameEvent(event_type, event_size, event_data)
            except Exception as e:
                print(e)


def Main():
    analyzer = Analyzer()
    analyzer.CreateAnalyzerSocket(HOST, PORT)
    analyzer.ListenForData()


if __name__ == "__main__":
    Main()