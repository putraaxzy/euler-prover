CXX = g++
CXXFLAGS = -std=c++17 -O3 -DNDEBUG -Wall -Wextra
SRCDIR = src
INCDIR = include
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/euler.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(OBJECTS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	if exist $(BUILDDIR) rmdir /s /q $(BUILDDIR)

run: $(TARGET)
	$(TARGET) proof number 100 10 4

test: $(TARGET)
	.\$(TARGET) proof complex 1000 6
	.\$(TARGET) proof topology 3
topology.o: topology.cpp topology.h
progress.o: progress.cpp progress.h
rng.o: rng.cpp rng.h config.h
