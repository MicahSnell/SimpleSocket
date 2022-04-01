#!/bin/sh

SRCS_CLIENT = \
	SocketClientTest.cpp \

SRCS_SERVER = \
	SocketServerTest.cpp \

INC_DIR = \
	-I../include \

LIB_PATH = \
	-L../lib \

USER_LIBS = \
	-lSocket \

OBJ_DIR = ./obj
OBJS_CLIENT = $(SRCS_CLIENT:%.cpp=$(OBJ_DIR)/%.o)
OBJS_SERVER = $(SRCS_SERVER:%.cpp=$(OBJ_DIR)/%.o)
LIBS = $(LIB_PATH) $(USER_LIBS)

RM = rm -f
CXX = g++
USER_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(USER_FLAGS) $(USER_DEFINES) $(INC_DIR)

TARGET_CLIENT = socket_client
TARGET_SERVER = socket_server

all: $(TARGET_CLIENT) $(TARGET_SERVER)
	@echo "Build successful"

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CXX) $(OBJS_CLIENT) $(LIBS) -o $@

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CXX) $(OBJS_SERVER) $(LIBS) -o $@

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
	@echo "Removing executables"
	$(RM) $(TARGET_CLIENT)
	$(RM) $(TARGET_SERVER)
