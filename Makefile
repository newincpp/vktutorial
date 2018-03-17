SRC	= main.cpp
NAME	=LunaticPlatypus2
CXX	= clang++
LINKER	= $(CXX)
EXT=cpp
OPTIM= $(GLM_DEFINE) -march=native -O0 -fno-rtti #-fno-exceptions
INCLUDE= -I./ # -I./imgui
CXXFLAGS= -Wall -Wextra -W -std=c++11 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE) -DEASTL_EABASE_DISABLED -std=c++17

LIBS+= $(shell pkg-config glfw3 --libs) 
LIBS+= $(shell pkg-config vulkan --libs)
LIBS+= libEASTL.a

LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS) -flto -use-gold-plugin
OBJS	= $(SRC:.$(EXT)=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER)  $(OBJS) -o $(NAME) $(LDFLAGS)

re: make -B -j4

.PHONY: all clean fclean re

full: extractLibs $(NAME) install project

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
