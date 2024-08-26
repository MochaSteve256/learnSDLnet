import socket

def udp_client(host, port):
    # Create a UDP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    print(f"Sending messages to {host}:{port}. Type 'exit' to quit.")
    
    while True:
        # Get the message from the user
        try:
            message = input("You: ")
        except KeyboardInterrupt:
            print("\nExit")
            break
        
        if message.lower() == 'exit':
            print("Exit")
            break
        
        try:
            # Send the message to the server
            client_socket.sendto(message.encode(), (host, port))
            
            # Receive a response from the server (if any)
            response, server_address = client_socket.recvfrom(1024)
            print(f"Server: {response.decode()}")
        
        except Exception as e:
            print(f"An error occurred: {e}")
            break
    
    # Close the socket
    client_socket.close()

if __name__ == "__main__":
    # Replace with the IP address and port of the server you want to connect to
    HOST = "127.0.0.1"  # Localhost for testing
    PORT = 12345
    
    udp_client(HOST, PORT)
