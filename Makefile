CXX=g++
RM=rm -f
#CPPFLAGS=-std=c++11 -g -W -Wall -Winline -Wextra
CPPFLAGS=-std=c++11 -O3 -static -march=native -flto -fwhole-program

SRCS=graph.cpp init.cpp main.cpp opt.cpp tree.cpp dijkstra.cpp heuristic.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	$(CXX) $(CPPFLAGS) -o 3TST.exe $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) *~ .depend
	$(RM) 3TST.exe

.PHONY: clean

include .depend
