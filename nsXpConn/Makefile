
include ../Makefile.cfg

TARGET  = nsxpconn
XPTFILE = IXpConn
GTK_INCLUDE = `pkg-config gtk+-3.0 --cflags`
GTK_LIB = `pkg-config gtk+-3.0 --libs`
CC  = color_compile  gcc -fPIC
CXX = color_compile  g++ -fPIC


LDFLAGS = `pkg-config --cflags --libs gtk+-3.0`  -lgdk_pixbuf-2.0 \
-lpthread  -lm -ldl \
 -Wl,-rpath -Wl,. \
 -Wl,-rpath -Wl,$(MOZ_ROOT)bin \
 -Wl,-rpath -Wl,$(MOZ_ROOT)sdk/lib \
 -Wl,-rpath -Wl,$(MOZ_ROOT)lib \
 -Wl,-rpath -Wl,/home/lixing/lib \
 -Wl,-rpath -Wl,/home/lixing/lib64 \
 ../3rdParty/ylxUtil/ylxUtil.a \
 -L$(MOZ_ROOT)lib -lnspr4 -lplds4 -lplc4\
 -L$(MOZ_ROOT)sdk/lib -lxul -lmozglue \
  $(MOZ_ROOT)sdk/lib/libmemory.a 


LIBS = -Wl,--whole-archive -lmozglue -lmozalloc -lxpcomglue  -Wl,--no-whole-archive  \
  -lnspr4 -lplds4 -lplc4  -lxpcomglue_s -lxul


CXXFLAGS=   -Wall -fshort-wchar  -fdiagnostics-color=auto -DMOZ_GLUE_IN_PROGRAM -DNO_NSPR_10_SUPPORT \
  -fPIC  -Wall -Wpointer-arith -Woverloaded-virtual -Werror=return-type -Wtype-limits -Wempty-body -Wsign-compare -Wno-invalid-offsetof -Wcast-align -fno-exceptions -fno-strict-aliasing -fno-rtti -ffunction-sections -fdata-sections -fno-exceptions -std=gnu++0x -pthread -pipe  -DNDEBUG -DTRIMMED -g -Os -freorder-blocks  -fomit-frame-pointer   -DMOZILLA_CLIENT \
	$(CFS)	-DNPRT_EXPORTS -DMOZILLA_CLIENT -DOS_POSIX=1 -DOS_LINUX=1 -DIMPL_XREAPI -DHW_THREADS \
			$(GTK_INCLUDE)  -DNDEBUG -std=gnu++0x  -fno-exceptions -I. \
						-I$(MOZ_ROOT)include \
						-I$(MOZ_ROOT)include/nspr

BIN_PATH = .
SRC_PATH := ./ 


files= nsHandle.o nsHandleModule.o
OBJFILES = $(addprefix $(OBJ_PATH), $(files))

xpidl = $(MOZ_ROOT)sdk/bin/
PYTHON=/home/lixing/bin/python


all:$(TARGET).so
	cp -f ./$(TARGET).so       $(MOZ_ROOT)bin/components
	cp -f ./$(TARGET).manifest $(MOZ_ROOT)bin/components
	cp -f ./$(XPTFILE).xpt     $(MOZ_ROOT)bin/components/$(TARGET).xpt
	cp -f ./$(XPTFILE).idl     $(MOZ_ROOT)idl 

package:
	echo "publish... $(TARGET)"

$(TARGET).so:IXpConn.xpt $(OBJFILES)
	@echo "*** Linking $@..."
	ld  -o $@ -G  $(OBJFILES)  $(MOZ_ROOT)sdk/lib/libxpcomglue_s.a


#GENERIC_XPT:IXpConn.xpt

# HOW TO LINK
%.xpt:%.idl
	@echo "*** Compile $<... "
	"$(PYTHON)" "$(xpidl)header01.py"  --cachedir=./obj  -o $*.h   -I$(MOZ_ROOT)idl ./$<
	"$(PYTHON)" "$(xpidl)typelib.py" --cachedir=./obj  -o $*.xpt -I$(MOZ_ROOT)idl ./$<

%.o:
	@echo "*** Linking $@..."
	$(CXX) $(CXXFLAGS) $<   $(LDFLAGS) $(LIBS) -o $@

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
	rm -f ./$(XPTFILE).h ./$(XPTFILE).xpt
	rm -f *.o *~ $(TARGET)
