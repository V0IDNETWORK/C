#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")


int main(){
    
    WSADATA wsa;

    WSAStartup(
        MAKEWORD(2,2),
        &wsa
    );

    SOCKET serverSocket;

    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );


    struct sockaddr_in server ;

    server.sin_family = AF_INET;

    server.sin_port = htons(8080);

    server.sin_addr.s_addr = INADDR_ANY;

    bind(
        serverSocket,
        (struct sockaddr*)&server,
        sizeof(server)
    );

    listen(
        serverSocket,
        5
    );


    printf("\n\tWaiting For A client... \n\t");


    SOCKET clientSocket;

    clientSocket = accept(
        serverSocket,
        NULL,
        NULL
    );

    printf("\n\n\t Client connected!\n\n\t ");

    char buffer[1024];

    recv(
        clientSocket,
        buffer,
        sizeof(buffer),
        0
    );


    printf("\nClient: %s\n", buffer);

    send(
        clientSocket,
        "\nHello, Client",
        12,
        0
    );


    closesocket(clientSocket);
    closesocket(serverSocket);

    WSACleanup();

    return 0;
}