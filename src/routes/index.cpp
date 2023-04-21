#include <iostream>
#include <string>
#include <sys/socket.h>    // For send()

using namespace std;

extern "C" {
void handleIndex(int clientSocket, const string& requestURI) {
    // Remove leading and trailing slashes from the requested path
    string requestedPath = requestURI.substr(1);
    if (requestedPath.back() == '/') {
        requestedPath = requestedPath.substr(0, requestedPath.length() - 1);
    }

    // Construct the HTTP response header
    string responseHeader =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";

    // Construct the HTTP response body
    string responseBody =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>My Awesome Web Server</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Welcome to my awesome web server!</h1>\n"
        "<p>This is the root page.</p>\n"
        "</body>\n"
        "</html>\n";

    // Send the HTTP response to the client
    string response = responseHeader + responseBody;
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    if (bytesSent < 0) {
        perror("send() failed");
        return;
    }
}
}

/*
For not serving a file:

#include <string>

using namespace std;

void handleRequest(const string& queryString) {
    cout << "<html>\n"
         << "<head>\n"
         << "    <title>My Website</title>\n"
         << "</head>\n"
         << "<body>\n"
         << "    <h1>Welcome to My Website!</h1>\n"
         << "    <p>Thanks for visiting. Check out our <a href=\"/about\">About</a> page to learn more about us.</p>\n"
         << "</body>\n"
         << "</html>";
}
*/