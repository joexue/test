#!/bin/env python3
import sys

if (len(sys.argv) == 1):
	print("Pleas give the program name to run")
	exit(1)

if sys.argv[1] == "test1":
	import test1
elif sys.argv[1] == "test2":
	import test2
else:
	print("No program is named %s" %(sys.argv[1]))

