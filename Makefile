SRCD := ./src
BUILDD := ./build
TESTD := ./test

OFILE := tpso
DBGFILE := $(OFILE).debug
ENTREGA := Cappella_TODO_TODO_TODO.zip #TODO

CXX := g++
CFLAGS := -std=c++17 -Og -I$(SRCD) #TODO: O2
DBGFLAGS := $(CFLAGS) -g -DDEBUG

CSRC := $(wildcard $(SRCD)/**/*.cc $(SRCD)/*.cc)
TEXSRC := ./informe/informe.tex

.PHONY: clean mkdir
default: main

entrega: clean mkdir
	zip $(BUILDD)/$(ENTREGA) informe.pdf Makefile README.md src test -r9

main: clean mkdir
	$(CXX) $(CFLAGS) $(CSRC) -o $(BUILDD)/$(OFILE)
	@printf "Run with:\n$(BUILDD)/$(OFILE)\n"

debug: clean mkdir
	$(CXX) $(DBGFLAGS) $(CSRC) -o $(BUILDD)/$(DBGFILE)
	@printf "Run with:\n$(BUILDD)/$(DBGFILE)\n"

test: clean mkdir
	@printf "TODO: Implement tests\n"

mkdir:
	mkdir -p $(BUILDD)

clean:
	rm -rf $(BUILDD) $(ENTREGA)
