#!/usr/bin/python

# serial test scripts
# use /dev/ttyS0 as test port

# created by Joe Xue    01 24, 2011

import sys, os, random
import serial
from threading import Thread

candidates = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%^&*<>"
recv_content = ''
end = False

def serial_recv(ser, length):
    global end
    global recv_content
    end = False
    recv_content = ''
    recv_content = ser.read(length)
    end = True

def serial_send(ser, content):
    ser.write(content)
    
def serial_test(lenght, counts):
    global end
    global recv_content
    success = counts
    ser = serial.Serial(port = "/dev/ttyS0", timeout = 5, baudrate=9600)
    #ser.open(port = "/dev/ttyS0", timeout= 5)
    for i in range(counts):
        Thread(target=serial_recv, args=(ser,length)).start()
        test_string =''.join(candidates[random.randint(0,len(candidates)-1)] for x in range(length-1))
        test_string = test_string + "\n"
        test_string = "12345\r\nabcd\r\nend\n~"
        #test_string = "xyz\r123\r"
        print test_string
        #wait for recv end
        serial_send(ser, test_string)

        while(end == False):
            pass

        print recv_content;
	for i in range(length):
		print hex(ord(recv_content[i])), "->" , recv_content[i]
	#if cmp(test_string[:-1], recv_content[:-1]) != 0:
	if cmp(test_string, recv_content) != 0:
            print "Not match send=%d recv=%d" %(length, len(recv_content))
            success -= 1

        ser.flushInput()
        ser.flushOutput()
    ser.close()
    print "%d of %d is matched!" %(success, counts)

if __name__ == '__main__':
    if(len(sys.argv) < 3):
        print "usage: %s bytes, counts" %(sys.argv[0])
    else:
        length = int(sys.argv[1])
        counts = int(sys.argv[2])
        serial_test(length, counts)
        pass


