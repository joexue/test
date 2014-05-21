#!/bin/env python3

# A tnpcg like program
# This is a try to use python to create cross platform tools

# Change Logs:
# May 09, 2014	Created by Joe Xue

import sys
from tkinter import *
from tkinter import ttk

# our own libs
from lib.tnpcg.commfile import *
from lib.tnpcg.net import *

VER = "0.1.0"

LOG_BUF_LEN = 256

# Command manager class
class commandManager:
	def __init__(self, win):
		self.win = win
		self.clist = win.nametowidget("win.manual.list")
		self.save = win.nametowidget("win.manual.savebtn")
		self.send = win.nametowidget("win.manual.sendbtn")
		self.name = win.nametowidget("win.manual.name")
		self.content = win.nametowidget("win.manual.content")
		self.conbtn = win.nametowidget("win.manual.conbtn")
		self.commfile = commFile()
	def saveCommand(self):
		#self.send.state(['!disabled'])
		if(self.save.cget("text") == "Save Command"):
			self.commfile.addElement(self.name.get(), self.content.get(0.0, "end"))
			if  self.name.get() not in self.clist.get(0, "end"):
				self.clist.insert("end", self.name.get())
				#self.clist.selection_set("end")
				#self.idxs = self.clist.size()
		else:
			self.commfile.removeElement(self.name.get())
			self.clist.delete(self.idxs)

		for i in  range(0, self.clist.size(), 2):
			self.clist.itemconfigure(i, background='#f0f0ff')
		for i in  range(1, self.clist.size(), 2):
			self.clist.itemconfigure(i, background='#ffffff')
	def connectCommand(self):
		self.conn = netConne(self.win.nametowidget("win.manual.ip").get(), self.win.nametowidget("win.manual.port").get())
		self.conbtn.config(text = "Disconnect")
		self.send.state(['!disabled'])
	def commListDbClick(self, *args):
		self.name.delete(0, "end");
		self.idxs = self.clist.curselection()
		self.nameTxt = self.clist.get(self.idxs)
		self.name.insert(0, self.nameTxt)
		self.content.delete(0.0, "end")
		self.contentTxt = self.commfile.getContent(self.nameTxt)
		self.content.insert("end", self.contentTxt)
		self.save.state(['!disabled'])
		self.save.config(text = "Delete Command")
	def changeCommand(self, *args):
		if self.name.get() != self.nameTxt or \
			(bytes(self.content.get(0.0, "end"), encoding="us-ascii")).strip() != self.contentTxt and \
			self.name.get() != "New Command":
			self.save.config(text = "Save Command")
		else:
			self.save.config(text = "Delete Command")
	def sendCommand(self):
		self.send.state(['disabled'])
		content = self.content.get(0.0, "end")
		print("Sending\n==========")
		print(content)
		sys.stdout.flush()
		data = self.conn.send(content)
		print("Received\n==========")
		print(data.decode('utf-8'))
		self.send.state(['!disabled'])

