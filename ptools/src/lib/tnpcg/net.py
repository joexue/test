#!/usr/bin/env python3

import select
import socket
import sys

class netConne:
	def __init__(self, ip, port):
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.s.connect((ip, int(port)))
		self.s.setblocking(0)
	def send(self, content):
		#print("Sending\n==========\n" + content + "\n==========\n")
		try:
			self.s.send(bytes(content, encoding="us-ascii"))
		except:
			return b"socket connection broken"
		timeout = 20
		readable , writable , exceptional = select.select([self.s], [], [self.s], 20)
		if not (readable or exceptional) :
			return b"Time out!"
		for s in readable :
			data = b''
			seg = "b"
			while( seg != ""):
				try:
					seg = s.recv(512)
					data = data + seg
				except:
					break
			return data
			#print("received:\n==========")
			#print(data.decode('utf-8'))
			#print("\n==========\nfrom " + str(s.getpeername()))
	def disconn(self):
		#self.s.shutdown()
		self.s.close()

if __name__ == "__main__":
	conn = netConne(sys.argv[1], sys.argv[2])
	data = conn.send("test")
	print(data.decode('utf-8'))
	conn.disconn()

