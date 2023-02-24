SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
INC_DIR = -I$(SRC_DIR)

SRCS = $(SRC_DIR)/Socket.cpp 
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

AR = ar -crs
RM = rm -f
CXX = g++
CPP_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(CPP_FLAGS) $(DEFINES) $(INC_DIR)

TARGET = libSimpleSocket.a

all: directories $(TARGET)
	@echo "Build successful"

directories:
	@mkdir -p $(OBJ_DIR) $(LIB_DIR)

$(TARGET): $(OBJS)
	$(AR) $(LIB_DIR)/$@ $(OBJS)

$(OBJS): $(SRCS)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

debug:
	@$(MAKE) DEFINES=-DDEBUG

clean:
	@echo "Removing object files"
	@$(RM) $(OBJ_DIR)/*.o

veryclean:
	@$(MAKE) clean
	@echo "Removing library file"
	@$(RM) $(TARGET)

distclean:
	@$(MAKE) veryclean
	@echo "Removing generated directories"
	@$(RM) -r $(LIB_DIR) $(OBJ_DIR)
