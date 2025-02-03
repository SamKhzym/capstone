import socket

# Server details
HOST = '192.0.2.1'  # Server's IP address or hostname
PORT = 4242         # Server's port number

# Message to send
MESSAGE = "Hello, Server!"

# Create a TCP socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:

    while True:

        try:
            # Connect to the server
            client_socket.connect((HOST, PORT))
            print(f"Connected to {HOST}:{PORT}")
            
            # Send data
            client_socket.sendall(MESSAGE.encode('utf-8'))
            print(f"Sent: {MESSAGE}")
            
            # Optional: Receive response from the server
            response = client_socket.recv(1024)  # Buffer size
            print(f"Received: {response.decode('utf-8')}")

        except Exception as e:
            print(f"An error occurred: {e}")
