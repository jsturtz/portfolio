#include<time.h>
#include<stdlib.h>
#include<stdio.h>


int main(int argc, char* argv[]) 
{
  // check correct args
  if (argc != 2) {
    fprintf(stderr, "USAGE: Expected one argument\n");
    exit(1);
  }
  
  srand(time(0));

  // buffer will be what is printed to stdout
  int size = atoi(argv[1]);
  char buffer[size+1];

  // create allowedChars array
  char allowedChars[27];
  allowedChars[0] = 32; // manually add spacebar char
  int i = 1;
  int c;
  for (c = 65; c < 91; c++) // add A - Z
  {
    allowedChars[i] = c;
    i++;
  }
  
  // use allowedChars array with rand() to determine char to stuff
  // in buffer
  for (i = 0; i < size; i++) 
  {
    buffer[i] = allowedChars[rand() % 27];
  }
  buffer[size] = '\0';
  
  // finally, print to stdout
  printf("%s\n", buffer);
}
