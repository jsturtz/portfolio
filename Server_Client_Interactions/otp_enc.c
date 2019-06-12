#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#ifndef BUFF_SIZE
#define BUFF_SIZE 1000000
#endif

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

// function delcarations
int getFileText(char *filename, char filetext[BUFF_SIZE]);
int validChars(char *message);

int main(int argc, char *argv[])
{
  
  int socketFD, portNumber, charsSent, charsLeft, charsRead, charsReadInChunk;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;

  char buffer[BUFF_SIZE];
  char *messageFileName;
  char *keyFileName;
  char messageText[BUFF_SIZE];
  char keyText[BUFF_SIZE];
    
  // read from args
  if (argc != 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args
  messageFileName = argv[1];
  keyFileName = argv[2];
  portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string

  // Set up the server address struct
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
  if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

  // Set up the socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (socketFD < 0) error("CLIENT: ERROR opening socket");
  
  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
          error("CLIENT: ERROR connecting");
    
  // read from filenames into filetexts
  if (getFileText(messageFileName, messageText) == -1)
  {
    error("Message filename could not be read");
  }

  if (getFileText(keyFileName, keyText) == -1) 
  {
    error("Key filename could not be read");
  }

  // validate keyfile longer than or equal to messagefile
  if (strlen(keyText) < strlen(messageText)) 
  {
    fprintf(stderr, "Error: key '%s' too short\n", keyFileName);
    exit(1);
  }

  // validate both files have acceptable characters
  if (!validChars(messageText) || !validChars(keyText)) 
  {
      fprintf(stderr, "ERROR: Invalid characters in one of the files\n");
      exit(1);
  }

  // build message to send to server
  memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
  strcpy(buffer, "I am opt_enc\n");
  strcat(buffer, keyText);
  strcat(buffer, "\n");
  strcat(buffer, messageText);

  // Send message to server
  charsSent = send(socketFD, buffer, BUFF_SIZE - 1, 0); // Write to the server
  if (charsSent < 0) error("CLIENT: ERROR writing to socket");

  // Get return message from server
  memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
  charsRead = 0;
  charsLeft = sizeof(buffer) - 1;
  do
  {
    charsReadInChunk = recv(socketFD, buffer + charsRead, charsLeft, 0); // Read data from the socket, leaving \0 at end
    if (charsReadInChunk < 0) error("CLIENT: ERROR reading from socket");
    charsRead = charsRead + charsReadInChunk;
    charsLeft = charsLeft - charsReadInChunk;
  } while (charsLeft > 0); // needed because not all data will be received if size too large

  // just going to arbitrarily use 1 as the character indicating incorrect connection to otp_dec_d
  if (buffer[0] == '1') 
  {
    fprintf(stderr, "ERROR: otp_enc not permitted to connect to otp_dec_d on port %d\n", portNumber);
    exit(2);
  }

  printf("%s\n", buffer);
  close(socketFD); // Close the socket
  return 0;
}

// will fill filetext with contents of filename
int getFileText(char *filename, char filetext[BUFF_SIZE] )
{
  FILE *FD = fopen(filename, "r");
  if (FD == NULL)
    return -1;

  fgets(filetext, BUFF_SIZE, FD);
  filetext[strcspn(filetext, "\n")] = '\0'; // replace newline with terminator
  fclose(FD);
}

// will validate that message contains only valid chars (A - Z or " " <whitespace>)
int validChars(char *message) 
{
  int i;
  for (i = 0; i < strlen(message); i++) 
  {
    if (! (message[i] == ' ' || (message[i] >= 'A' && message[i] <= 'Z')) )
    {
      return 0;
    }
  }
  return 1;
}
