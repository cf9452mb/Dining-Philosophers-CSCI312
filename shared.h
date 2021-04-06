//This is the shared header file for Project5
#ifndef SHARED_H
#define SHARED_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define N 5  //Number of philosophers

//Message structure used in sending through sockets
struct message{
  int id;
  int port;
  int values;
  int list[N + 1];
  char buffer[1024];
};

//Find largest element in an array
int largest(int arr[], int size);

//Create a socket
int createSocket() {
  int socketConnection;

  socketConnection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(socketConnection < 0) {
    perror("client: socket creation failed");
    exit(1);
  }

  return socketConnection;
}

//Function to start up the Coordinator process. Passes values in msg.list[] and the port to the process
void startCoordinator(struct message msg, int value){

  char first[4], second[4], third[4], fourth[4], fifth[4], sixth[4];
  char port[4];

  sprintf(first, "%d", msg.list[0]);
  sprintf(second, "%d", msg.list[1]);
  sprintf(third, "%d", msg.list[2]);
  sprintf(fourth, "%d", msg.list[3]);
  sprintf(fifth, "%d", msg.list[4]);
  sprintf(sixth, "%d", msg.list[5]);
  sprintf(port, "%d", (value + 6));

  execl("./coordinator", "coordinator", first, second, third, fourth, fifth, sixth, port, (char *)NULL);

}

#endif
