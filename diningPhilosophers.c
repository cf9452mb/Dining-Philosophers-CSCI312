//This is the program that each dining philosopher runs. It communicates directly with the Coordinator via a socket
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "shared.h"

int main(int argc, char *argv[]){

  int id, coordPort, eatLength, thinkLength;
  int isEating = 0;
  int exit = 0;
  struct message msg;
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  time_t seconds;

  id = atoi(argv[1]);
  msg.id = id;
  coordPort = atoi(argv[2]);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(coordPort);

  srand(time(0));
  eatLength = (rand()%5) + 1;
  thinkLength = (rand()%5) + 1;

  printf("I am a Dining Philosopher\n");
  printf("My ID is = %d\n", id);
  printf("Coordinator port = %d\n", coordPort);
  printf("My random eat time is %d seconds.\n", eatLength);
  printf("My random think time is %d seconds.\n", thinkLength);

  //Sleep for a second to ensure Coordinator has setup server socket for communication
  sleep(1);

  printf("Philosopher %d is thinking\n", id);
  sleep(thinkLength);

  //Connect here to the Coordinators socket using the port number in coordPort
  int socketfd = createSocket();
  if( connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("ERROR: Philospher failed to connect.\n");
    return -1;
  }

  while(exit == 0) {
    if(isEating == 0) { //If not eating, start eating.
      struct message msgRec;
      strcpy(msg.buffer, "REQUEST");
      sendMsg(socketfd, msg);
      msgRec = recMsg(socketfd);
      if( (strcmp(msgRec.buffer, "OK") == 0)) {
        seconds = time(NULL);
        printf("Philospher %d is eating. Time: %d\n", id, seconds);
        isEating = 1;
        sleep(eatLength);
      }

    }
    else { //If eating, stop.
      struct message msgRec;
      strcpy(msg.buffer, "RELEASE");

      sendMsg(socketfd, msg);

      printf("Philospher %d is thinking.\n", id);
      isEating = 0;
      sleep(thinkLength);
    }
  }

  return 0;

}
