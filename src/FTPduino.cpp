/*

FTPduino.h - Library for FTP communication with Arduino.
Created by Karolis Juozapaitis

An easy to use FTP client library for the Arduino ecosystem.
Uses the WiFiClient library to download files from FTP servers that support Pasive mode (PASV). The library stores the received data in a user defined buffer.   

*/


#include "Arduino.h"
#include "FTPduino.h"


FTPduino::FTPduino()
{
    // constructor
}

bool FTPduino::connect(const char *serverIP, const int serverPort, const int timeout)
{
    // OPEN FTP CONNECTION********************************************
    if (!client.connect(serverIP, serverPort))
    {
        return false;
    }
    while (!client.available())
    {
        delay(availCheckDelay);
    }
    return true;
}

bool FTPduino::authenticate(const char *FTPuser, const char *FTPpass)
{
    // Clear the client's input buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // LOGIN USER******************************************************
    client.println("USER " + (String)FTPuser);
    while (!client.available())
    {
        delay(availCheckDelay);
    }

    // LOGIN PASSWORD*******************************************************
    client.println("PASS " + (String)FTPpass);
    while (!client.available())
    {
        delay(availCheckDelay);
    }
    return true;
}

bool FTPduino::setWorkDirectory(const char *workDirectory)
{
    // Clear the client's input buffer before reading the new response
    while (client.available())
    {
        client.read();
    }
    // CHANGE WORKING DIRECTORY****************************************
    client.println("CWD " + (String)workDirectory);
    while (!client.available())
    {
        delay(availCheckDelay);
    }
    return true;
}

size_t FTPduino::getFileSize(const char *fileName)
{
    // Clear the client's data buffer before continuing
    while (client.available())
    {
        client.read();
    }

    String response = "";
    // SIZE******************************************
    client.println("SIZE " + (String)fileName);
    while (!client.available())
    {
        delay(availCheckDelay);
    }

    // Read messages from server until response code 213 is received
    while (!response.startsWith("213 "))
    {
        response = client.readStringUntil('\n');
    }

    return response.substring(4).toInt();
}

bool FTPduino::downloadFile(const char *fileName, uint8_t *fileBuffer, size_t bufferSize)
{
    // Clear the client's input buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Set binary transfer type (type 'I' for B(i)nary)
    client.println("TYPE I");
    while (!client.available())
    {
        delay(availCheckDelay);
    }

    // Enter passive mode (PASV)
    client.println("PASV");
    while (!client.available())
    {
        delay(availCheckDelay);
    }

    // Clear the client's input buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Parse the passive mode response
    String response = client.readStringUntil('\n');
    Serial.println("PASV response: " + response);
    int ip1, ip2, ip3, ip4, portHigh, portLow;
    if (sscanf(response.c_str(), "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &portHigh, &portLow) != 6)
    {
        Serial.println("Failed to parse passive mode response: " + response);
        return false;
    }

    // Assemble ip address from fragments received from PASV command
    String dataIp = String(ip1) + "." + String(ip2) + "." + String(ip3) + "." + String(ip4);
    // Calculate Port number from data received from PASV command
    // int dataPort = (portHigh*256)+portLow

    int dataPort = (portHigh << 8) + portLow;

    // Create new client dedicated for data transmission
    Serial.println("Data client IP:" + dataIp + ":" + (String)dataPort);
    if (!Dclient.connect(dataIp.c_str(), dataPort))
    {
        Serial.println("Failed to connect to data port");
        return false;
    }

    // Send RETR
    client.println("RETR " + (String)fileName);
    while (!client.available())
    {
        delay(availCheckDelay);
    }

    // Wait for the "150 Opening BINARY mode data connection" response (starting file transfer)
    while (client.available())
    {
        String response = client.readStringUntil('\r');
        if (response.startsWith("150"))
        {
            break;
        }
    }

    // Read data until the connection is closed or buffer is full
    size_t bytesRead = Dclient.readBytes(fileBuffer, bufferSize);
    fileBuffer[bytesRead] = '\0'; // Null-terminate the buffer

    // Wait for the "226 Transfer complete" response
    while (client.available())
    {
        String response = client.readStringUntil('\r');
        if (response.startsWith("226"))
        {
            Serial.println("Transfer complete");
            break;
        }
    }

    return true;
}

void FTPduino::disconnect()
{
    client.stop();
    Dclient.stop();
}
