#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import sys
import os

class Singleton(object):
	def __new__(cls, *args, **kw):
		if not hasattr(cls, '_instance'):
			orig = super(Singleton, cls)
			cls._instance = orig.__new__(cls, *args, **kw)
		return cls._instance

class commFile(Singleton):
	def __init__(self):
		path = sys.path[0]
		self.fileName = path[0:(path.rfind(os.sep) + 1)] + 'command.xml'
		self.tree = ET.parse(self.fileName)
		self.root = self.tree.getroot()

	def getNames(self):
		names=["New Command"]
		for child in self.root:
			names.append(child.attrib['name'])
		return tuple(names)
	def getContent(self, name):
		cont = ""
		for child in self.root:
			#print(name, child.tag, child.text, child.find('content').find("rank").text)
			if child.attrib['name'] == name:
				cont = ET.tostring(child)
				cont = cont[cont.find(b">", 2) + 1:]
				cont = cont[:cont.rfind(b"<", 0, -2)]
				cont = cont.translate(None, b'\t')
				cont = cont.strip(b'\n')
				break
		return cont
	def removeElement(self, name):
		for child in self.root:
			if child.attrib['name'] == name:
				self.root.remove(child)
				self.tree.write(self.fileName)
				break
	def addElement(self, name, content):
		#for child in self.root:
		#	if child.attrib['name'] == name:
		#		self.root.remove(child)
		#		break
		self.removeElement(name)
		content='<content>' + content + '</content>'
		new = ET.Element('command', name = name)
		#new.text = bytes(content, encoding="us-ascii")
		new1 = ET.fromstring(content)
		for child in new1:
			new.append(child)
		self.root.append(new)
		self.tree.write(self.fileName)

if __name__ == "__main__":
	comm = commFile()
	comm1 = commFile()
	if(id(comm)==id(comm1)):
		print("Same")
	else:
		print("Different")
	print(comm.getNames())
	print(comm.getContent('Singapore'))
	comm.removeElement("test")
	comm.addElement('test', "<test>123</test><test1>1234</test1>")

