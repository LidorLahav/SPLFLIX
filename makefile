# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
CC = g++
CFLAGS  = -g -Wall -Weffc++ -std=c++11

# All Targets
all: splflix

# Tool invocations
# Executable "splflix" depends on the files Session.o, Action.o, User.o, Watchable.o and main.o.
splflix: bin/main.o bin/Session.o bin/Action.o bin/User.o bin/Watchable.o
	@echo 'Building target: splflix'
	@echo 'Invoking: C++ Linker'
	$(CC) -o bin/splflix bin/Session.o bin/Action.o bin/User.o bin/Watchable.o bin/main.o
	@echo 'Finished building target: splflix'
	@echo ' '

# Depends on the source and header files
bin/Session.o: src/Session.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/Session.o src/Session.cpp

# Depends on the source and header files
bin/Action.o: src/Action.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/Action.o src/Action.cpp

# Depends on the source and header files
bin/User.o: src/User.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/User.o src/User.cpp

# Depends on the source and header files
bin/Watchable.o: src/Watchable.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/Watchable.o src/Watchable.cpp

# Depends on the source and header files 
bin/main.o: src/main.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/main.o src/main.cpp

#Clean the build directory
clean: 
	rm -f bin/*
