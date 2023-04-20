#include "server.h"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <arpa/inet.h>

#include "../logger/logger.h"

using namespace std;

void handleRequest(int clientSocket, const string& requestPath) {
    Logger logger("log.txt", INFO);
    logger.info("handling request for path: " + requestPath);

    // Build full file path
    string filePath = "./www" + requestPath;
    if (requestPath == "/") {
        filePath += "index.html";
    }

    // Try to open file
    ifstream file(filePath);
    if (!file) {  // File not found
        logger.warning("file not found for path: " + requestPath);

        // Send 404 response with HTTP cat image
        stringstream buffer;
        buffer << "HTTP/1.1 302 Found\r\n"
               << "Content-Type: text/html\r\n"
               << "Location: https://http.cat/404.jpg\r\n"
               << "\r\n";
        string responseStr = buffer.str();
        write(clientSocket, responseStr.c_str(), responseStr.size());
    } else {  // File found
        logger.info("serving file for path: " + requestPath);

        // Read file contents into buffer
        stringstream buffer;
        buffer << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "\r\n"
               << file.rdbuf();
        string responseStr = buffer.str();
        write(clientSocket, responseStr.c_str(), responseStr.size());
    }
    close(clientSocket);
}

Server::Server(int port) : m_port(port) {}

void Server::start() {
    Logger logger("log.txt", INFO);
    logger.info("starting server");

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0) {
        logger.error("failed to create server socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    int bindResult = bind(serverSocket, (struct sockaddr*) &address, sizeof(address));
    if (bindResult < 0) {
        logger.error("failed to bind to port " + to_string(m_port));
        exit(EXIT_FAILURE);
    }
    logger.info("bound to port " + to_string(m_port));

    if (listen(serverSocket, 5) < 0) {
        logger.error("failed to start listening on port " + to_string(m_port));
        exit(EXIT_FAILURE);
    }
    logger.info("started listening on port " + to_string(m_port));

    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientAddressSize);
        if (clientSocket < 0) {
            logger.error("failed to accept client connection");
            continue;
        }
        logger.info("client connected from " + string(inet_ntoa(clientAddress.sin_addr)) + ":" + to_string(ntohs(clientAddress.sin_port)));

        // Read request
        stringstream requestBuffer;
        char buffer[4096] = {0};
        ssize_t bytesRead;
        do {
            bytesRead = read(clientSocket, buffer, sizeof(buffer));
            requestBuffer.write(buffer, bytesRead);
        } while (bytesRead == sizeof(buffer));
        string requestStr = requestBuffer.str();
        logger.debug("received request from client: " + requestStr);

        // Get request path
        size_t start = requestStr.find(' ') + 1;
        size_t end = requestStr.find(' ', start);
        string requestPath = requestStr.substr(start, end - start);
        logger.debug("requested path: " + requestPath);

        handleRequest(clientSocket, requestPath);
    }
}

