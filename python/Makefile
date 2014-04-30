# The test Makefile to generate zip executable compiled python package
#
# Apr 30, 2014	Created by Joe Xue

CURDIR:=${CURDIR}
SRCDIR:=$(CURDIR)/src
BUILDDIR:=$(CURDIR)/builddir

SRCS:=$(shell find $(SRCDIR) -name *.py)
OBJS:=$(addsuffix .pyc,$(basename $(subst src,builddir,$(SRCS))))

COMPILE="import sys\nimport py_compile\n\nif __name__ == \"__main__\":\n\tpy_compile.compile(sys.argv[1], sys.argv[2])"

.PHONY: pythontest compile clean

all: pythontest

pythontest: compile $(OBJS)
	@#echo "$(SRCS)"
	@#echo "$(OBJS)"
	@cd $(BUILDDIR); zip  -r ../testpython.zip *
	@echo "#!/bin/sh" > testpython
	@echo -n "python3 testpython.zip $$" >> testpython; echo "1" >> testpython
	@chmod +x testpython

compile:
	@- [ ! -e compile ] && echo  $(COMPILE) > compile

$(BUILDDIR)/%.pyc: $(SRCDIR)/%.py
	@#echo $(@D)
	@mkdir -p $(@D)
	@echo $< .. $@
	@python3 $(CURDIR)/compile  $< $@
clean:
	@-rm -rf $(BUILDDIR) 2>/dev/null
	@-rm -f testpython testpython.zip 2>/dev/null
	@-rm -f compile 2>/dev/null
