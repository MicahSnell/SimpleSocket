OBJ_DIR = obj
INC_DIR = -I../src

SRCS_CLIENT = SocketClientTest.cpp
SRCS_SERVER = SocketServerTest.cpp

OBJS_CLIENT = $(SRCS_CLIENT:%.cpp=$(OBJ_DIR)/%.o)
OBJS_SERVER = $(SRCS_SERVER:%.cpp=$(OBJ_DIR)/%.o)

LIB_DIRS = -L../lib
LIBS = -lSimpleSocket

RM = rm -f
CXX = g++
CPP_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(CPP_FLAGS) $(DEFINES) $(INC_DIR)

TARGET_CLIENT = socket_client
TARGET_SERVER = socket_server

all: $(TARGET_CLIENT) $(TARGET_SERVER)
	@echo "Build successful"

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
	@$(RM) $(OBJ_DIR)/*.o

veryclean:
	@$(MAKE) clean
	@echo "Removing object directory"
	@$(RM) -r $(OBJ_DIR)
	@echo "Removing executables"
	@$(RM) $(TARGET_CLIENT) $(TARGET_SERVER)

