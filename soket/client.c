#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


int main(){
    WSADATA wsa;
    WSAStartup(
        MAKEWORD(2,2),
        &wsa
    );

    SOCKET client;

    client = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );


    struct sockaddr_in server;
    server.sin_family = AF_INET;

    server.sin_port = htons(8080);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server.sin_addr
    );

    connect(
        client,
        (struct sockaddr*)&server,
        sizeof(server)
    );

    send(
        client,
        "\nHello Server",
        12,
        0
    );


    char buffer[1024];

    recv(
        client,
        buffer,
        sizeof(buffer),
        0
    );

    printf("\n\n\t Server: %s \n", buffer);

    closesocket(client);

    WSACleanup();

    return 0;
}