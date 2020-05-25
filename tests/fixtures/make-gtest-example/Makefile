# Makefile for gtest example

CPPFLAGS = -I /usr/local/include
CXXFLAGS = -c -Wall
LDFLAGS = -L /usr/local/lib -l gtest_main -l gtest -l pthread

TARGET = alltests
OBJECTS = test_stringcompare.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LD_FLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
