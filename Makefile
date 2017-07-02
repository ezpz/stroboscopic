
ALL = strobe

default: $(ALL)

BASE_LIBS = -lm
ALLEGRO_LIBS = -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf
LIBS = $(BASE_LIBS) $(ALLEGRO_LIBS)

INC = -I./include

SRCS = src/strobe.cc \
	   src/driver.cc

strobe: 
	g++ $(INC) $(SRCS) $(LIBS) -o strobe

clean:
	rm -f $(ALL)
