import socket
import struct
import re

class Socket():
    
    def __init__(self, HOST, PORT):
        # Create a socket object for IPv6
        self.client_socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        
        self.client_socket.connect((HOST, PORT, 0, 0))  # The last two arguments are for IPv6-specific address info
        self.client_socket.settimeout(10.0)

        print(f"Connected to {HOST} on port {PORT}")
    
    def close_connection(self):
        self.client_socket.close()
        print("Connection closed")

    def send_environment_info(self, lead_speed, set_speed, lead_distance):
        
        payload = "<{:.3f}|{:.3f}|{:.3f}>".format(lead_speed, lead_distance, set_speed)
        
        self.client_socket.send(payload.encode('utf-8'))

    def send_speed_request(self, speed_req):
        
        payload = str(speed_req)
        
        print(f"Sending payload: " + payload)
        self.client_socket.send(payload.encode('utf-8'))
    
    def receive_confirmation_message(self):
        
        #TODO Check recv payload size
        payload = self.client_socket.recv(128)
        
        decoded_str = payload.decode('utf-8', errors='ignore')
        match = re.search(r'\d+\.\d+', decoded_str)  # Look for '0.xx' pattern
        if match:
            ego_speed = float(match.group())  # Convert to float
        else:
            print("No speed info avaliable.")
            ego_speed = 0.0

        print(f"Ego Speed: {ego_speed}")
        return ego_speed
        
    def receive_vehicle_speed(self):
        
        #TODO Check recv payload size
        payload = self.client_socket.recv(16)
        
        decoded_str = payload.decode('utf-8', errors='ignore')

        # Extract the numeric part (assuming it's enclosed in '<' and '>')
        match = re.search(r'<([\d\.]+)>', decoded_str)
        if match:
            vehicle_speed = float(match.group(1))
        else:
            print("No valid float found in payload.")
        
        # payload = payload.decode().strip('<>\x00\x08')
        
        # vehicle_speed = float(payload)
        return vehicle_speed