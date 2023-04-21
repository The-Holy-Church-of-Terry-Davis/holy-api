#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <regex>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "../json.hpp"
#include "../logger/logger.h"

using namespace std;
using json = nlohmann::json;

Logger logger("server.log", LogLevel::INFO);

string askTerry() {
    const string answers[] = {
        "I honestly don't see that fucking possible with those fucking CIA guys...",  // no
        "I have divine intellect, you have divine intellect. Go for it!",  // yes
        "I asked God, and he said he isnt sure. So I'm not sure.",  // i dont know/maybe
        "Sorry, I got distraced by those pesky glow in-the-dark CIA monkeys, ask me again..."  // ask again
    };

    // Get the size of the answers array
    int numAnswers = sizeof(answers) / sizeof(answers[0]);

    // Generate a random index between 0 and numAnswers - 1
    int randomIndex = rand() % numAnswers;

    return answers[randomIndex];
}

string getQuote(int amount = 1) {
    const string quotes[] = {
        "It's ok to run-over space aliens",
        "THAT DOG IS GETTING RAPED",
        "The most important thing in programming is naming.",
        "The most powerful statement in the C language is the switch statement.",
        "In TempleOS, my compiler will put together multiple characters in a character constant. We don't do Unicode. We do ASCII--8-bit ASCII, not 7-bit ASCII; 7-bit signed ASCII is retarded.",
        "The IRA is like the NRA, but for computers. The CIA wants all code in the cloud under their lock and key. They want to ban compilers and make people think HTML is computer programming.",
        "Sir, I can do you a nice SEO.",
        "It's OK to run-over space aliens",
        "An idiot admirers complexity, a genius admires simplicity",
        "There's nothing better to do with your time than kill time with Mr God and enjoy divine intellect all day long.",
        "Maybe I'm just a bizarre little person who walks back and forth",
        "God is all around us, it is our job to seek him out.",
        "This is fucking ridiculous, I have a godamn space alien",
        "I HAVE A FUCKING SPACE ALIEN",
        "I wrote my own fucking compiler",
        "I am the best fucking programmer that has ever lived because I have devine intellect",
        "God likes elephants, and I like elephants.",
        "God's gonna fuck your shit up N...",
        "The first time you meet an angel you get a horrible beating",
        "When my bird was looking at my computer monitor, I thought 'woah, that bird has no idea what he's looking at'. And yet, what does the bird do, does he panic? No, he can't really panic, he just does the best he can.",
        "Hey God?  Could a team of monkeys beat humans, if You trained them?  How about orangutangs?",
        "Thermostats are the number one possible fuel saving thing.  Wear cloth indoors in winter, take-off cloths in summer.  Carter did sweaters.",
        "God said we have lost our ancestral lands -- the C64 Apple II generation. Bill Gates has moved programmers onto a reservation.  Assembly language is a forbiden language.  They are welding-shut car hoods so you cannot access your engine.",
        "Download God's Temple Operating System and study it to see divine intellect at work.  It was designed by God, just like Noah's ark and Solomon's Temple and the Bible.",
        "If a file is too big to be loaded all at once into memory, use Windows.",
        "Let me explain this to you.  It is God's temple.  Everything I say can be confirmed by God.",
        "Talk to God, then you will know better than being stupid.",
        "I keep it simple, on purpose.",
        "This really angered me in 1983",
        "It is approximately the case that I want to BAN multimedia from TempleOS.",
        "The fuck is your problem?  I wrote a fucking 20,000 HolyC x86_64 compiler.",
        "Do you know what a compiler is?  I know what a compiler is.  I took a course on compilers.",
        "Why is my internet just a handful of retard FBI n*****s who I cannot shoot in the head enough times to cure their stupidity!",
        "I know God.  I can identify Him by what He says.",
        "I can flip a coin for random numbers.",
        "I am not doing UEFI.",
        "The purpose of life is to know God.",
        "I am not doing PCI devices.",
        "God said 640x480 16 color.",
        "This is what I do.",
        "I decided to make my code pleasant.  I don't like multiple architecture projects -- code is ugly.",
        "I don't like Unicode.  I might bail if it goes to Unicode.",
        "You guys are FBI or NSA or CIA.",
        "You wanna place ourself in a position to give maximum glory to your Creator. So you go out there, and you be somebody glorious, okay?"
    };

    // Get the number of quotes in the array
    const int size = sizeof(quotes) / sizeof(quotes[0]);

    // Set up random number generation
    srand((unsigned) time(0));

    // Initialize an empty vector to hold the quotes
    vector<string> quoteList;

    // Generate random quotes for the given amount
    for (int i = 0; i < amount; i++) {
        int index = rand() % size;
        string quote = quotes[index];
        quoteList.push_back(quote);
    }

    // Create a JSON object with an array of quotes under the "quotes" key
    json finalJson;
    finalJson["quotes"] = quoteList;

    // Return the JSON string
    return finalJson.dump();
}

