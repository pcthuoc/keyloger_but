#include "chap03.h"
#include <time.h>
#include <ctype.h>

void writefile(char read[]) {
  FILE * fp;
  const char filePath[] = "/home/pcthuoc/Desktop/server/log/";
  char filename[] = "log.txt";
  //sprintf(ip_adrr, "%s", filename);
  strcat(filePath,filename);
  fp = fopen(filePath, "a");
  char writestr[2];
  memset(writestr, '\0', 2);
  writestr[0] = read[0];
  fputs(read, fp);
  fclose(fp);
  return;
}
int main(int argc, char * argv[]) {

  #if defined(_WIN32)
  WSADATA d;
  if (WSAStartup(MAKEWORD(2, 2), & d)) {
    fprintf(stderr, "Failed to initialize.\n");
    return 1;
  }
  #endif

  printf("Configuring local address...\n");
  struct addrinfo hints;
  memset( & hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo * bind_address;
  getaddrinfo(0, argv[1], & hints, & bind_address);

  printf("Creating socket...\n");
  SOCKET socket_listen;
  socket_listen = socket(bind_address -> ai_family,
    bind_address -> ai_socktype, bind_address -> ai_protocol);
  if (!ISVALIDSOCKET(socket_listen)) {
    fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }

  printf("Binding socket to local address...\n");
  if (bind(socket_listen,
      bind_address -> ai_addr, bind_address -> ai_addrlen)) {
    fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }
  freeaddrinfo(bind_address);

  printf("Listening...\n");
  if (listen(socket_listen, 10) < 0) {
    fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }

  fd_set master;
  FD_ZERO( & master);
  FD_SET(socket_listen, & master);
  SOCKET max_socket = socket_listen;

  printf("Waiting for connections...\n");
  int dem = 50;
  test:
    while (1) {
      fd_set reads;
      reads = master;
      if (select(max_socket + 1, & reads, 0, 0, 0) < 0) {
        fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
      }

      SOCKET i;
      for (i = 1; i <= max_socket; ++i) {
        if (FD_ISSET(i, & reads)) {

          if (i == socket_listen) {
            struct sockaddr_storage client_address;
            socklen_t client_len = sizeof(client_address);
            SOCKET socket_client = accept(socket_listen,
              (struct sockaddr * ) & client_address, &
              client_len);
            if (!ISVALIDSOCKET(socket_client)) {
              fprintf(stderr, "accept() failed. (%d)\n",
                GETSOCKETERRNO());
              return 1;
            }

            FD_SET(socket_client, & master);
            if (socket_client > max_socket)
              max_socket = socket_client;

            char address_buffer[100];
            getnameinfo((struct sockaddr * ) & client_address,
              client_len,
              address_buffer, sizeof(address_buffer), 0, 0,
              NI_NUMERICHOST);
            printf("New connection from %s\n", address_buffer);

          } else {
          char read[100];
			memset(read, '\0', 100);
			int bytes_received = recv(i, read, 100, 0);
			puts(read);
			
			writefile(read);
			if (bytes_received < 1)
			{
				FD_CLR(i, &master);
				CLOSESOCKET(i);
				continue;
			}
			if (read[0]=='\n')
			{
			   time_t timer;
			   struct tm *info;
			   time( &timer );
			   info = localtime( &timer );
			   char filename[50], duoi[]="jpg";
			   
			   sprintf(filename,"%s",asctime(info));
			   for(int i=0;i<strlen(filename);i++)
					if (filename[i]==' ')
						filename[i]='_';
					else
					if (filename[i]=='\n')
						filename[i]='.';
				
			    strcat (filename, duoi);
			    
			    writefile(filename);
			    writefile(read);
			    char buff[BUFSIZ];
			    memset(buff, '\0', BUFSIZ);
			    //  int bytes_received = recv(i, read, 10, 0);

			    //printf("Reading Picture Size\n");
			    recv(i, buff, BUFSIZ, 0);
			    int file_size = atoi(buff);

			    if (file_size < 1) {
			      FD_CLR(i, & master);
			      CLOSESOCKET(i);
			      continue;
			    }
			    //printf("Picture size:%d", file_size);
			    //char filename[] = "t_image.jpg";
			    //filename[0] = dem;
  			    const char filePath[] = "/home/pcthuoc/Desktop/server/log/";
  			    strcat(filePath,filename);
			    FILE * imageFile;
			    imageFile = fopen(filePath, "ab");
			    //printf("Reading Picture Byte Array\n");
			    ssize_t len;
			    int remain_data = file_size;
			    int reciveddata = 0;
			    //printf("Recive Started:\n");
			    while ((remain_data > 0) && ((len = recv(i, buff, BUFSIZ, 0)) > 0)) {
			      //imageFile.write(buff, len);
			      fwrite(buff, len, 1, imageFile);
			      memset(buff, '\0', BUFSIZ);
			      reciveddata += len;
			      remain_data -= len;
			      //int percent = (reciveddata*1.0 / file_size*1.0) * 100;
			      // printf("Recive:%d\n", percent);

			    }

			    fclose(imageFile);
			    dem++;
			    printf("Recived Size:%d\n", reciveddata);
			    //goto test;
		       }
		       
		  }

        } //if FD_ISSET
      } //for i to max_socket
    } //while(1)

  printf("Closing listening socket...\n");
  CLOSESOCKET(socket_listen);

  #if defined(_WIN32)
  WSACleanup();
  #endif

  printf("Finished.\n");

  return 0;
}
