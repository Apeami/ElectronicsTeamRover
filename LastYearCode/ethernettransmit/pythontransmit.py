import socket
import sys

UDP_IP ="192.168.1.91"#Arduino
UDP_PORT = 1234
UDP_PORTREC = 4321
UDP_IPREC ="192.168.1.69" #Computer

MESSAGE = b"Hello, WORLD!"

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.bind((UDP_IPREC, UDP_PORTREC))

while True:
    sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))



print("UDP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message: %s" % MESSAGE)



while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print("received message: %s" % data)