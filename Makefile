SRCD := ./src
BUILDD := ./build
TESTD := ./test

OFILE := tpso
ENTREGA := Cappella_TODO_TODO_TODO.zip #TODO

CXX := g++
CFLAGS := -Og -I$(SRCD)

CSRC := $(wildcard $(SRCD)/**/*.cc $(SRCD)/*.cc)
TEXSRC := ./informe/informe.tex

.PHONY: clean mkdir
default: main

entrega: clean mkdir
	zip $(BUILDD)/$(ENTREGA) informe.pdf Makefile README.md src test -r9

main: clean mkdir
	$(CXX) $(CFLAGS) $(CSRC) -o $(BUILDD)/$(OFILE)
	@printf "Run with:\n$(BUILDD)/$(OFILE)\n"

test: clean mkdir
	@printf "TODO: Implement tests\n"

mkdir:
	mkdir -p $(BUILDD)

clean:
	rm -rf $(BUILDD) $(ENTREGA)
