SRCD := ./src
BUILDD := ./build
TESTD := ./test

OFILE := tpso
DBGFILE := $(OFILE).debug
TESTFILE := $(OFILE).test
ENTREGA := Cappella_TODO_TODO_TODO.zip #TODO

CXX := g++
CFLAGS := -std=c++17 -Og -I$(SRCD) #TODO: O2
DBGFLAGS := $(CFLAGS) -g -DDEBUG
TESTFLAGS := $(CFLAGS) -g -DTESTING

CSRC := $(wildcard $(SRCD)/**/*.cc $(SRCD)/*.cc)
TEXSRC := ./informe/informe.tex

.PHONY: clean mkdir
default: main

entrega: mkdir
	zip $(BUILDD)/$(ENTREGA) informe.pdf Makefile README.md src test -r9

main: mkdir
	$(CXX) $(CFLAGS) $(CSRC) -o $(BUILDD)/$(OFILE)
	@printf "Run with:\n$(BUILDD)/$(OFILE)\n"

debug: mkdir
	$(CXX) $(DBGFLAGS) $(CSRC) -o $(BUILDD)/$(DBGFILE)
	@printf "Run with:\n$(BUILDD)/$(DBGFILE)\n"

test: mkdir
	$(CXX) $(TESTFLAGS) \
				$(filter-out $(SRCD)/main.cc, $(CSRC)) \
				test/{*,gtest-1.8.1/*}.cc \
				-o $(BUILDD)/$(TESTFILE)
	@printf "Run with:\n$(BUILDD)/$(TESTFILE)\n"

mkdir:
	mkdir -p $(BUILDD)

clean:
	rm -rf $(BUILDD) $(ENTREGA)
