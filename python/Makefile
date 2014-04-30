# The test Makefile to generate zip executable compiled python package
#
# Apr 30, 2014	Created by Joe Xue

CURDIR:=${CURDIR}
SRCDIR:=$(CURDIR)/src
BUILDDIR:=$(CURDIR)/builddir

SRCS:=$(shell find $(SRCDIR) -name *.py)
OBJS:=$(addsuffix .pyc,$(basename $(subst src,builddir,$(SRCS))))

.PHONY: pythontest

all: pythontest

pythontest: $(OBJS)
	@echo "$(SRCS)"
	@echo "$(OBJS)"
	cd $(BUILDDIR); zip  -r ../testpython.zip *
	echo "#!/bin/sh" > testpython
	echo "python3 testpython.zip" >> testpython
	chmod +x testpython

$(BUILDDIR)/%.pyc: $(SRCDIR)/%.py
	@echo $(@D)
	@mkdir -p $(@D)
	@python3 $(CURDIR)/compile  $< $@
clean:
	@rm -rf $(BUILDDIR)
	@rm testpython testpython.zip
