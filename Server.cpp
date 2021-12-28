
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

void startListening(ClientHandler &ch, int myPort, int* dstSocket) {
    serverMessages("started listening");
    int mySocket, clientID;
    mySocket = socket(AF_INET, SOCK_STREAM, 0);

    if (mySocket < 0) {
        serverMessages("error creating socket");
        return;
    } else {
        serverMessages("socket created");
    }
    *dstSocket = mySocket;
    struct hostent *server = gethostbyname("localhost");
    struct sockaddr_in serv_addr;
    //Binding
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = myPort;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(mySocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        serverMessages("error binding");
        close(mySocket);
        return;
    } else {
        serverMessages("binding successful");
    }
    //Listening
    listen(mySocket, 15);
    //Timeout
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(mySocket, &readfds);
    int t = select(mySocket + 1, &readfds, NULL, NULL, &tv);
    if (t == 0) {
        serverMessages("select() error - timeout");
        close(mySocket);
        return;
    } else if (t == -1){
        serverMessages("select() error");
        close(mySocket);
        return;
    } else {
        //Accepting clients
        //while (true) {
            clientID = accept(mySocket, (struct sockaddr *) &serv_addr, (socklen_t *) sizeof(serv_addr));
            if (clientID < 0) {
                serverMessages("accepting failed");
                close(mySocket);
                return;
            } else {
                serverMessages("accepted client " + to_string(clientID) );
                ch.handle(clientID);
            }
        //}
    }
}

string now() {
    time_t now = time(0);
    struct tm  tstruct;
    char nowString[80];
    tstruct = *localtime(&now);
    strftime(nowString, sizeof(nowString), "%X", &tstruct);

    return nowString;
}

void Server::start(ClientHandler& ch)throw(const char*){
    serverMessages("started " + now());
    this->myThread = new thread (startListening, ref(ch) ,this->myPort, &this->mySocket);
    serverMessages("finished " + now());
    //this->myThread[0].join();
}

void Server::stop(){
    this->myThread[0].join();
    close(this->mySocket);
    serverMessages("stopped");
}

Server::~Server() {
}