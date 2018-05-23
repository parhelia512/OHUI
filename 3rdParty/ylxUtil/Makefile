
include ../../Makefile.cfg

GTK_INCLUDE = `pkg-config gtk+-3.0 --cflags`
GTK_LIB = `pkg-config gtk+-3.0 --libs`
CC  = color_compile  gcc 
CXX = color_compile  g++ -g

TARGET = ylxUtil

CXXFLAGS= -Wall -fshort-wchar  -fdiagnostics-color=auto \
			$(GTK_INCLUDE)  -DNDEBUG  -std=gnu++0x -fno-exceptions -I. 

files=Util.o
OBJFILES = $(addprefix $(OBJ_PATH), $(files))


all:$(OBJFILES)
	ar rvls ylxUtil.a  $(addprefix $(OBJ_PATH), Util.o)


$(OBJ_PATH)%.o:%.cpp
	@echo "*** Compile cpp $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)%.o:%.cxx
	@echo "*** Compile cxx $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)%.o:%.c
	@echo "*** Compile c $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf ./obj/*
	rm $(TARGET).a


