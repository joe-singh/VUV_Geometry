# Starter makefile
# July 2017

# make sure your "rat_env.sh" has been sourced first!

# TODO put the name you "want" for executable on RHS
# for example, compiling "analysis.cpp" to executable "analysis"
EXE=newLoopOverFiles
CPPFLAGS=$(shell root-config --cflags) -I$(RATROOT)/include -I$(RATROOT)/include/libpq
LDFLAGS=$(shell root-config --glibs) -L$(RATROOT)/lib -lRATEvent

$(EXE): $(EXE).cc
	g++ -o $@ $(CPPFLAGS) $^ $(LDFLAGS)

.cpp.o:
	g++ -c $<

clean:
	rm *.log
	rm *.geo
.PHONY: clean
