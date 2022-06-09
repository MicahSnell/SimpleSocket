SRCS = \
	./src/Socket.cpp \

INC_DIR = \
	-I./src \

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
OBJS = $(SRCS:./src/%.cpp=$(OBJ_DIR)/%.o)

AR = ar -crs
RM = rm -f
CXX = g++
CPP_FLAGS = -O3 -Wall -Wextra -Wno-unused -pedantic -ansi -std=c++11
CXX_FLAGS = $(CPP_FLAGS) $(USER_DEFINES) $(INC_DIR)

TARGET = $(BUILD_DIR)/libSimpleSocket.a

all: $(OBJ_DIR) $(TARGET)
	@echo "Build successful"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(AR) $@ $(OBJS)

$(OBJS): $(SRCS)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

debug:
	@$(MAKE) USER_DEFINES=-DDEBUG

clean:
	@echo "Removing object files"
	$(RM) $(OBJ_DIR)/*.o

veryclean:
	@$(MAKE) clean
	@echo "Removing library file"
	$(RM) $(TARGET)

superclean:
	@$(MAKE) veryclean
	@echo "Removing build directory"
	$(RM) -r $(BUILD_DIR)
