#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define buffersize 1024

int main(int argc, char *argv[]){
 
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Ref https://stackoverflow.com/a/39298615/4123703
    unsigned char options[] = {
        30,    // option type 30 (experimental)
        14,    // option length
        1,2,3,4,5,6,7,8,9,10,11,12,   // option data
        1,     // option type 1 (no-op, no length field)
        1      // option type 1 (no-op, no length field)
    };
    // unsigned char options[40] = {1};
    if (setsockopt(sock, IPPROTO_IP, IP_OPTIONS, (char *)&options, sizeof(options))== -1) {
        perror("Error setting options");
        close(sock);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    serv_addr.sin_port = htons(5566); 
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    const char *message = "1";
    printf("send\n");
    send(sock, message, strlen(message)+1, 0) ;

    char *buffer = (char*) calloc(buffersize, sizeof(char)) ;
    printf("read\n");
    read(sock, buffer, buffersize);
    printf("result form server: %s\n", buffer);

    // Get ip options set on sock
    // Result: only the ip options we set are retrieved
    struct ip_opts getopt = {0};
    socklen_t optlen = sizeof(getopt);
    if (getsockopt(sock, IPPROTO_IP, IP_OPTIONS, (char *)&getopt, &optlen)== -1) {
        perror("Error get setting options");
    }
    printf("retrieved options dst %u\n", getopt.ip_dst.s_addr);
    for(int i = 0; i < 4; i++){
        printf("retrieved options dst %u\n", (getopt.ip_dst.s_addr >> (i*2)) & 0xFF );
    }
    printf("====ip_opts start ====\n");
    for(size_t i = 0; i < sizeof(getopt.ip_opts); i++){
        printf("%d ", getopt.ip_opts[i]);
    }
    printf("\n====ip_ipts end====\n");


    free(buffer);
    close(sock);
    return 0;
}
