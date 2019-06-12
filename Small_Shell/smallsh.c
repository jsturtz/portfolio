#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef MAX_ARGS
#define MAX_ARGS 512
#endif

#ifndef CMD_LENGTH
#define CMD_LENGTH 2048
#endif

#ifndef WORD_LENGTH
#define WORD_LENGTH 100
#endif

#ifndef NUM_PROCESSES
#define NUM_PROCESSES 50
#endif

// global needed for handling CTRL-Z signal
int   pauseProgram = 0;

// removes newline at end of input buffer, replaces with null terminator
void stripNewline(char* inputBuffer) 
{
  int i = 0; 
  while (inputBuffer[i] != '\n') i++;
  inputBuffer[i] = '\0';
}

// expands $$ into process id
void expandPID(char* inputBuffer, int pid) 
{
  int i = 0; 
  char firstHalf[CMD_LENGTH]  = { 0 };
  char secondHalf[CMD_LENGTH] = { 0 };

  while (inputBuffer[i] != '\0')
  {
    if (inputBuffer[i] == '$' && inputBuffer[i+1] == '$')
    {
      strncpy(firstHalf, inputBuffer, i);
      strncpy(secondHalf, inputBuffer+i+2, CMD_LENGTH - i - 2);
      sprintf(inputBuffer, "%s%d%s", firstHalf, pid, secondHalf);
    }
    i++;
  }
}

// used to free and set null the pointers held by args
void deleteArgs(char** args) 
{
  int a = 0; 
  while (args[a] != NULL) 
  {
    free(args[a]);
    args[a] = NULL;
    a++;
  }
}

// used to memory allocate new arg and put pointer into args
void addArg(char** args, char* newArg) 
{
  // find index of next spot in args array
  int a = 0;
  while (args[a] != NULL) a++;

  // allocate memory and copy characters into new pointer
  args[a] = malloc(sizeof(char) * WORD_LENGTH);
  strcpy(args[a], newArg);
}

// splits raw inputBuffer into args string array
// also fills contents of in/out with filenames and sets bg flag
int parse(char* inputBuffer, char** args, char* in, char* out, int* bg) {
  
  // frees all pointers in args, sets to Null
  deleteArgs(args);

  // reset in/out/bg
  in[0] = '\0';
  out[0] = '\0';
  *bg = 0;

  // iterate over tokenized words
  char* token = strtok(inputBuffer, " "); 
  while (token) 
  {
    // if input redirect symbol detected, fill contents of in
    if (token[0] == '<' && token[1] == '\0')
    {
      token = strtok(NULL, " ");        // skip the next word
      strcpy(in, token);
    }

    // if output redirect symbol detected, fill contents of out
    else if (token[0] == '>' && token[1] == '\0')
    {
      token = strtok(NULL, " ");        // skip the next word
      strcpy(out, token);
    }

    // if background symbol detected, set bg flag and return
    else if (token[0] == '&' && token[1] == '\0')
    {
      *bg = 1;
      return 0;
    }
    else 
    {
      addArg(args, token); // allocates space for array, fills with contents of token
    }
    token = strtok(NULL, " "); // keep searching for args
  }
}

// function that handles built-in "cd" command
int changeDir(char* path, char* error) 
{
  // if path provided, change to it
  if (path) 
  {
    if (chdir(path) == -1) 
    {
      sprintf(error, "Path doesn't exist: %s", path);
      return -1;
    }
  }
  // if not, change to home dir
  else 
  {
    chdir(getenv("HOME"));
  }
  return 0;
}

// function that handles built-in "exit" command
void exitProgram(int* children, int childrenCount) 
{
  // kill children and exit
  for (int c = 0; c < childrenCount; c++) 
  {
    kill(children[c], SIGKILL);
  }
  exit(0);
}

// parent handlers will not terminate shell
void parentSIGINT(int signo) {
  int childExitStatus = -5;
  wait(&childExitStatus);
}

// toggles pauseProgram and writes relevant message
void parentSIGTSTP(int signo) {
  char message[31];
  if (pauseProgram) 
  {
    strcpy(message, "Exiting foreground-only mode\n");
    write(STDOUT_FILENO, message, 30);
  }
  else 
  {
    strcpy(message, "Entering foreground-only mode\n");
    write(STDOUT_FILENO, message, 31);
  }
  pauseProgram = (pauseProgram == 0); // toggle global pause variable
}

