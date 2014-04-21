OBJ = MPlayYouTubeServer.c
LIBS = 
PROGRAM = MPlayYouTubeServer

all: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(PROGRAM)

clean:
	rm -f $(PROGRAM)
	
