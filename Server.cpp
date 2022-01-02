
#include "Server.h"

int Server::configureCommunication() {
    // create socket
    this->mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket == -1)
    {
        srvmsg( "Can't create a socket!" );
        return -1;
    }
    srvmsg("my socket # is " + to_string(this->mySocket));
    // bind socket to ip/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(this->myPort);
    srvmsg( "my port is " + std::to_string( hint.sin_port) );
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    int binding = bind(mySocket, (sockaddr*)&hint, sizeof(hint));
    if (binding < 0) {
        srvmsg( "Can't bind a socket! (" + std::to_string( binding ) + ")" );
        return -2;
    }
    // telling winsock
    int listening = listen(mySocket, SOMAXCONN);
    if (listening < 0) {
        srvmsg( "Can't listen! (" + std::to_string( listening ) + ")" );
        return -3;
    }
    /*
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];*/
    return 0;
}

bool Server::handleClient(int clientSocket, char* buf, int byteRcv){
    string clientInput = std::string (buf, 0, byteRcv);
    srvmsg("CLIENT:_" + std::string (buf, 0, byteRcv) + "_");
    if (clientInput == EXIT_WORD) {
        srvmsg("finished communication with " + to_string(clientSocket));
        char okBye[] = "ok bye\n";
        send(clientSocket, okBye, sizeof(okBye), 0);
        return false;
    }
    else {
        char gotYou[] = "got you fam\n";
        send(clientSocket, gotYou, sizeof(gotYou), 0);
    }
    return true;
}

void Server::listenToClient() {
    while(!this->startCommunication);
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(this->mySocket, (sockaddr*)&client, &clientSize);
    if (clientSocket < 0) {
        srvmsg( "Can't accept! (" + std::to_string(clientSocket) + ")" );
        return;
    }
    int bufSize = 4096;
    char buf[bufSize];
    srvmsg( "accepted client " + to_string(clientSocket));
    std::string clientInput = "";
    bool cont = true;
    while (cont){
        memset(buf, 0, bufSize);
        int byteRcv = recv(clientSocket, buf, bufSize, 0);
        if (byteRcv < 0) {
            srvmsg( "connection issue" );
            break;
        }
        if (byteRcv == 0) {
            srvmsg( "client disconnected" );
            break;
        }
        cont = handleClient(clientSocket, buf, byteRcv);
    }
    this->clientsSockets.push_back(clientSocket);
}

void Server::start(ClientHandler& ch)throw(const char*){
    if (this->mySocket >= 0) {
        this->startCommunication = true;
        srvmsg("thread allowed to start communication");
    }
}

void Server::stop(){
    this->myThread->join();
    close(this->mySocket);
    for (int sock = 0; sock < this->clientsSockets.size(); ++sock) {
        close(this->clientsSockets.at(sock));
    }
    srvmsg("stopped and cleared everything");
}

Server::~Server() {
}

