#include "server.h"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "logger.h"

using namespace std;

void handleRequest(int clientSocket) {
    stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/plain\r\n"
             << "\r\n"
             << "Hello, World!\r\n";
    string responseStr = response.str();
    write(clientSocket, responseStr.c_str(), responseStr.size());
    close(clientSocket);
}

void startServer(int port) {
    Logger logger("log.txt", INFO);
    logger.info("starting server");
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(serverSocket, (struct sockaddr*) &address, sizeof(address));
    listen(serverSocket, 5);
    logger.info("started server on http://localhost:8080");

    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientAddressSize);
        handleRequest(clientSocket);
    }
}
