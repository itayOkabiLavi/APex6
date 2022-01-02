/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <thread>

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID){
        cout<< "!!!handling " << clientID << " !!!" << endl;
    }
};

// implement on Server.cpp
class Server {
    thread *myThread;
    bool startCommunication;
    int myPort;
    int mySocket;
    vector<int> clientsSockets = {};
    const string EXIT_WORD = "\n"; // also in mainTrain.cpp
	// you may add data members
    void srvmsg(std::string msg) {
        std::cout << "SRV->" << msg << std::endl;
    }
    void listenToClient();
public:
	Server(int port) throw (const char*) : myPort(port) {
        this->configureCommunication();
        this->startCommunication = false;
        this->myThread = new thread(&Server::listenToClient, this);
    }
	virtual ~Server();
    //void startListening(ClientHandler &ch ,int myPort);
    void start(ClientHandler& ch)throw(const char*);
    int configureCommunication();
    bool handleClient(int clientSocket, char* buf, int byteRcv);
    void stop();
};

#endif /* SERVER_H_ */
