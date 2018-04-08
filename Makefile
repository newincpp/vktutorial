SRC	= main.cpp
NAME	=LunaticPlatypus2
CXX	= clang++
LINKER	= $(CXX)
EXT=cpp
OPTIM= $(GLM_DEFINE) -O0  #-fno-exceptions
MOLTENVKINSTALLPATH=/Users/newin/Documents/MoltenVK
INCLUDE= -I./ $(shell pkg-config --cflags glfw3) # -I./imgui
INCLUDE+= -I$(MOLTENVKINSTALLPATH)/Package/Release/MoltenVK/include/
CXXFLAGS= -DVK_USE_PLATFORM_MACOS_MVK -Wall -Wextra -W -std=c++11 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE) -DEASTL_EABASE_DISABLED -std=c++14 -g

LIBS+= $(shell pkg-config glfw3 --libs) 
#LIBS+= $(shell pkg-config vulkan --libs) #linux only
#LIBS+= libEASTL_linux.a
LIBS+= libEASTL_osx.a
LIBS+= -framework Cocoa -framework IOKit -framework IOSurface -framework Metal -framework QuartzCore -F$(MOLTENVKINSTALLPATH)/Package/Release/MoltenVK/macOS/ -framework MoltenVK -lc++ #osx only

LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS) 
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
