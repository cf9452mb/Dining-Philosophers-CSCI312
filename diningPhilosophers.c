//This is the program that each dining philosopher runs. It communicates directly with the Coordinator via a socket
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

int main(int argc, char *argv[]){

  int id, coordPort;

  id = atoi(argv[1]);
  coordPort = atoi(argv[2]);

  printf("I am a Dining Philosopher\n");
  printf("My ID is = %d\n", id);
  printf("Coordinator port = %d\n", coordPort);

  //Sleep for a second to ensure Coordinator has setup server socket for communication
  sleep(1);

  //Connect here to the Coordinators socket using the port number in coordPort

  return 0;

}
