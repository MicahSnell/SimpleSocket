OBJ_DIR = ../obj
INC_DIR = -I../src

SRCS_CLIENT = SocketClientTest.cpp
OBJS_CLIENT := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS_CLIENT))

SRCS_SERVER = SocketServerTest.cpp
OBJS_SERVER := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS_SERVER))

LIB_DIRS = -L../lib
LIBS = -lSimpleSocket

RM = rm -f
CXX = g++
CPP_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(CPP_FLAGS) $(DEFINES) $(INC_DIR)

TARGET_CLIENT = socket_client
TARGET_SERVER = socket_server

all: directories $(TARGET_CLIENT) $(TARGET_SERVER)
	@echo "Build successful"

directories:
	@mkdir -p $(OBJ_DIR)

$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CXX) $(LIB_DIRS) -o $@ $^ $(LIBS)

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CXX) $(LIB_DIRS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

debug:
	@$(MAKE) DEFINES=-DDEBUG

clean:
	@echo "Removing object files"
	$(RM) $(OBJS_CLIENT) $(OBJS_SERVER)

distclean:
	@$(MAKE) clean
	@echo "Removing executables"
	$(RM) $(TARGET_CLIENT) $(TARGET_SERVER)
	@echo "Removing directories"
	$(RM) -r $(OBJ_DIR)
