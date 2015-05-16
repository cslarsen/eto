CXXFLAGS := -W -Wall -std=c++11
TARGETS := eto

all: $(TARGETS)

run: all
	./eto

clean:
	rm -f $(TARGETS)
