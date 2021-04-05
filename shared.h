//This is the shared header file for Project5
#ifndef SHARED_H
#define SHARED_H

#include <sys/socket.h>
#include <arpa/inet.h>

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

#endif