string getGodsay(int amount = 1) {
    vector<string> words;

    // Open the file and read the contents into a string
    ifstream file("www/vocab.txt");
    stringstream buffer;
    buffer << file.rdbuf();
    string contents = buffer.str();

    // Split the string into words, using whitespace as the delimiter
    istringstream iss(contents);
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(words));

    // Shuffle the vector of words
    random_device rd;
    mt19937 g(rd());
    shuffle(words.begin(), words.end(), g);

    // Get the first `amount` words and join them with a space separator
    string godsay = accumulate(words.begin(), words.begin() + min(amount, (int)words.size()), string(""), [](const string& a, const string& b) { return a + " " + b; });

    // Remove the leading and trailing whitespace from the string
    godsay = regex_replace(godsay, regex("^\\s+|\\s+$"), "");

    return godsay;
}

string handleRootEndpoint() {
    string response;
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

    // Read the file contents
    ifstream file("www/index.html");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            response += line + "\n";
        }
        file.close();

        logger.info("Successfully served index.html");
    } else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        response += "<h1>404 Not Found</h1>\n";
        response += "<p>The requested URL was not found on this server.</p>\n";

        logger.warning("Failed to serve index.html");
    }

    return response;
}

string handleAboutEndpoint() {
    // Create a JSON object and add some data to it
    json data;
    data["person"]["name"] = "John Doe";
    data["person"]["age"] = 42;
    data["person"]["email"] = "john.doe@example.com";

    // Convert the JSON object to a string
    string jsonString = data.dump();

    // Construct the HTTP response
    string response;
    response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += jsonString;

    return response;
}

string handleGodsayEndpoint(const string& query) {
    json data;

    // Parse the query string to extract the `amount` parameter value
    int amount = 1;
    size_t pos = query.find("amount=");
    if (pos != string::npos) {
        string amount_str = query.substr(pos + 7);
        amount = stoi(amount_str);
    }

    string godsay = getGodsay(amount);
    data["godsay"] = godsay;

    string jsonString = data.dump();
    string response;
    response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += jsonString;

    return response;
}

string handleQuoteEndpoint(const string& query) {
    json data;

    // Parse the query string to extract the `amount` parameter value
    int amount = 1;
    size_t pos = query.find("amount=");
    if (pos != string::npos) {
        string amount_str = query.substr(pos + 7);
        amount = stoi(amount_str);
    }

    string quote = getQuote(amount);

    string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += quote;

    return response;
}

string handleDivineIntellectEndpoint() {
    json data;

    // Set the seed value for the random number generator
    srand((unsigned) time(0));

    // Generate a random number between 0 and 100
    int number = rand() % 101;
    int decimal = rand() % 100;

    if (number == 100) {
        decimal = 0;
    }

    string percentage = to_string(number) + "." + to_string(decimal) + "%";
    data["divineIntellect"] = percentage;
    data["message"] = "You are " + percentage + " divine intellect!";

    string jsonString = data.dump();

    string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += jsonString;
    return response;
}

string handleStatusEndpoint() {
    json data;

    data["message"] = "Hello, from The Holy Church of Terry Davis! (discord.gg/xEFtNzEzE8)";
    data["status"] = "OK";

    string jsonString = data.dump();

    string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += jsonString;

    return response;
}

