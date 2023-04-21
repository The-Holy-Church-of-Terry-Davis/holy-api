/*
#include "server/server.h"

using namespace std;

int main() {
    Server server(8080);
    server.start();
    return 0;
}
*/

#include <iostream>

using namespace std;

// Declare the function in server.cpp
void startServer();

int main() {
    cout << "Starting server..." << endl;
    startServer();
    cout << "Server stopped." << endl;

    return 0;
}