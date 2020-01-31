  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <iostream>
  
  #include <opencv2/opencv.hpp>
  
  using namespace cv;
  using namespace std;
  
  int main(void)
  {
    Mat image_recieved;
    struct sockaddr_in sa;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
      perror("Can not create a Socket");
      exit(EXIT_FAILURE);
    }
  
    memset(&sa, 0, sizeof sa);
  
    sa.sin_family = AF_INET;
    sa.sin_port = htons(1100);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
  
    if (bind(SocketFD,(struct sockaddr *)&sa, sizeof sa) == -1) {
      perror("bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
  
  
    for (;;) {
      
      if (listen(SocketFD, 10) == -1) {
        perror("listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
      int ConnectFD = accept(SocketFD, NULL, NULL);
  
      if (0 > ConnectFD) {
        perror("accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
      
      Mat  img = Mat::zeros(320,240, CV_8UC3);
      long imgSize = img.total()*img.elemSize();
      char sockData[921600];
      long bytes;
      //Receive data here

      for (int i = 0; i < imgSize; i += bytes) {
        if ((bytes = recv(ConnectFD, sockData +i, imgSize  - i, 0)) == -1)  {
          perror("recieve failed");
          close(SocketFD);
          exit(EXIT_FAILURE);
        }
      }

      // Assign pixel value to img
      int ptr=0;
      for (int i = 0;  i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
          img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
          ptr=ptr+3;
          }
      }
        
        
      resize(img, img, Size(640,480));
      namedWindow("Stream");
      imshow("Stream", img);
      waitKey(30);
    }
    destroyAllWindows();
    close(SocketFD);
    return EXIT_SUCCESS;  
}