string handleInfoEndpoint() {
    json data;

    json links;
    links["discord"] = "discord.gg/xEFtNzEzE8";
    links["repository"] = "https://github.com/The-Holy-Church-of-Terry-Davis/holy-api";
    links["gh_organization"] = "https://github.com/The-Holy-Church-of-Terry-Davis";
    links["our_website"] = "https://the-holy-church-of-terry-davis.github.io";
    data["links"] = links;

    data["infoMessage"] = "Hello! This API/Webserver was made using C++! I was bored, I don't even know how to use C++.";

    string jsonString = data.dump();
    string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += jsonString;

    return response;
}

string handleAskTerryEndpoint() {
    json data;
    data["answer"] = askTerry();

    string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    response += data.dump();

    // Return the JSON string
    return response;
}

string handleStaticFile(const string& filePath, const string& contentType) {
    string response;

    // Read the file contents
    ifstream file(filePath);
    if (file.is_open()) {
        string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n\r\n";
        response += contents;

        logger.info("Successfully served " + filePath);
    } else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        response += "<h1>404 Not Found</h1>\n";
        response += "<p>The requested URL was not found on this server.</p>\n";

        logger.warning("Failed to serve " + filePath);
    }

    return response;
}

void handleRequest(int clientSocket, const string& request) {
    string response;

    if (request.find("GET / ") != string::npos || request.find("GET /?") != string::npos) {
        response = handleRootEndpoint();
    }
    else if (request.find("GET /about") != string::npos) {
        response = handleAboutEndpoint();
    }
    else if (request.find("GET /vocab") != string::npos) {
        response = handleStaticFile("www/vocab.txt", "text/plain");
    }
    else if (request.find("GET /style.css HTTP/1.1\r\n") != string::npos) {
        response = handleStaticFile("www/style.css", "text/css");
    }
    else if (request.find("GET /favicon.ico HTTP/1.1\r\n") != string::npos) {
        response = handleStaticFile("www/favicon.ico", "image/x-icon");
    }
    else if (request.find("GET /Terry.png HTTP/1.1\r\n") != string::npos) {
        response = handleStaticFile("www/Terry.png", "image/x-png");
    }
    else if (request.find("GET /godsay") != string::npos) {
        size_t pos = request.find("?");
        if (pos != string::npos) {
            string query = request.substr(pos + 1);
            response = handleGodsayEndpoint(query);
        }
        else {
            response = handleGodsayEndpoint("");
        }
    }
    else if (request.find("GET /quote") != string::npos) {
        size_t pos = request.find("?");
        if (pos != string::npos) {
            string query = request.substr(pos + 1);
            response = handleQuoteEndpoint(query);
        }
        else {
            response = handleQuoteEndpoint("");
        }
    }
    else if (request.find("GET /divineintellect") != string::npos) {
        response = handleDivineIntellectEndpoint();
    }
    else if (request.find("GET /status") != string::npos) {
        response = handleStatusEndpoint();
    }
    else if (request.find("GET /info") != string::npos) {
        response = handleInfoEndpoint();
    }
    else if (request.find("GET /askterry") != string::npos) {
        response = handleAskTerryEndpoint();
    }
    else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        response += "<h1>404 Not Found</h1>\n";
        response += "<p>The requested URL was not found on this server.</p>\n";
        response += "\n<h3>Debug Info:\n\n</h3><p>" + request + "</p>\n";
        logger.warning("Invalid request received");
    }

    send(clientSocket, response.c_str(), response.size(), 0);
}

void startServer() {
    // Create the server socket
    logger.info("Creating server socket");
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket() failed");
        return;
    }

    logger.info("Binding socket to port 8080");
    // Bind the server socket to port 8080
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind() failed");
        return;
    }

    logger.info("Listening for incoming connections");
    // Listen for incoming client connections
    if (listen(serverSocket, 10) < 0) {
        perror("listen() failed");
        return;
    }

    logger.info("Server started, http://localhost:8080");

    // Accept incoming client connections
    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("accept() failed");
            continue;
        }

        // Read the HTTP request from the client
        const int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived < 0) {
            perror("recv() failed");
            close(clientSocket);
            continue;
        }

        // Parse the request from the client
        string request(buffer, buffer + bytesReceived);

        // Handle the request
        handleRequest(clientSocket, request);

        // Close the client socket
        close(clientSocket);
    }

    // Close server socket
    close(serverSocket);
}
