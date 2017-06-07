import socket
import struct

TCP_IP = '127.0.0.1'
TCP_PORT = 8000
message = "create-lobby"

def send_str(s, message):
	s.sendto(struct.pack('!i', len(message)),(TCP_IP, TCP_PORT))
	s.sendto(message.encode(),(TCP_IP, TCP_PORT))
	# s.send(struct.pack('!i', len(message)))
	# s.send(message)

def recv_str(s):
	data_len = struct.unpack('!i', s.recv(4))
	print (data_len[0])
	return s.recv(data_len[0])

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
send_str(s, message)
data = recv_str(s)
print ("Recibi: ", data)

while True:
	nb = input('Enter next command: ')
	send_str(s, nb)
	data = recv_str(s)
	print ("Recibi: ", data)