/*

FTPduino.h - Library for FTP communication with Arduino.

An easy to use FTP client library for the Arduino ecosystem.
Uses the WiFiClient library to download files from FTP servers that support Passive mode (PASV). The library stores the received data in a user defined buffer.

*/

#include "Arduino.h"
#include "FTPduino.h"

FTPduino::FTPduino()
{
    // constructor
}

bool FTPduino::connect(const char *serverIP, const int serverPort, const int timeout)
{
    // Open a connection to the FTP server
    if (!client.connect(serverIP, serverPort))
    {
        return false;
    }
    while (!client.available())
        ;

    return true;
}

bool FTPduino::authenticate(const char *FTPuser, const char *FTPpass)
{
    // Clear the client's buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Give username to FTP server
    client.println("USER " + (String)FTPuser);
    while (!client.available())
        ;

    // Give password to FTP server
    client.println("PASS " + (String)FTPpass);
    while (!client.available())
        ;

    return true;
}

bool FTPduino::setWorkDirectory(const char *workDirectory)
{
    // Clear the client's buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Change working directory
    client.println("CWD " + (String)workDirectory);
    while (!client.available())
        ;

    return true;
}

size_t FTPduino::getFileSize(const char *fileName)
{
    // Clear the client's buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Request file size
    client.println("SIZE " + (String)fileName);
    while (!client.available())
        ;

    // Read messages from server until response code 213 is received
    while (!response.startsWith("213"))
    {
        response = client.readStringUntil('\n');
    }

    return response.substring(4).toInt();
}

bool FTPduino::downloadFile(const char *fileName, uint8_t *fileBuffer, size_t bufferSize)
{
    // Clear the client's buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Set file transfer type to binary data ('I' for "Image")
    client.println("TYPE I");
    while (!client.available())
        ;

    // Enter passive mode (PASV)
    client.println("PASV");
    while (!client.available())
        ;

    // Clear the client's input buffer before reading the new response
    while (client.available())
    {
        client.read();
    }

    // Parse the passive mode response
    response = client.readStringUntil('\n');
    Serial.println("PASV response: " + response);
    if (sscanf(response.c_str(), "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &portHigh, &portLow) != 6)
    {
        Serial.println("Failed to parse passive mode response: " + response);
        return false;
    }

    // Assemble IP address from fragments received from PASV command
    dataIp = String(ip1) + "." + String(ip2) + "." + String(ip3) + "." + String(ip4);

    // Calculate Port number from data received from PASV command
    dataPort = (portHigh * 256) + portLow;

    // Create new client dedicated to data transmission and connect to it
    Serial.println("Data client IP:" + dataIp + ":" + (String)dataPort);
    if (!Dclient.connect(dataIp.c_str(), dataPort))
    {
        Serial.println("Failed to connect to data port");
        return false;
    }

    // Request the file from server
    client.println("RETR " + (String)fileName);
    while (!client.available())
        ;

    // Wait for the "150" (Starting file transfer) response code
    while (client.available() && !response.startsWith("150"))
    {
        response = client.readStringUntil('\r');
    }

    // Read data until buffer gets all the data
    bytesRead = Dclient.readBytes(fileBuffer, bufferSize);

    // Wait for the "226" (Transfer complete) response code
    while (client.available() && !response.startsWith("226"))
    {
        response = client.readStringUntil('\r');
    }

    return true;
}

void FTPduino::disconnect()
{
    // Close the client connections
    client.stop();
    Dclient.stop();
}
