#include <FTPduino.h>
#include <WiFi.h>



FTPduino ftp;

//Add your WiFi details here
static char const SSID[] = "YOUR_SSID";
static char const PASS[] = "YOUR_PASS";


// Add the details of the FTP server you are trying to connect to
static char const serverIP[] = "FTP_SERVER_IP";

static char const FTPuser[] = "FTP_USERNAME";
static char const FTPpass[] = "FTP_PASSWORD";



static char const workDirr[] = "/";  //dirrectory in which desired file resides in

static char const fileName[] = "HelloWorld.txt";  //Name of desired file


void setup() {
  Serial.begin(115200);

  while (!Serial)
    ;

  //Attempt WiFi connection
  if (!ConnectWiFi(SSID, PASS)) {
    Serial.println("WiFi Connection Failed!");
    while (true)
      ;
  }
  //if connections successful, print ip address of device
  Serial.println("WiFi connected");
  Serial.println("Local IP address: " + WiFi.localIP());

  //Attempt FTP server connection
  if (!ftp.connect(serverIP, 21)) {
    Serial.println("conn failed");
  }
  Serial.println("Connection successful");

  ftp.authenticate(FTPuser, FTPpass);
  Serial.println("Authentication successful");

  ftp.setWorkDirectory(workDirr);



  size_t fileSize = ftp.getFileSize(fileName);

  uint8_t fileBuffer[fileSize];

  ftp.downloadFile(fileName, fileBuffer, fileSize);

  Serial.write(fileBuffer, fileSize);

  ftp.disconnect();
}

void loop() {
  // put your main code here, to run repeatedly:
}






bool ConnectWiFi(const char* SSID, const char* PASS) {

  Serial.print("Connecting to ");
  Serial.println(SSID);

  if (!WiFi.begin(SSID, PASS))
    return false;

  while (WiFi.status() != WL_CONNECTED) {
    //do nothin
  }
  return true;
}
