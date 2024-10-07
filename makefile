SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
INC_DIR = -I$(SRC_DIR)

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

AR = ar -crs
RM = rm -f
CXX = g++
CPP_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(CPP_FLAGS) $(DEFINES) $(INC_DIR)

TARGET = $(LIB_DIR)/libSimpleSocket.a

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR) $(LIB_DIR)

$(TARGET): $(OBJS)
	$(AR) $@ $^

$(OBJS): $(SRCS)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

debug:
	@$(MAKE) DEFINES=-DDEBUG

clean:
	$(RM) $(OBJS)

distclean:
	@$(MAKE) clean
	$(RM) $(TARGET)
	$(RM) -r $(LIB_DIR) $(OBJ_DIR)
