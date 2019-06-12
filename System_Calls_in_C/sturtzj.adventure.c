#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <pthread.h>


/* --------------------------------------------------------------------------- */
/* room data structure to hold info scraped from files */
/* --------------------------------------------------------------------------- */
struct Room 
{
  char* name;
  char* connections[7];
  int numConnections;
  char* type;
};

/* initializes array of seven Rooms */
void initRooms(struct Room rooms[7]) {
  int i, j;

  for (i = 0; i < 7; i++) {
    char* name = calloc(9, sizeof(char));
    char* type = calloc(11, sizeof(char));
    rooms[i].name = name;
    rooms[i].type = type;
    rooms[i].numConnections = 0;
    for (j = 0; j < 7; j++) {
      char* connect = calloc(9, sizeof(char));
      rooms[i].connections[j] = connect;
    }
  }
}

/* necessary to free up space allocated on heap */
void deleteRooms(struct Room rooms[7]) {
  int i, j;
  for (i = 0; i < 7; i++) {
    free(rooms[i].name);
    free(rooms[i].type);

    for (j = 0; j < 7; j++) {
      free(rooms[i].connections[j]);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* room data structure ends here*/
/* --------------------------------------------------------------------------- */

/* will copy into buffer the name of the latest sturtzj.rooms.XXXX directory */
void getDir(char* buffer) {

  char* baseName = "sturtzj.rooms.";    /* used to check that directory is relevant to search */
  size_t numBytes = 14 * sizeof(char);  /* used in memcmp below for baseName check */

  char dirPath[25];                     /* used to specify path for stat */
  time_t latestTime = 0;                /* keeps track of the latest directory timestamp in loop */
  
  DIR* cd;                              /* holds current directory "." */
  struct dirent* nextFile;              /* holds next file in current directory */
  struct stat dirInfo;                  /* the stat struct holds info about directory */

  /* get next dir */
  cd = opendir(".");
  while ((nextFile = readdir(cd)) != NULL)
  {
    /* checks that nextFile has format sturtzj.rooms.XXXXX */ 
    if (memcmp(baseName, nextFile->d_name, numBytes) == 0) 
    {
      /* set path for stat */
      memset(dirPath, '\0', 20);
      strcpy(dirPath, nextFile->d_name);
      stat(dirPath, &dirInfo);
      
      /* if nextFile was made later, update latestTime and copy into buffer */
      if (difftime(dirInfo.st_mtime, latestTime) > 0) 
      {
        latestTime = dirInfo.st_mtime;
        strcpy(buffer, nextFile->d_name);
      }
    }
  } 
  closedir(cd);
}

/* helper function that will replace first newline char with null terminator. use with caution */
void replaceNewline(char* str) {
  int i = 0;
  while(1) {
    if (str[i] == '\n') {
      str[i] = '\0';
      break;
    }
    i++;
  }
}

/* will scrape data from files into rooms struct and initialize currentRoom to start */
void getData(char* dir, struct Room rooms[7], struct Room* currentRoom) {

  DIR* cd;                              /* holds current directory "." */
  struct dirent* nextFile;              /* holds next file in current directory */
  FILE* fp;                             

  char path[100];       /* path string for opening file with fopen */
  char c;               /* this character will determine whether room is start/end/mid */
  int i;

  /* get next dir */
  cd = opendir(dir);
  i = 0;
  while ((nextFile = readdir(cd)) != NULL)
  {
    /* ignore directories */
    if ((strcmp(nextFile->d_name, "..") != 0) && (strcmp(nextFile->d_name, ".") != 0)) {

      /* build up path string, call fopen to access file*/
      memset(path, '\0', 100);
      strcpy(path, dir);
      strcat(path, "/");
      strcat(path, nextFile->d_name);
      fp = fopen(path, "r");
      
      /* get name first */
      char line[100];
      rewind(fp);
      fscanf(fp, "%[^\n]", line);
      strcpy(rooms[i].name, line+11);

      /* get connections */
      rewind(fp);
      while (fgets(line, sizeof(line), fp)) 
      {
        /* only interested in lines that begin with "CONNECTION" */
        if (memcmp(line, "CONNECTION", 10) == 0) 
        {
          replaceNewline(line);
          strcpy(rooms[i].connections[rooms[i].numConnections], line+14);
          rooms[i].numConnections++;
        }
      }
      
      /* get type of room */
      fseek(fp, -8, SEEK_END);
      c = fgetc(fp);
      /* if c == R, then must be START_ROOM */
      /* if c == N, then must be END_ROOM */
      if (c == 'R') 
      {
        strcpy(rooms[i].type, "START_ROOM");
        *currentRoom = rooms[i];
      } 

      else if (c == 'N')
      {
        strcpy(rooms[i].type, "END_ROOM");
      }
      else 
      {
        strcpy(rooms[i].type, "MID_ROOM");
      }
      fclose(fp);
      i++;
    }
  } 
  closedir(cd);
}

/* loops through connections stored in room to see if connection exists */
int isValidConnection(char* connection, struct Room* room) {
  int i; 
  for (i = 0; i < 7; i++) {
    if (strcmp(connection, room->connections[i]) == 0)
      return 1;         
  }
  return 0;
}

/* does not validate that connection exists. use isValidConnection before */
void updateCurrentRoom(char* connection, struct Room* currentRoom, struct Room rooms[7]) {
  int i;
  for (i = 0; i < 7; i++) {
    if (strcmp(rooms[i].name, connection) == 0) {
      *currentRoom = rooms[i];
    }
  }
}

void printPrompt(struct Room* currentRoom) {
  int i;
  printf("CURRENT LOCATION: %s\n", currentRoom->name);
  
  printf("POSSIBLE CONNECTIONS: ");
  for (i = 0; i < currentRoom->numConnections - 1; i++) {
    printf("%s, ", currentRoom->connections[i]);
  }
  printf("%s.\n", currentRoom->connections[currentRoom->numConnections-1]);
  printf("WHERE TO? >");
}

void printError() {
  printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
}

void printVictory(char** path, int count) {
  
  int i;
  printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
  printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", count);
  for (i = 0; i < count; i++) 
  {
    printf("%s\n", path[i]);
  }
}

void* printTime(void* timeLock) {

  /* lock right away so will only execute when main thread unlocks */
  pthread_mutex_lock(timeLock); 
  
  time_t t;                             /* used for holding time in time function */
  struct tm *tmp;                       /* struct needed for strftime function */
  char currentTime[50];
  memset(currentTime, '\0', 50);        

  time(&t);                             /* time since unix epoch */
  tmp = localtime(&t);                  /* fills tmp with current time info */
  strftime(currentTime, sizeof(currentTime),  "%I:%M%p, %A, %B %d, %Y", tmp);
  printf("\n%s\n", currentTime);

  /* output to file */ 
  FILE* fp;
  fp = fopen("currentTime.txt", "w");
  fprintf(fp, "%s\n", currentTime);
  fclose(fp);
  
  /* unlock thread before termination so main thread can proceed */
  pthread_mutex_unlock(timeLock); 
}

/* deletes both the memory allocations for the pointers of path array and the pointer to the array itself */
void deletePath(char** path, int count) {
  
  int i;
  for (i = 0; i < count; i++) 
  {
    free(path[i]);
  }
  free(path);
}


int main() {
  
  /* create mutex and have main thread lock straight away */
  pthread_mutex_t timeLock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&timeLock);
 
  /* create thread that will attempt to gain lock before anything else */
  pthread_t timeThread;
  pthread_create(&timeThread, NULL, printTime, &timeLock);
  
  /* get latest directory by timestamp */
  char dir[20];
  getDir(dir);                                  

  /* put data from files into rooms data structure */
  struct Room rooms[7];
  struct Room currentRoom;
  initRooms(rooms);
  getData(dir, rooms, &currentRoom);
  
  /* path holds steps taken to get to end */
  char** path = malloc(200 * sizeof(char*));
  char* nextStep;
  int count = 0;
  
  char nextCommand[10];
  while ((strcmp(currentRoom.type, "END_ROOM")) != 0) {
    
    printPrompt(&currentRoom);
    scanf("%s", nextCommand);
    
    if (strcmp(nextCommand, "time") == 0) {
      /* unlock so that function can execute */
      pthread_mutex_unlock(&timeLock);

      /* do not continue until thread terminates */
      pthread_join(timeThread, NULL);        

      /* lock again, create new thread to repeat process */
      pthread_mutex_lock(&timeLock);
      pthread_create(&timeThread, NULL, printTime, &timeLock);
    }
     
    else if (isValidConnection(nextCommand, &currentRoom)) {

      updateCurrentRoom(nextCommand, &currentRoom, rooms);      /* updates currentRoom variable */
      nextStep = malloc(10 * sizeof(char));                     /* allocate space on heap for holding characters from nextroom */
      strcpy(nextStep, currentRoom.name);                       /* copy next currentRoom into nextStep */
      path[count] = nextStep;                                   /* put that address into path array */
      count++;
    }

    else {
      printError();
    }
    printf("\n");
  }

  printVictory(path, count);
  deleteRooms(rooms);
  deletePath(path, count);
  return 0;
}
