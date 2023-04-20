#include "server.h"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <arpa/inet.h>
#include <algorithm>
#include <unordered_map>
#include <cstdio>
#include <memory>
#include <stdexcept>

#include "../logger/logger.h"

using namespace std;

string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return tolower(c); });
    return result;
}

string getMimeType(const string& requestPath) {
    string type = requestPath.substr(requestPath.find_last_of(".") + 1);
    type = toLower(type);

    if (type == "css") {
        type = "text/css";
    } else {
        type = "text/html";
    }

    return type;
}

unordered_map<string, string> getParams(const string& queryString) {
    unordered_map<string, string> params;

    stringstream ss(queryString);
    string pair;
    while (getline(ss, pair, '&')) {
        stringstream ssPair(pair);
        string key;
        if (getline(ssPair, key, '=')) {
            string value;
            if (getline(ssPair, value)) {
                params[key] = value;
            }
        }
    }

    return params;
}

string executeCommand(const string& command) {
    shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    char buffer[128];
    string result;
    while (fgets(buffer, 128, pipe.get()) != nullptr) {
        result += buffer;
    }
    return result;
}

void handleRequest(int clientSocket, const string& requestURI) {
    Logger logger("log.txt", INFO);
    logger.info("handling request for URI: " + requestURI);

    // Extract the request path and query string
    size_t pos = requestURI.find('?');
    std::string requestPath = requestURI.substr(0, pos);
    std::string queryString;
    if (pos != std::string::npos) {
        queryString = requestURI.substr(pos + 1);
    }

    // Build full file path
    string filePath = "./routes" + requestPath + ".cpp";

    // Check if request URI is root URL
    if (requestPath == "/") {
        logger.info("serving default content: index.cpp");

        // Read file contents into buffer
        ifstream file("index.cpp");
        stringstream buffer;
        buffer << file.rdbuf();
        string sourceCode = buffer.str();

        // Construct HTTP response
        stringstream responseBuffer;
        responseBuffer << "HTTP/1.1 200 OK\r\n"
                       << "Content-Type: text/plain\r\n"
                       << "Content-Length: " << sourceCode.size() << "\r\n"
                       << "\r\n"
                       << sourceCode;
        string responseStr = responseBuffer.str();

        // Send response
        write(clientSocket, responseStr.c_str(), responseStr.size());
    } else {  // Request is not root URL
        // Try to open file
        ifstream file(filePath);
        if (!file) {  // File not found
            logger.warning("file not found for URI: " + requestURI);

            // Send 404 response
            stringstream buffer;
            buffer << "HTTP/1.1 404 Not Found\r\n"
                   << "Content-Type: text/html\r\n"
                   << "\r\n"
                   << "<html><body><h1>404 Not Found</h1></body></html>";
            string responseStr = buffer.str();
            write(clientSocket, responseStr.c_str(), responseStr.size());
        } else {  // File found
            logger.info("serving file for URI: " + requestURI);

            // Read file contents into buffer
            stringstream buffer;
            buffer << "#include <iostream>\n"
                   << "#include <string>\n"
                   << "\n"
                   << "using namespace std;\n"
                   << "\n"
                   << "void handleRequest(const string& queryString) {\n"
                   << file.rdbuf()
                   << "}\n"
                   << "\n"
                   << "int main() {\n"
                   << "    handleRequest(\"" << queryString << "\");\n"
                   << "    return 0;\n"
                   << "}\n";
            string sourceCode = buffer.str();

            // Compile and execute the CPP file
            string compileCommand = "g++ -x c++ -o temp -";
            string output = executeCommand(compileCommand + sourceCode);
            logger.info("compile output: " + output);

            output = executeCommand("./temp");
            logger.info("execution output: " + output);

            // Send the response
            stringstream responseBuffer;
            responseBuffer << "HTTP/1.1 200 OK\r\n"
                           << "Content-Type: text/html\r\n"
                           << "\r\n"
                           << output;
            string responseStr = responseBuffer.str();
            write(clientSocket, responseStr.c_str(), responseStr.size());
        }
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
