SRCD := ./src
BUILDD := ./build
TESTD := ./test

OFILE := tpso
DBGFILE := $(OFILE).debug
TESTFILE := $(OFILE).test
ANFILE := $(OFILE).an
ENTREGA := Cappella_TODO_TODO_TODO.zip #TODO

CXX := g++
CFLAGS := -std=c++17 -O3 -I$(SRCD) #-Wall
DBGFLAGS := $(CFLAGS) -g -DDEBUG
TESTFLAGS := $(DBGFLAGS) -DTESTING
ANFLAGS := $(CFLAGS) -DAN -lrt

CSRC := $(wildcard $(SRCD)/**/*.cc $(SRCD)/*.cc)

.PHONY: clean mkdir
default: main

entrega: clean mkdir
	zip $(BUILDD)/$(ENTREGA) informe.pdf Makefile README.md src test analysis extern -r9

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

an: mkdir
	$(CXX) $(ANFLAGS) \
				$(filter-out $(SRCD)/main.cc, $(CSRC)) \
				analysis/main.cc \
				-o $(BUILDD)/$(ANFILE)
	python analysis/main.py

mkdir:
	mkdir -p $(BUILDD)

clean:
	rm -rf $(BUILDD) $(ENTREGA)
