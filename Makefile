# # Compiler and flags
# CXX = g++
# CXXFLAGS = -std=c++20 -pthread -Wall -Wextra

# # Target executable name
# TARGET = concurrent_containers
# AUTOGRADER_SCRIPT = autograde.sh

# # Source files
# SOURCES = main.cpp

# # Default target: compile the code
# all: $(TARGET)

# # Compile the executable
# $(TARGET): $(SOURCES)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# # Run the autograder script
# run-autograder: all
# 	@chmod +x $(AUTOGRADER_SCRIPT)
# 	./$(AUTOGRADER_SCRIPT)

# # Clean up the generated files
# clean:
# 	rm -f $(TARGET)
# 	rm -rf *.o


# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -pthread -Wall -Wextra

# Target executable name
TARGET = concurrent_containers
AUTOGRADER_SCRIPT = autograde.sh

# Source files
SOURCES = main.cpp

# Default target: compile the code
all: $(TARGET)

# Compile the executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Run the program with the -h flag
help: $(TARGET)
	@echo "Displaying usage instructions for $(TARGET):"
	./$(TARGET) -h

# Run the autograder script
run-autograder: all
	@chmod +x $(AUTOGRADER_SCRIPT)
	./$(AUTOGRADER_SCRIPT)

# Clean up the generated files
clean:
	rm -f $(TARGET)
	rm -rf *.o
