ifeq ($(origin CXX),default)
  CXX = g++
endif

CXXFLAGS ?=  -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align\
			-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security\
			-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long\
			-Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format\
			-pthread# -fsanitize=leak,undefined,address

#CXXFLAGS ?= -g -O0 -fsanitize=thread#leak,undefined,address
#LDFLAGS ?= -fsanitize=thread#leak,undefined,address
#CXXFLAGS ?= -O3 -mfpmath=sse

CSRC = main.cpp func.cpp
COBJ = main.o func.o

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

.PHONY: all
all: a.out

a.out: $(COBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o
