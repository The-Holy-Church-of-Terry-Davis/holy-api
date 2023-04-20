#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void handleRequest(int clientSocket, const string& requestURI) {
    // Extract the request path and query string
    size_t pos = requestURI.find('?');
    std::string requestPath = requestURI.substr(0, pos);
    std::string queryString;
    if (pos != std::string::npos) {
        queryString = requestURI.substr(pos + 1);
    }

    // Check if request URI is root URL
    if (requestPath == "/") {
        // Read file contents into buffer
        ifstream file("www/index.html");
        stringstream buffer;
        buffer << file.rdbuf();
        string fileContents = buffer.str();

        // Construct HTTP response
        stringstream responseBuffer;
        responseBuffer << "HTTP/1.1 200 OK\r\n"
                       << "Content-Type: text/html\r\n"
                       << "Content-Length: " << fileContents.size() << "\r\n"
                       << "\r\n"
                       << fileContents;
        string responseStr = responseBuffer.str();

        // Send response
        write(clientSocket, responseStr.c_str(), responseStr.size());
    } else {  // Request is not root URL
        // TODO: Implement handling of other routes
    }

    // Close the socket
    close(clientSocket);
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