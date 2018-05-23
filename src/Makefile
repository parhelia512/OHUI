
include ../Makefile.cfg

TARGET = tsjm
GTK_INCLUDE = `pkg-config gtk+-3.0 --cflags`
GTK_LIB = `pkg-config  gtk+-3.0 --libs`
CC  = color_compile  gcc 
CXX = color_compile  g++ -g

 
 
LDFLAGS = `pkg-config --libs gtk+-3.0 gdk-x11-3.0` -lgdk_pixbuf-2.0 \
-lpthread  -lm -ldl -lX11 \
  -Wl,-rpath -Wl,. \
  -Wl,-rpath -Wl,./lib64 \
  -Wl,-rpath -Wl,$(MOZ_ROOT)bin \
  -Wl,-rpath -Wl,$(MOZ_ROOT)lib \
 ../3rdParty/ylxUtil/ylxUtil.a \
 -L$(MOZ_ROOT)lib -lnspr4 -lplds4 -lplc4 \
 -L$(MOZ_ROOT)bin -lxul \
 -L$(MOZ_ROOT)sdk/lib -lxpcomglue -lxpcomglue_s
 
 
LIBS = -Wl,--whole-archive -L$(MOZ_ROOT)sdk/lib  -lmozglue -Wl,--no-whole-archive
  


CXXFLAGS= -Wall -fshort-wchar  -fdiagnostics-color=auto \
		$(CFS) -DXPCOM_GLUE -DMOZILLA_CLIENT -DOS_POSIX=1 -DOS_LINUX=1 -DIMPL_XREAPI -DHW_THREADS \
			$(GTK_INCLUDE)  -DNDEBUG -std=gnu++0x  -fno-exceptions -I. \
						-I$(MOZ_ROOT)include \
						-I$(MOZ_ROOT)include/nspr \
						-Imozilla-config.h


BIN_PATH = .
SRC_PATH := ./ 


files=stdafx.o MozUtil.o nsWeakReference.o gtkmozembedmarshal.o BrowserImpl.o BrowserWrapper.o \
    nsProfileDirServiceProvider.o PromptService.o \
	 MozEmbedSignal.o  MainFrame.o OhuiApp.o main.o
OBJFILES = $(addprefix $(OBJ_PATH), $(files))


$(TARGET): $(OBJFILES)  $(SUBDIRS)
	@echo "*** Linking $@..."
	$(CXX)  $(OBJFILES) $(LDFLAGS) $(LIBS) -o $(MOZ_ROOT)bin/$@
	@echo "success!"
#	mv -f ./$@ $(MOZ_ROOT)bin
	ln -sf $(MOZ_ROOT)bin/$@

# HOW TO LINK
.o$(EXEEXT):
	@echo "*** Linking $@..."
	$(CXX) $<   $(LDFLAGS) $(LIBS) -o $@

%.cpp:
	@echo "*** Compile cpp $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)%.o:%.cpp
	@echo "*** Compile cpp $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)%.o:%.cxx
	@echo "*** Compile cxx $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)%.o:%.c
	@echo "*** Compile c $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

SUBDIRS = .

#.PHONY: $(SUBDIRS)
$(OBJFILES): $(SUBDIRS)


cscope:
	-rm cscope.files
	-rm cscope.in.out
	-rm cscope.out
	-rm cscope.po.out
	find . \
		-path /home/lixing/include -o \
		-name "*.c" -o -name "*.cxx" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.H"  > cscope.files
	cscope -q  -R -b -I /home/lixing/include -I. -I /usr/include -i cscope.files
	export CSCOPE_DB=./cscope.out:/home/lixing/include/cscope.out

clean:
	rm -rf ./obj/*
	rm -f *.o *~ $(TARGET)
