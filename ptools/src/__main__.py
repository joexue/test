#!/bin/env python3
import sys

if (len(sys.argv) == 1):
	print("Pleas give the program name to run")
	print("usage: %s <program name>" %(sys.argv[0]))
	print("Programs:")
	print("	tnpcg	- The transac gateway tool")
	exit(0)

if sys.argv[1] == "tnpcg":
	import tnpcg
	tnpcg.main()
else:
	print("No program is named %s" %(sys.argv[1]))

