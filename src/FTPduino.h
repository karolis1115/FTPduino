
/*
Insert comments here


*/
#ifndef FTPduino_h
#define FTPduino_h

#include "Arduino.h"
#include "WiFiClient.h"




#define availCheckDelay  500


class FTPduino
{
public:

    FTPduino();


    bool connect(const char* serverIP, const int serverPort, const int timeout = 10000);
    bool authenticate(const char* FTPuser, const char* FTPpass);

    bool setWorkDirectory(const char* workDirectory);
    size_t getFileSize(const char* fileName);
    bool downloadFile(const char* fileName, uint8_t* fileBuffer, size_t bufferSize);
    void disconnect();
    // read();
    // write();

    // returns:
    // if connection was successful (1 - success, 0 - fail)
    // if authentication was successful (1 - success, 0 - fail)
    // if work directory was set (1 - success, 0 - fail)
    // size of file in bytes
    // if download was successful (1 - success, 0 - fail)
    // void (nothing)

    

private:
    WiFiClient client;
    WiFiClient Dclient;

    // const char[] serverIP;
    // const int serverPort;
    // const int timeout;

    // const char[] FTPuser;
    // const char[] FTPpass;

    // char[] fileBuffer;
    // size_t bufferSize;
    // const char[] workDirectory;
    // const char[] fileName;

};

#endif