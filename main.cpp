#include "server/server.h"

using namespace std;

int main() {
    Server server(8080);
    server.start();
    return 0;
}
