GCC := g++
RM := rm
APP := Node
INC_DIR := ./inc
CFLAGS := -g -Wall -std=c++11 -I$(INC_DIR)
LIBS := -lpthread
SRC_FILES := Node.cpp Member.cpp Thread.cpp File.cpp Logger.cpp Message.cpp Socket.cpp

.PHONY: clean

all: clean app

app:
	$(GCC) -o $(APP) $(SRC_FILES) $(CFLAGS) $(LIBS)

clean:
	$(RM) -f $(APP) *.o 
