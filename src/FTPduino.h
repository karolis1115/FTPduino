/*

FTPduino.h - Library for FTP communication with Arduino.
Created by Karolis Juozapaitis

An easy to use FTP client library for the Arduino ecosystem.
Uses the WiFiClient library to download files from FTP servers that support Pasive mode (PASV). The library stores the received data in a user defined buffer.

*/
#ifndef FTPduino_h
#define FTPduino_h

#include "Arduino.h"
#include "WiFiClient.h"

#define availCheckDelay 500

class FTPduino
{
public:
    FTPduino();

    bool connect(const char *serverIP, const int serverPort, const int timeout = 10000);
    bool authenticate(const char *FTPuser, const char *FTPpass);

    bool setWorkDirectory(const char *workDirectory);
    size_t getFileSize(const char *fileName);
    bool downloadFile(const char *fileName, uint8_t *fileBuffer, size_t bufferSize);
    void disconnect();

private:
    WiFiClient client;
    WiFiClient Dclient;
};

#endif
