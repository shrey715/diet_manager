CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -lstdc++fs

# Source files
SOURCES = src/main.cpp \
          src/cli.cpp \
          src/models/basic_food.cpp \
          src/models/composite_food.cpp \
          src/models/user.cpp \
          src/models/log_entry.cpp \
          src/managers/food_database.cpp \
          src/managers/log_manager.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = diet_manager

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

# Rule for building object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
