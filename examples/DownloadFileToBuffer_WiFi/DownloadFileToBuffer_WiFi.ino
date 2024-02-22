/*

FTPduino.h - Library for FTP communication with Arduino.

An easy to use FTP client library for the Arduino ecosystem.
Uses the WiFiClient library to download files from FTP servers that support Passive mode (PASV). The library stores the received data in a user defined buffer.

*/

#include <FTPduino.h>
#include <WiFi.h>

FTPduino ftp;

// Add your WiFi details here
const char SSID[] = "YOUR_SSID";
const char PASS[] = "YOUR_PASS";

// Add the details of the FTP server you are trying to connect to
const char serverIP[] = "FTP_SERVER_IP";
const int FTPport = 21;

const char FTPuser[] = "FTP_USERNAME";
const char FTPpass[] = "FTP_PASSWORD";

// Add the details of the file you are trying to download
const char workDirr[] = "/";              // Directory in which desired file resides in
const char fileName[] = "helloworld.txt"; // Name of desired file

void setup()
{
  Serial.begin(115200);

  // Wait for serial to initialize (Required for microcontrollers such as the Arduino Portenta H7)
  while (!Serial)
    ;

  // Connect to WiFi
  if (!ConnectWiFi(SSID, PASS))
  {
    Serial.println("WiFi Connection Failed!");
    while (true)
      ;
  }
  Serial.println("WiFi connected");

  // Connect to FTP server
  Serial.println("Connecting to FTP server");
  if (!ftp.connect(serverIP, 21))
  {
    Serial.println("Connection failed");
    while (true)
      ;
  }
  Serial.println("Connection successful");

  // Authenticate with FTP server
  if (!ftp.authenticate(FTPuser, FTPpass))
  {
    Serial.println("Authentication failed");
    while (true)
      ;
  }
  Serial.println("Authentication successful");

  // Set work directory
  if (!ftp.setWorkDirectory(workDirr))
  {
    Serial.println("Failed to set work directory");
    while (true)
      ;
  }
  Serial.println("Work directory set");

  // Get file size
  size_t fileSize = ftp.getFileSize(fileName);
  Serial.println("File size: " + String(fileSize) + " Bytes");

  // Create a buffer to store the file
  uint8_t fileBuffer[fileSize];

  // Download file to buffer
  if (!ftp.downloadFile(fileName, fileBuffer, fileSize))
  {
    Serial.println("Failed to download file");
    while (true)
      ;
  }
  Serial.println("File downloaded");

  // Disconnect from FTP server
  ftp.disconnect();
  WiFi.disconnect();
  Serial.println("Disconnected from server and WiFi");
  Serial.println("");

  // Print out the file (buffer) to the serial monitor
  Serial.write(fileBuffer, fileSize);
  Serial.println("");
}

void loop()
{
  // Do nothing
}

// Function to connect to WiFi
bool ConnectWiFi(const char *SSID, const char *PASS)
{

  Serial.print("Connecting to ");
  Serial.println(SSID);

  if (!WiFi.begin(SSID, PASS))
    return false;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  return true;
}
