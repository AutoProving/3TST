CXX=g++
RM=rm -f
#CPPFLAGS=-std=c++11 -g -W -Wall -Winline -Wextra
CPPFLAGS=-std=c++11 -O3 -march=native -flto

SRCS=graph.cpp init.cpp main.cpp opt.cpp tree.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	$(CXX) $(CPPFLAGS) -o steinertree $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) *~ .depend
	$(RM) steinertree

.PHONY: clean

include .depend
