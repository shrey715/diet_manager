CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Isrc
LIBS = -lstdc++fs

# Source files
SRCS = $(shell find src -name "*.cpp")
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = diet_manager

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS)
	mkdir -p data

# Compiling
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

# Run
run: $(TARGET)
	./$(TARGET)

# Install
install: $(TARGET)
	mkdir -p $(HOME)/bin
	cp $(TARGET) $(HOME)/bin/
	mkdir -p $(HOME)/.diet_manager/data
	cp -r data/* $(HOME)/.diet_manager/data/ 2>/dev/null || :

.PHONY: all clean run install
