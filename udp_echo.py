import socket

# Simple UDP echo server

def udp_echo_server(port=12345):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", port))


    print("Server started on port {}".format(port))

    try:
        while True:
            data, addr = sock.recvfrom(1024)
            print(f"Received message from {addr}: {data.decode()}")
            sock.sendto(data, addr)
    except KeyboardInterrupt:
        sock.close()
        print("Server stopped.")


if __name__ == "__main__":
    udp_echo_server()
