import socket
import threading


HOST = "127.0.0.1"
RECV_PORT = 11111
SEND_PORT = 22222


class ProxyServer:

    def __init__(self):
        self.mRecvSocket = None
        self.mSendSocket = None

    def __del__(self):
        if self.mRecvSocket:
            self.mRecvSocket.close()
        if self.mSendSocket:
            self.mSendSocket.close()

    def CreateRecvSocket(self, host: str, port: int) -> None:
        self.mRecvSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.mRecvSocket.bind((host, port))
        self.mRecvSocket.listen()
        print(f"recv socket created and listening at {host}:{port}")

    def CreateSendSocket(self, host: str, port: int) -> None:
        self.mSendSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.mSendSocket.bind((host, port))
        self.mSendSocket.listen()
        print(f"send socket created and listening at {host}:{port}")

    def ListenForRecvData(self) -> None:
        client, _ = self.mRecvSocket.accept()
        print("client connected to recv socket")
        while True:
            client.recv(0x1000)

    def ListenForSendData(self) -> None:
        client, _ = self.mSendSocket.accept()
        print("client connected to send socket")
        while True:
            client.recv(0x1000)


def Main():
    proxy_server = ProxyServer()
    proxy_server.CreateRecvSocket(HOST, RECV_PORT)
    proxy_server.CreateSendSocket(HOST, SEND_PORT)

    recv_thread = threading.Thread(target = ProxyServer.ListenForRecvData, args = [proxy_server])
    recv_thread.start()  
    send_thread = threading.Thread(target = ProxyServer.ListenForSendData, args = [proxy_server])
    send_thread.start()

    recv_thread.join()
    send_thread.join()


if __name__ == "__main__":
    Main()