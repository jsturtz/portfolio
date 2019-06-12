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

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void decode(char *message, char *key, char *decrypted);

int main(int argc, char *argv[])
{
  int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsReadInChunk, charsLeft, charsSent;
  socklen_t sizeOfClientInfo;
  char buffer[BUFF_SIZE];
  char decrypted[BUFF_SIZE];
  struct sockaddr_in serverAddress, clientAddress;
  int pid;
  
  // these are the three components send by client delimited by newlines
  char auth[BUFF_SIZE];
  char key[BUFF_SIZE];
  char cipher[BUFF_SIZE];

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
  listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
  while(1) 
  {
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) error("ERROR on accept");

    pid=fork();
    if(pid==0)
    {
      memset(buffer, '\0', BUFF_SIZE);
      charsRead = 0;
      charsLeft = sizeof(buffer) - 1;
      do
      {
        charsReadInChunk = recv(establishedConnectionFD, buffer + charsRead, charsLeft, 0); // Read the client's message from the socket
        if (charsReadInChunk < 0) error("CLIENT: ERROR reading from socket");
        charsRead = charsRead + charsReadInChunk;
        charsLeft = charsLeft - charsReadInChunk;

      } while (charsLeft > 0);

      // parse the data (auth/key/cipher delimited by newline i.e. "auth\nkey\ncipher\0"
      memset(auth, '\0', BUFF_SIZE);
      memset(key, '\0', BUFF_SIZE);
      memset(cipher, '\0', BUFF_SIZE);

      // get initial authentication string to authenticate
      strcpy(auth, strtok(buffer, "\n"));
      if (strcmp(auth, "I am otp_dec") != 0)
      {
        // Send a failure message back to the client
        decrypted[0] = '1'; // this is my way of indicating that this client is not permitted
        charsRead = send(establishedConnectionFD, decrypted, BUFF_SIZE - 1, 0); 
        close(establishedConnectionFD);
        exit(2);
      }

      // get key and cipher
      strcpy(key, strtok(NULL, "\n"));
      strcpy(cipher, strtok(NULL, "\n"));

      // decode data
      memset(decrypted, '\0', BUFF_SIZE);
      decode(cipher, key, decrypted);

      // Send a Success message back to the client
      charsRead = send(establishedConnectionFD, decrypted, BUFF_SIZE - 1, 0); // Send success back
      if (charsRead < 0) error("ERROR writing to socket");
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
  /* close(establishedConnectionFD); // Close the existing socket which is connected to the client */
  close(listenSocketFD); // Close the listening socket
  return 0; 
}

void decode(char *cipher, char *key, char *decrypted) 
{
  int i, j;
  int cipherIdx;
  int keyIdx;
  int decrIdx;
  char accepted[27];

  // fill accepted with all acceptable chars 
  // (indices will represent unique values to be subtracted to get decrIdx) 
  accepted[0] = ' ';
  j = 1;
  for (i = 'A'; i <= 'Z'; i++)
  {
    accepted[j] = i;
    j++;
  }
  
  // loop over every char in cipher/key to stuff decrypted with new value
  for (i = 0; i < strlen(cipher); i++) 
  {
    // find idx of cipher[i] in accepted
    for (j = 0; j < 27; j++) 
    {
      if (cipher[i] == accepted[j])
      {
        cipherIdx = j;
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
    decrIdx = cipherIdx - keyIdx;
    if (decrIdx < 0) decrIdx = decrIdx + 27;
    decrypted[i] = accepted[decrIdx];       // then stuff decrypted with value at that new index
  }
}
