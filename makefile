#!/bin/sh

SRCS = \
	Socket.cpp \

INC_DIR = \
	-I../include

OBJ_DIR = ./obj
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

AR = ar -crs
RM = rm -f
CXX = g++
USER_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(USER_FLAGS) $(USER_DEFINES) $(INC_DIR)

TARGET = ../lib/libSocket.a

all: $(TARGET)
	@echo "Build successful"

$(TARGET): $(OBJS)
	$(AR) $@ $(OBJS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

debug:
	@$(MAKE) USER_DEFINES=-DDEBUG

clean:
	@echo "Removing object files"
	$(RM) $(OBJ_DIR)/*.o

veryclean:
	@echo "Removing object files"
	$(RM) $(OBJ_DIR)/*.o
	@echo "Removing library files"
	$(RM) $(TARGET)
