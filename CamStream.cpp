#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
int main(void)
{
  const char *IP = "172.31.12.102";
  
  struct sockaddr_in sa;
  int res;
  int SocketFD;
  VideoCapture cap;
  if(!cap.open(0)) {
        printf("Camera doesn`t work");
        return 0;
  }
  for (;;) {
        SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (SocketFD == -1) {
          perror("Failed to create a socket");
          exit(EXIT_FAILURE);
        } 
        memset(&sa, 0, sizeof sa);
        sa.sin_family = AF_INET;
        sa.sin_port = htons(1100);
        res = inet_pton(AF_INET, IP, &sa.sin_addr);
        connect(SocketFD, (struct sockaddr *)&sa, sizeof sa);
        
        Mat image;
        cap >> image;
        resize(image, image, Size(320,240));
        long imgSize = image.total()*image.elemSize();
        send(SocketFD, reinterpret_cast<char*>(image.data), imgSize, 0);
  }
  destroyAllWindows();
  shutdown(SocketFD, SHUT_RDWR);
  return EXIT_SUCCESS;
}
