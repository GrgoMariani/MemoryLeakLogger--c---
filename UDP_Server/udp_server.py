import random
import socket


"""
    This is a very simple UDP socket server.
    It catches all of the packages send to this port and prints them.
"""

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(('', 12000))

while True:
    message, address = server_socket.recvfrom(8192)
    print(message.decode("UTF-8"))
