
PLATFORM := $(shell uname)

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -lGL
EXT = 
DEFS = `pkg-config --cflags glfw3`

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32
endif

LINK += stb_image.o 
LINK += tiny_obj_loader.o 

.PHONY:  clean

all : model_viewer$(EXT)

model_viewer$(EXT) : main.o $(LINK)
	g++ $(DEFS) -o model_viewer main.o $(LINK) $(GL_LIBS)

main.o : main.cpp $(LINK)

stb_image.o : stb_image.cpp stb_image.h
	g++ $(DEFS) -c stb_image.cpp

tiny_obj_loader.o : tiny_obj_loader.cc tiny_obj_loader.h
	g++ $(DEFS) -c tiny_obj_loader.cc

clean:
	rm -f *.o model_viewer$(EXT)
