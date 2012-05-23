#!/usr/bin/python
import SocketServer

class MyTCPHandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        print "%s wrote %d bytes:" % (self.client_address[0], len(self.data))
	for i in range(len(self.data)):
		print ord(self.data[i])
        print self.data
        # just send back the same data, but upper-cased
        #self.request.send(self.data.upper())

if __name__ == "__main__":
    HOST, PORT = "192.168.4.253", 2222

    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()