// used to reset array data structure so that there are no "holes",
// i.e. a value of 0 counts as the end of the array
void resetArray(int* arr, int oldSize) 
{
  // temp will be filled with any nonzero values found in arr
  int temp[oldSize];
  memset(temp, 0, sizeof(int) * oldSize);

  int oldIndex = 0;
  int newIndex = 0;

  for (oldIndex = 0; oldIndex < oldSize; oldIndex++) 
  {
    if (arr[oldIndex] != 0)
    {
      temp[newIndex] = arr[oldIndex];
      newIndex++;
    }
  }

  // then, empty arr and fill with values in temp to eliminate "holes"
  memset(arr, 0, sizeof(int) * oldSize);
  for (newIndex = 0; newIndex < oldSize; newIndex++) 
  {
    arr[newIndex] = temp[newIndex];
  }
}

int main() {

  char  inputBuffer[CMD_LENGTH]     = {'\0'};   // holds entire line read in from stdin
  char  cmd[CMD_LENGTH]             = {'\0'};   // holds command to be executed
  char  in[CMD_LENGTH]              = {'\0'};   // holds filename for stdin redirection
  char  out[CMD_LENGTH]             = {'\0'};   // holds filename for stdout redirection
  char  startDir[CMD_LENGTH]        = {'\0'};   // holds path for current working directory
  char  error[100]                  = {'\0'};   // holds error message from failed function calls

  // args will hold all the pointers to pointers, initialized to null
  char** args = (char**) calloc(MAX_ARGS, sizeof(char*));
  for (int i = 0; i < MAX_ARGS; i++) 
  {
    args[i] = NULL;
  }

  int   children[NUM_PROCESSES]     = { 0 };    // holds error message from failed function calls
  int   childrenCount   = 0;                    // holds number of child processes

  int   bg              = 0;                    // flag for whether command should be backgrounded
  int   fgExitStatus    = 0;                    // holds exit status for foregrounded processes that terminate
  char  fgExitMessage[CMD_LENGTH]   = {'\0'};   // holds exit message for foregrounded processes that terminate
  
  pid_t spawnPid;                               // used for forking below
  int   childExitStatus;                        // will hold exit status retrieved from wait/waidpid
  
  // keep signal handling separate for child/parent
  struct sigaction SIGINT_action = {0};          
  struct sigaction SIGTSTP_action = {0};
  struct sigaction SIGINT_childAction = {0};
  struct sigaction SIGTSTP_childAction = {0};

  while(1) {

    // check if children have become zombies
    int c = 0;
    while (children[c] != 0)
    {
      // if zombie detected among children
      if (waitpid(children[c], &childExitStatus, WNOHANG) != 0)
      {
        // check if exit was normal
        if (WIFEXITED(childExitStatus))
        {
          int exitStatus = WEXITSTATUS(childExitStatus);
          printf("Process %d completed normally with exit status %d\n", children[c], exitStatus);
          fflush(stdout);
        }
        // if not, it was killed by signal
        else
        {
          int exitSignal = WIFSIGNALED(childExitStatus);
          printf("Process %d terminated by signal %d\n", children[c], exitSignal);
          fflush(stdout);
        }
        children[c] = 0; // set value to zero to be cleaned up by resetArray
        childrenCount--; 
      }
      c++;
    }
    resetArray(children, childrenCount);

    // get input from user
    printf(": ");
    fflush(stdout);
    memset(inputBuffer, CMD_LENGTH, '\0');
    if (fgets(inputBuffer, CMD_LENGTH, stdin)) // will return NULL upon receipt of signal so this is necessary
    {   
      stripNewline(inputBuffer);        // replace automatic newline with null terminator
      expandPID(inputBuffer, getpid()); // do variable expansion on $$

      // fills args with everything needed for execvp
      // fills in/out with filenames if exist, sets bg flag for parent
      parse(inputBuffer, args, in, out, &bg); 
      
      // do something only if (a) command given or (2) not a comment line
      if (args[0] && args[0][0] != '\0' && args[0][0] != '#')
      {
        // built-in commands here
        if (strcmp(args[0], "exit") == 0) 
        {
          exitProgram(children, childrenCount);
        }

        else if (strcmp(args[0], "status") == 0) 
        {
          printf("%s", fgExitMessage);
          fflush(stdout);
        }

        else if (strcmp(args[0], "cd") == 0) 
        {
          if (changeDir(args[1], error) == -1) 
          {
            printf("ERROR: %s\n", error);
            fflush(stdout);
          }
        }

        // execute non-built in command with new process
        else 
        {
          spawnPid = -5;
          childExitStatus = -5;
          spawnPid = fork();
          switch (spawnPid)
          {
            // something went horribly wrong
            case -1:
              perror("Hull Breach!\n");
              exit(1);
              break;

            // child process will get spawnpid == 0
            case 0:

              // both fg and bg should ignore SIGTSTP
              SIGTSTP_childAction.sa_handler = SIG_IGN;
              sigfillset(&SIGTSTP_childAction.sa_mask);
              SIGTSTP_childAction.sa_flags = 0;
              sigaction(SIGTSTP, &SIGTSTP_childAction, NULL);
              
              // set to ignore SIGINT only if program backgrounded (must also not have been paused by SIGTSTP)
              if (bg && !pauseProgram)
              {
                SIGINT_childAction.sa_handler = SIG_IGN;
                sigfillset(&SIGINT_childAction.sa_mask);
                SIGINT_childAction.sa_flags = 0;
                sigaction(SIGINT, &SIGINT_childAction, NULL);
              }
            
              // set redirection for output
              if (out[0] != '\0') 
              {
                int outFile = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (outFile >= 0) 
                {
                  dup2(outFile, 1);
                  close(outFile);
                }
                else    // something went wrong in opening file
                {
                  printf("cannot open %s for output\n", out);
                  fflush(stdout);
                  exit(1);
                }
              }

              // set redirection for input
              if (in[0] != '\0') 
              {
                int inFile = open(in, O_RDONLY, 0666);
                if (inFile >= 0) 
                {
                  dup2(inFile, 0);
                  close(inFile);
                }
                else    // something went wrong in opening file
                {
                  printf("cannot open %s for input\n", in);
                  fflush(stdout);
                  exit(1);
                }
              }

              // If process to be backgrounded, change redirections to dev/null
              if (bg && !pauseProgram) 
              {
                int devNull = open("/dev/null", O_RDWR);
                dup2(devNull, 0);
                dup2(devNull, 1);
              }
              
              // execute command, print error and set exit status if fails
              if (execvp(args[0], args) == -1)
              {
                printf("ERROR: Invalid command\n"); 
                fflush(stdout);
                exit(1);
              }
              break;

            default:

              // signal handlers for parent
              // function handlers
              SIGINT_action.sa_handler = parentSIGINT;
              SIGTSTP_action.sa_handler = parentSIGTSTP;

              // block all signals while handler active
              sigfillset(&SIGINT_action.sa_mask);
              sigfillset(&SIGTSTP_action.sa_mask);

              // no special options
              SIGINT_action.sa_flags = 0;
              SIGTSTP_action.sa_flags = 0;
              
              // call sigaction to register handlers
              sigaction(SIGINT, &SIGINT_action, NULL);
              sigaction(SIGTSTP, &SIGTSTP_action, NULL);

              // wait only if backgrounded or paused
              if (bg && !pauseProgram)
              {
                printf("background pid is %d\n", spawnPid);
                children[childrenCount] = spawnPid;
                childrenCount++;
              }
              else
              {
                // since not backgrounded, wait here
                waitpid(spawnPid, &childExitStatus, 0);

                // if child exited normally: 
                if (WIFEXITED(childExitStatus))
                {
                  fgExitStatus = WEXITSTATUS(childExitStatus);
                  sprintf(fgExitMessage, "exit value %d\n", fgExitStatus);
                }

                // if killed by signal:
                else if (WIFSIGNALED(childExitStatus))
                {
                  fgExitStatus = WTERMSIG(childExitStatus);
                  printf("terminated by signal %d\n", fgExitStatus);
                  sprintf(fgExitMessage, "terminated by signal %d\n", fgExitStatus);
                  fflush(stdout);
                }
              }
              break;
          }
        }
      }

    }
  }

  // free space
  deleteArgs(args); // frees space of each pointer within args
  free(args);       // frees args itself
}  
