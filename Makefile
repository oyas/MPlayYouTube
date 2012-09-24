OBJ = server.c
LIBS = 

all: $(OBJ)
	$(CC) -W -Wall $(OBJ) $(LIBS) -o server
	
