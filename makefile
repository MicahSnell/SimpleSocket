SRCS = \
	./src/Socket.cpp \

INC_DIR = \
	-I./include \

OBJ_DIR = ./src/obj
OBJS = $(SRCS:./src/%.cpp=$(OBJ_DIR)/%.o)

AR = ar -crs
RM = rm -f
CXX = g++
CPP_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(CPP_FLAGS) $(USER_DEFINES) $(INC_DIR)

TARGET = ./lib/libSimpleSocket.a

all: $(TARGET)
	@echo "Build successful"

$(TARGET): $(OBJS)
	$(AR) $@ $(OBJS)

$(OBJ_DIR)/%.o: ./src/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

debug:
	@$(MAKE) USER_DEFINES=-DDEBUG

clean:
	@echo "Removing object files"
	$(RM) $(OBJ_DIR)/*.o

veryclean:
	@echo "Removing object files"
	$(RM) $(OBJ_DIR)/*.o
	@echo "Removing library file"
	$(RM) $(TARGET)
