OBJ = server.c
LIBS = 

all: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o server
	
