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
int largest(int arr[], int size){

  int i;

  // Initialize maximum element
  int max = arr[0];

  // Traverse array elements from second and
  // compare every element with current max
  for (i = 1; i < size; i++)
    if (arr[i] > max)
      max = arr[i];

  return max;
}

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
void startCoordinator(struct message msg, struct message msgID, int value){

  char first[4], second[4], third[4], fourth[4], fifth[4], sixth[4];
  char id[4], port[4];

  sprintf(first, "%d", msg.list[0]);
  sprintf(second, "%d", msg.list[1]);
  sprintf(third, "%d", msg.list[2]);
  sprintf(fourth, "%d", msg.list[3]);
  sprintf(fifth, "%d", msg.list[4]);
  sprintf(sixth, "%d", msg.list[5]);
  sprintf(id, "%d", msgID.id);
  sprintf(port, "%d", (value + 1000));  //Add 1000 to the port to make sure its unique

  execl("./coordinator", "coordinator", first, second, third, fourth, fifth, sixth, id, port, (char *)NULL);

}

//Function to start up the Dining Philosophers process. Passes the processes ID and the Coordinator's port to the program
void startPhilosopher(struct message msg, int value){

  char id[4], port[4];
  sprintf(id, "%d", msg.id);
  sprintf(port, "%d", (value + 2024));  //Add 2024 to match the Coordinator's port

  execl("./diningPhilosophers", "diningPhilosophers", id, port, (char *)NULL);

}

#endif
