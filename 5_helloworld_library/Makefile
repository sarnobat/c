LIBS  = -ljson
CFLAGS = -Wall

objects = test1
all: $(objects)

$(objects): %: %.c
	$(CXX) $(CFLAGS) -o $@ $<  $(LIBS) 

clean: 
	rm -rf $(objects)
	
