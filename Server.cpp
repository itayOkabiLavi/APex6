
#include "Server.h"
const int SOCK_ERR = -1, BIND_ERR = -2, CONNECTION_ERR = -3;
/**
 * Notice:   "serv_addr.sin_addr.s_addr = INADDR_ANY;" may cause pproblems
 */
Server::Server(int port)throw (const char*)  : myPort(port) {

}

void serverMessages(string msg) {
    cout << "-> Server: " << msg << endl;
}

void startListening(ClientHandler &ch, int myPort) {
    int mySocket, clientID;
    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket < 0) {
        serverMessages("error creating socket");
        return;
    }

    struct hostent *server = gethostbyname("localhost");
    struct sockaddr_in serv_addr;
    //Binding
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = myPort;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(mySocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        serverMessages("error binding");
        return;
    }
    //Listening
    listen(mySocket, 15);
    //Timeout
    int timeout = 3;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(mySocket, &readfds);
    if (select(timeout, &readfds, NULL, NULL, &tv) > 0) {
        //Accepting clients
        while (true) {
            clientID = accept(mySocket, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(serv_addr));
            if (clientID < 0) {
                serverMessages("accepting failed");
                return;
            } else {
                ch.handle(clientID);
            }
        }
    } else {
        serverMessages("select() error");
    }
}

void Server::start(ClientHandler& ch)throw(const char*){
    serverMessages("started");
    this->myThread = new thread(startListening, ref(ch), this->myPort);
}

void Server::stop(){
	this->myThread->join(); // do not delete this!

}

Server::~Server() {
}