
#define SFML_STATIC
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <thread>

using namespace std;








int main2(){


    char connectionType;
    cout << "DIGITE (s) PARA SERVER OU (c) PARA CLIENTE" << endl;
    cin >> connectionType;



   
    if(connectionType == 's'){

        sf::IpAddress ip = sf::IpAddress::getLocalAddress();

        sf::TcpListener listener;
        listener.setBlocking(false);


        if(listener.listen(2000) != sf::Socket::Done){
            cout << "Unable to initialize server on port 2000" << endl;
            system("pause");
            return 0;
        } else {
            cout << "Server on Local Address : " << ip.toString() << ":" << listener.getLocalPort() << endl;
        }

        sf::TcpSocket socket;

        while (listener.accept(socket) != sf::Socket::Done){

        }

        socket.setBlocking(false);


        int frames = 0;
        while(1){
                frames ++;


                system("pause");

             // Sending Answer
                sf::Packet sendPacket;
                sf::Packet extra;

                for(int i = 0; i < frames; i++){
                    sendPacket << i;
                    cout <<  i << " ";
                }

                for( int i = -frames; i < 0; i++){
                    extra << i;
                    cout << i << " ";
                }

                cout << " Sended" << endl;

                sendPacket.append(extra.getData(), extra.getDataSize());



                sf::Socket::Status sendStatus = socket.send(sendPacket);

                while(sendStatus == sf::Socket::Partial){
                    cout << "Failed: Sent to server" << endl;
                }

        }


    } else {

        sf::TcpSocket socket;

        socket.setBlocking(true);

        bool loop = true;

        cout << "Please type the Server Ip Address  Ex.: 195.132.0.115" << endl;

        string ipString;

        cin >> ipString;

        cout << "Please type the Server Port  Ex.: 2000" << endl;

        int portInt;

        cin >> portInt;

        sf::IpAddress ip(ipString);

        cout << "Trying to connect to Server: " << ipString << ":" << portInt << endl;

        int timesTried = 0;

        while(loop && timesTried < 3){
            //system("pause");
            timesTried++;
            sf::Socket::Status status = socket.connect(ip,portInt);
            //sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(),2000);
            if(status == sf::Socket::Done){
                cout << "Connected to Server" << endl;
                loop = false;
            } else{
                cout << "Connection Timeout :";
                if(status == sf::Socket::Error){
                    cout << "error";
                } else if(status == sf::Socket::NotReady){
                    cout << "notReady";
                }
                cout << endl;
            }
        }

        loop = true;

        socket.setBlocking(false);


        while(1){
            sf::Packet receivePacket;

            sf::Socket::Status statusReceive = socket.receive(receivePacket);
            while(statusReceive == sf::Socket::Partial){
                cout << "Partial Packet received trying again" << endl;
                statusReceive = socket.receive(receivePacket);
            }

            if(statusReceive == sf::Socket::Done){

                //printPacketInstructions(receivePacket, allGameObjects);


                while(receivePacket && !receivePacket.endOfPacket()){
                    int number;
                    receivePacket >> number;
                    cout << number << " ";

                }
                cout << " Received" << endl;
            }
            system("pause");
        }

    }

}