# Make the window to center
def center(win):
	win.update_idletasks()
	width = win.winfo_width()
	height = win.winfo_height()
	x = (win.winfo_screenwidth() // 2) - (width // 2)
	y = (win.winfo_screenheight() // 2) - (height // 2)
	win.geometry('{}x{}+{}+{}'.format(width, height, x, y))

# Let the entry and text area generate the "Change" event
def generateOnChange(obj):
	obj.tk.eval('''
		proc widget_proxy {widget widget_command args} {
			# call the real tk widget command with the real args
			set result [uplevel [linsert $args 0 $widget_command]]
			# generate the event for certain types of commands
			#if {([lindex $args 0] in {insert replace delete}) ||
			#	([lrange $args 0 2] == {mark set insert}) ||
			#	([lrange $args 0 1] == {xview moveto}) ||
			#	([lrange $args 0 1] == {xview scroll}) ||
			#	([lrange $args 0 1] == {yview moveto}) ||
			#	([lrange $args 0 1] == {yview scroll})} {

			#	event generate  $widget <<Change>> -when tail
			#}
			if {([lindex $args 0] in {insert replace delete}) } {
				event generate  $widget <<Change>> -when tail
			}

			# return the result from the real widget command
			return $result
		}
		''')
	obj.tk.eval('''
		rename {widget} _{widget}
		interp alias {{}} ::{widget} {{}} widget_proxy {widget} _{widget}
		'''.format(widget = str(obj)))

# The redirect class
#import time
class redirector(object):
	def __init__(self, widget, msgtype):
		self.widget = widget
		self.msgtype = msgtype

	def write(self, string):
		if self.msgtype == 1:
			self.widget.insert("end", string, "info")
		else:
			self.widget.insert("end", string, "error")
		lines = len(self.widget.get(0.0, "end").splitlines())
		height = self.widget.cget('height')
		if lines > LOG_BUF_LEN:
			self.widget.delete(1.0, str(lines - LOG_BUF_LEN) + ".0")

		self.widget.see(END)

	def flush(self):
		self.widget.update()
		pass

def main(args = None):
	root = Tk()
	commfile = commFile()
	commands = commfile.getNames()
	cnames = StringVar(value = commands)
	root.title('Precidia Tools - tnpcg ' + VER)
	#root.geometry(str(root.winfo_screenwidth()//2) + "x" +  str(root.winfo_screenheight()//2) + "+" + str(root.winfo_screenwidth()//4)  + "+" + str(root.winfo_screenheight()//4))
	nb = ttk.Notebook(root, name = "win")
	nb.grid(sticky = (N, S, E, W))

	manual = ttk.Frame(nb, padding=(12,12,12,12), name = "manual")
	setup = ttk.Frame(nb, padding=(12,12,12,12), name = "setup")

	nb.add(manual, text='Manual')
	nb.add(setup, text='Setup')

	ttk.Label(manual, text="Commands List:").grid(column = 0, row = 0, sticky = (S, W))

	commandList = Listbox(manual, listvariable = cnames, name = "list")
	commandList.grid(column = 0, row = 1, rowspan = 7, sticky = (N, S, E, W))


	scrollbar = Scrollbar(manual)
	scrollbar.grid(column = 1, row = 1, rowspan = 7, sticky = (N, S, E, W))

	commandName = ttk.Entry(manual, foreground="blue", name = "name")
	commandContent = Text(manual, foreground="blue", name = "content")

	ttk.Label(manual, text = "Command Name:").grid(column = 2, row = 1, sticky = (S, W));
	commandName.grid(column = 2, row = 2, columnspan = 6, sticky = (W, E))
	ttk.Label(manual, text = "Command Content:").grid(column = 2, row = 3, sticky = (S, W))
	commandContent.grid(column = 2, row = 4, columnspan = 6, sticky = (W, E));

	ttk.Label(manual, text = "").grid(column = 3, row = 6)

	saveCommandBtn = ttk.Button(manual, text = "Save Command", state = "disabled", name = "savebtn")
	saveCommandBtn.grid(column = 4, row = 7, sticky = (S, E));

	sendCommandBtn = ttk.Button(manual, text = "Send Command", state = "disabled", name = "sendbtn")
	sendCommandBtn.grid(column = 7, row = 7, sticky = (S, E));

	for i in range(0,len(commands),2):
		commandList.itemconfigure(i, background='#f0f0ff')

	scrollbar.config( command = commandList.yview )
	commandList.configure(yscrollcommand = scrollbar.set)

	ttk.Label(manual, text = "").grid(column = 0, row = 8)

	ttk.Separator(manual, orient = HORIZONTAL).grid(column = 0, row = 9, columnspan = 9, sticky = "ew")

	ip = ttk.Entry(manual, width = 15, name = "ip")
	ip.grid(row = 11, column = 3, columnspan = 1, sticky = (S, W))
	ip.insert(0, "127.0.0.1")
	port = ttk.Entry(manual, width = 5, name = "port")
	port.insert(0, "9999")
	port.grid(row = 11, column = 4, columnspan = 1, sticky = (S, E))

	ttk.Checkbutton(manual, text='Use SSL', state='disable', name = "ssl").grid(row = 11, column = 6, sticky = (S, E))

	ttk.Label(manual, text = "").grid(column = 0, row = 10)

	ttk.Label(manual, text = "POSLynx Address: ").grid(row = 11, column = 2, columnspan = 1, sticky = (S, E))
	ttk.Label(manual, text = "        Port: ").grid(row = 11, column = 4, sticky = (S, W))

	connectCommandBtn = ttk.Button(manual, text="Connect", name = "conbtn")
	connectCommandBtn.grid(column = 7, row = 11, sticky = (S, E));

	ttk.Label(manual, text = "").grid(column = 0, row = 13)
	ttk.Separator(manual, orient = HORIZONTAL).grid(column = 0, row = 14, columnspan = 9, sticky = (E, W))
	logInfo = Text(manual, height = 10)
	ttk.Label(manual, text="Log Infomation:").grid(column = 0, row = 14, sticky = (S, W))
	logInfo.grid(column = 0, row = 15, columnspan = 8, sticky = (W, E))

	root.columnconfigure(0, weight = 1)
	root.rowconfigure(0, weight = 1)
	nb.columnconfigure(0, weight = 1)
	nb.rowconfigure(0, weight = 1)
	for i in range(16):
		manual.rowconfigure(i, weight = 1)
	for i in range(8):
		manual.columnconfigure(i, weight = 1)

	manual.columnconfigure(1, weight = 0)
	center(root)

	#install the callback functions
	commManager = commandManager(root)
	saveCommandBtn.config(command = commManager.saveCommand)
	sendCommandBtn.config(command = commManager.sendCommand)
	connectCommandBtn.config(command = commManager.connectCommand)
	commandList.bind('<Double-1>', commManager.commListDbClick)
	#commandList.bind('<Button-1>', commManager.commListDbClick)
	generateOnChange(commandName)
	generateOnChange(commandContent)
	commandName.bind('<<Change>>', commManager.changeCommand)
	commandContent.bind('<<Change>>', commManager.changeCommand)

	logInfo.tag_config("info", foreground="blue")
	logInfo.tag_config("error", foreground="red")
	sys.stdout = redirector(logInfo, 1)
	sys.stderr = redirector(logInfo, 2)
	windowSystem = root.tk.call("tk", "windowingsystem")
	if windowSystem == "win32":
		root.iconbitmap("precidia.ico")
	#img = PhotoImage(file='precidia.gif')
	#root.tk.call('wm', 'iconphoto', root._w, img)
	root.mainloop()

if __name__ == "__main__":
	sys.exit(main())
