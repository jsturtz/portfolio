#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef BUFF_SIZE
#define BUFF_SIZE 1000000
#endif

// helper function delcarations
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
void encode(char *message, char *key, char *encrypted);

int main(int argc, char *argv[])
{
  int listenSocketFD, establishedConnectionFD, portNumber, charsReadInChunk, charsRead, charsLeft, charsSent;
  socklen_t sizeOfClientInfo;
  char buffer[BUFF_SIZE];
  char encrypted[BUFF_SIZE];
  struct sockaddr_in serverAddress, clientAddress;
  int pid;
  
  // these are the three components send by client delimited by newlines
  char auth[BUFF_SIZE];
  char key[BUFF_SIZE];
  char plaintext[BUFF_SIZE];

  // check that args are right
  if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

  // Set up the address struct for this process (the server)
  memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

  // Set up the socket
  listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (listenSocketFD < 0) error("ERROR opening socket");

  // Enable the socket to begin listening
  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
    error("ERROR on binding");

  // Flip the socket on - it can now receive up to 5 connections
  listen(listenSocketFD, 5); 
  while(1) 
  {
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) error("ERROR on accept");
    
    // when connection accepted, fork off child and let parent continue listening
    pid=fork();
    if(pid==0)
    {
      // receive from client 
      memset(buffer, '\0', BUFF_SIZE);
      charsRead = 0;
      charsLeft = sizeof(buffer) - 1;
      do
      {
        charsReadInChunk = recv(establishedConnectionFD, buffer + charsRead, charsLeft, 0); // Read the client's message from the socket
        if (charsReadInChunk < 0) error("CLIENT: ERROR reading from socket");
        charsRead = charsRead + charsReadInChunk;
        charsLeft = charsLeft - charsReadInChunk;

      } while (charsLeft > 0);  // needed because not all data is received in one chunk

      // parse the data (auth/key/plaintext delimited by newline i.e. "auth\nkey\nplaintext\0"
      memset(auth, '\0', BUFF_SIZE);
      memset(key, '\0', BUFF_SIZE);
      memset(plaintext, '\0', BUFF_SIZE);

      // get initial authentication string to authenticate
      strcpy(auth, strtok(buffer, "\n"));
      if (strcmp(auth, "I am opt_enc") != 0)
      {
        // Send a failure message back to the client
        encrypted[0] = '1'; // this is my way of indicating that this client is not permitted
        charsSent = send(establishedConnectionFD, encrypted, BUFF_SIZE - 1, 0); 
        if (charsSent < 0) error("CLIENT: ERROR writing to socket");
        close(establishedConnectionFD);
        exit(2);
      }

      // get key and plaintext
      strcpy(key, strtok(NULL, "\n"));
      strcpy(plaintext, strtok(NULL, "\n"));

      // encode data
      memset(encrypted, '\0', BUFF_SIZE);
      encode(plaintext, key, encrypted);

      // Send a Success message back to the client
      charsSent = send(establishedConnectionFD, encrypted, BUFF_SIZE - 1, 0); // Write to the server
      if (charsSent < 0) error("CLIENT: ERROR writing to socket");
      close(establishedConnectionFD);
      exit(1);
    }
    else if (pid == -1) {
      error("Child process could not be created, abort\n");
    }
    else {
      close(establishedConnectionFD);
    }
  }
  close(listenSocketFD); // Close the listening socket
  return 0; 
}

void encode(char *message, char *key, char *encrypted) 
{

  int i, j, c;
  int messageIdx;
  int keyIdx;
  int encrIdx;
  char accepted[27];

  // fill accepted with all acceptable chars 
  // (indices will represent unique values to be added together)
  accepted[0] = ' ';
  i = 1;
  for (c = 'A'; c <= 'Z'; c++)
  {
    accepted[i] = c;
    i++;
  }
  
  // loop over every char in message/key to stuff encrypted with new value
  for (i = 0; i < strlen(message); i++) 
  {
    // find idx of message[i] in accepted
    for (c = 0; c < 27; c++) 
    {
      if (message[i] == accepted[c])
      {
        messageIdx = c;
        break;
      }
    }

    // find idx of key[i] in accepted
    for (j = 0; j < 27; j++) 
    {
      if (key[i] == accepted[j])
      {
        keyIdx = j;
        break;
      }
    }

    // indices represent the "values" to be added together with modulus
    encrIdx = (messageIdx + keyIdx) % 27;
    encrypted[i] = accepted[encrIdx];       // then stuff encrypted with value at that new index
  }
}

