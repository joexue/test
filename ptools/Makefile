# The test Makefile to generate zip executable compiled python package
#
# Apr 30, 2014	Created by Joe Xue

CURDIR:=${CURDIR}
SRCDIR:=$(CURDIR)/src
BUILDDIR:=$(CURDIR)/builddir
RELDIR:=$(CURDIR)/ptools
FILESDIR:=$(CURDIR)/files
SRCS:=$(shell find $(SRCDIR) -name *.py)
OBJS:=$(addsuffix .pyc,$(basename $(subst src,builddir,$(SRCS))))

COMPILE="import sys\nimport py_compile\n\nif __name__ == \"__main__\":\n\tpy_compile.compile(sys.argv[1], sys.argv[2])"

.PHONY: ptools compile clean tnpcg

all: ptools

ptools: compile $(OBJS) tnpcg
	@#echo "$(SRCS)"
	@#echo "$(OBJS)"
	@cd $(BUILDDIR); zip  -r $(RELDIR)/ptools.zip *

compile:
	@- [ ! -e compile ] && echo  $(COMPILE) > compile

tnpcg:
	@mkdir -p $(RELDIR)
	@echo "#!/bin/sh" > $(RELDIR)/tnpcg
	@echo 'DIR=$$(dirname $$0)' > $(RELDIR)/tnpcg
	@echo -n 'python3 $${DIR}/ptools.zip tnpcg &' >> $(RELDIR)/tnpcg
	@chmod +x $(RELDIR)/tnpcg
	@echo -n 'CreateObject("WScript.Shell").Run """python.exe"" ptools.zip tnpcg", 0' > $(RELDIR)/tnpcg.vbs
	@cp -rf $(FILESDIR)/* $(RELDIR)/

$(BUILDDIR)/%.pyc: $(SRCDIR)/%.py
	@#echo $(@D)
	@mkdir -p $(@D)
	@echo $< .. $@
	@python3 $(CURDIR)/compile  $< $@
clean:
	@-rm -rf $(BUILDDIR) 2>/dev/null
	@-rm -rf $(RELDIR) 2>/dev/null
	@-rm -f compile 2>/dev/null
	@ find src -name *.pyc | xargs rm
	@ find src -name __pycache__ | xargs rm -r
