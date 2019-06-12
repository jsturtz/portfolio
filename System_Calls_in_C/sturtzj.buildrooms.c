#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Room 
{
  char* name;
  int numConnections;
  struct Room* connections[6];
  char* type;
};

/* makes unique directory by concatenating process ID */
void makeDirectory(char* dir) {

  sprintf(dir, "sturtzj.rooms.%d", getpid());
  mkdir(dir, S_IRUSR | S_IWUSR | S_IXUSR);
}

/* will make the files that should populate the directory named by dir with values from rooms array*/
void makeFiles(struct Room rooms[7], char* dir) {

  int r, c;     /* r = index for rooms, c = index for connections */
  FILE* file;   
  char path[50];
  for (r = 0; r < 7; r++) {
    /* build up path string first, then use fopen */
    memset(path, '\0', 50);
    strcpy(path, dir);                  /* copy directory characters first */
    strcat(path, "/");                  /* concat the path symbol */
    strcat(path, rooms[r].name);        /* concat the name of room */
    strcat(path, "_room");              /* final extra bit */
    file = fopen(path, "w");

    /* print results to file */
    fprintf(file, "ROOM NAME: %s\n", rooms[r].name);
    for (c = 0; c < rooms[r].numConnections; c++) {
      fprintf(file, "CONNECTION %d: %s\n", c+1, rooms[r].connections[c]->name);
    }
    fprintf(file, "ROOM TYPE: %s\n", rooms[r].type);
    fclose(file);
  }
}

/* initializes structs, make sure to call deleteRooms below */
void initRooms(struct Room rooms[7]) {
  int i, j;

  for (i = 0; i < 7; i++) {
    char* name = calloc(9, sizeof(char));
    char* type = calloc(11, sizeof(char));
    rooms[i].name = name;
    rooms[i].type = type;
    rooms[i].numConnections = 0;
    for (j = 0; j < 6; j++) {
      rooms[i].connections[j] = NULL;
    }
  }
}

/* necessary to free up space allocated on heap */
void deleteRooms(struct Room rooms[7]) {
  int i;  
  for (i = 0; i < 7; i++) {
    free(rooms[i].name);
    free(rooms[i].type);
  }
}

/* assigns START_ROOM, END_ROOM, and MID_ROOM types to the seven rooms */
void assignTypes(struct Room rooms[7]) {
  
  strcpy(rooms[0].type, "START_ROOM");
  strcpy(rooms[6].type, "END_ROOM");
  
  int i;
  for (i = 1; i < 6; i++) {
    strcpy(rooms[i].type, "MID_ROOM");
  }
}

/* assigns unique random names to each room from the roomNames argument */
void assignNames(struct Room rooms[7], char* roomNames[10]) {
  
  int i = 0; 
  while (i < 7) {
    /* get random index */
    int index = rand() % 10;

    /* will set already used indices to NULL */
    if (roomNames[index] != NULL) {
      strcpy(rooms[i].name, roomNames[index]);
      roomNames[index] = NULL;
      i++;
    }
  }
}

/* Returns true if all rooms have 3 to 6 outbound connections, false otherwise */
int IsGraphFull(struct Room rooms[7])  
{
  int i;
  for (i = 0; i < 7; i++) 
  {
    if (rooms[i].numConnections < 3)
      return 0;
  }
  return 1;
}

/* Returns a random Room, does NOT validate if connection can be added */
struct Room* GetRandomRoom(struct Room rooms[7])
{
  int i = rand() % 7;
  return &rooms[i];
}

/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise */
int CanAddConnectionFrom(struct Room* x) 
{
  return x->numConnections < 6;
}

/* Returns true if a connection from Room x to Room y already exists, false otherwise */
int ConnectionAlreadyExists(struct Room* x, struct Room* y)
{
  int i;
  for (i = 0; i < x->numConnections; i++) {
    if (x->connections[i] == y) 
      return 1;
  }
  return 0;
}

/* Connects Rooms x and y together, does not check if this connection is valid */
void ConnectRoom(struct Room* x, struct Room* y) 
{
  x->connections[x->numConnections] = y;
  x->numConnections++;
}

/* Returns true if Rooms x and y are the same Room, false otherwise */
int IsSameRoom(struct Room* x, struct Room* y) 
{
  return x == y;
}

/* Adds a random, valid outbound connection from a Room to another Room */
void AddRandomConnection(struct Room rooms[7])  
{
  struct Room* A;   
  struct Room* B;

  while(1)
  {
    A = GetRandomRoom(rooms);

    if (CanAddConnectionFrom(A) == 1)
      break;
  }

  do
  {
    B = GetRandomRoom(rooms);
  }
  while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);

  ConnectRoom(A, B);  /*TODO: Add this connection to the real variables,*/
  ConnectRoom(B, A);  /*because this A and B will be destroyed when this function terminates*/
}

int main() {

  srand(time(0));

  /* hardcode ten room names */
  char* roomNames[10] = {
    "Socrates", "Camus", "Sartre", "Marx", "Proudhon", 
    "Bakunin", "Soren", "Hegel", "Cohen", "Chomsky"
  };

  /* set up rooms */
  struct Room rooms[7];
  initRooms(rooms);                     /* initializes with values */
  assignNames(rooms, roomNames);        /* assigned names randomly from ten roomNames */
  assignTypes(rooms);                   /* assigns first in rooms as START_ROOM, last as END_ROOM, rest as MID_ROOM */
  
  /* fully add connections to meet requirements */
  while (IsGraphFull(rooms) == 0)
  {
    AddRandomConnection(rooms);
  }

  /* make directory for files */
  char dir[20];
  memset(dir, '\0', 20);
  makeDirectory(dir);
  
  /* loop through rooms and create one file per room */
  makeFiles(rooms, dir);
  deleteRooms(rooms);
  return 0;
}
