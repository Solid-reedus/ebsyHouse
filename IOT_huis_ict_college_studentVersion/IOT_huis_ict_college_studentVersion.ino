

/*==============================================================================
|* Program File Name          IOT_HOME_ICTCOLLIGE.c
|* Program Description        Web client for IOT HOME connected to ICT college
|*
|*  version   | Description                                 | date    | author 
|*------------------------------------------------------------------------------
|* v0.01        Initial version                              23-feb-23  GEST
|* v0.02        added lcd, rfid, dth sensor, leds            26-feb-23  GEST
|* v0.03        HTTP post now recieves data back             01-mrt-23  GEST
|*              added readSenors switch, made the code 
|*              student friendlier. moved all the HTTP 
|*              stuff to "FUNCTIONS -- DO NOT CHANGE!!"
|*==============================================================================
*/

/*------------------------------------------------------------------------------
|* Notes
|*------------------------------------------------------------------------------
|* Build on - WiFiEsp example: WebClientRepeating (http://yaab-arduino.blogspot.com/p/wifiesp.html)
|* Used HardwareSerial on pins 18&19 and Emulate Serial1 on pins 6/7 if not present

|* IMPORTANT: receiving data from the post request takes some time and you can not use a delay while waiting on data!
|* the DTH and LCD libraries use delay so make them only run when not receiving data.

|*
*/


/*------------------------------------------------------------------------------
|* INCLUDES
|*------------------------------------------------------------------------------
*/
#include "WiFiEsp.h"
/*------------------------------------------------------------------------------
|* DEFINES
|*------------------------------------------------------------------------------
*/

#define DO_STATUSLED 5

/*------------------------------------------------------------------------------
|* Classes
|*------------------------------------------------------------------------------
*/
WiFiEspClient client;   

/*------------------------------------------------------------------------------
|* VARIABLES
|*------------------------------------------------------------------------------
*/

// variables for user functions
long timer = 0;

// WiFi Stuff

char ssid[] = "Arduino";
char pass[] = "Zp3Y8FKt";
int status = WL_IDLE_STATUS;   

// your server address
//https://85307.hosting.ict-college.net/projects/ebsyHouse/index.php
//char server[] = "http://85307.hosting.ict-college.net";   
char server[] = "85307.hosting.ict-college.net";   
char postPage[] = "/projects/ebsyHouse/index.php";

/// vars used for timing/timeout
unsigned long lastConnectionTime = 0;
unsigned long postingInterval = 5000;

bool readSenors = true;
String content;



/*------------------------------------------------------------------------------
|*  FUNCTIONS
|*------------------------------------------------------------------------------
*/
/* function     : setup(void)
|* 
|* Description  : setup IO, Serial and WiFi
|*
|* In/Out       : nothing
*/ 
void setup()
{
  // setup IO pins
  pinMode(DO_STATUSLED, OUTPUT);
  
  /// serial Monitor
  Serial.begin(115200); 
  
  // initialize the lcd 
  // initialize sensors
  
  /// setup WiFi
  SetupWifi();
  
}


/* function     : loop(void)
|* 
|* Description  : endless loop. get sensors, set actuators, receive and send wifi data 
|*
|* In/Out       : nothing
*/ 
void loop()
{
  // only read and set sensors and actuators here while readSensors == true!
  if(readSenors == true){
    // get button(s)
    // get temp en humidity
    // get rfid
    // print to the lcd
    // gest sensors
    // generate string to be sent with the post action
    // content = "....."

  }
 
  // handel the http stuff
  handleHttpPost();
  handleHttpResponce();
  
  
}


/* function     : handleHttpResponce()
|* 
|* Description  : Receive data from a server after a postRequist. 
|*                the data is processed and used to set the actuators.
|*
|* In/Out       : in  : String to search trough
|*                in  : search word and end char
|*                out : value received from server
*/ 
void handleHttpResponce()
{
  String readString; 
  // receive incommin wifi data
  while (client.available()) {
    char c = client.read();
    readString += c;
    //Serial.print(c); // debug only
  }
   if (readString.length() >0) {
   // Serial.println(readString); // debug only

    /* insert your processString function to get the data form the serever
        e.g:  int dataReceived = processString(readString, start, end);
    */

    client.stop();           // stop http client
    readSenors = true;       // sensors can be read again
  } 
}


/* function     : (String strInput, String keyStart, String keyEnd)
|* 
|* Description  : Seach trough a string to find the value after keyStart and before keyEnd
|*                e.g. strInput = LED01!DOOR04!RFID93283959235!  KeyStart = DOOR; KeyEnd = !
|*                returns 04
|*
|* In/Out       : in  : String to search trough
|*                in  : search word and end char
|*                out : value received from server
*/ 
int processString(String strInput, String keyStart, String keyEnd)
{
 
}


/*------------------------------------------------------------------------------
|*  END OF STUDENT CODE
|*------------------------------------------------------------------------------
*/


#pragma region dontChangeCode

/*------------------------------------------------------------------------------
|*  FUNCTIONS -- DO NOT CHANGE!!
|*------------------------------------------------------------------------------
*/
/* function     : handleHttpPost(String)
|* 
|* Description  : this method sends a post request to the server
|*
|* In/Out       : in : nothing
|*                var: readSenors: disable reading the sensors while sending/receiving
|*                var: timer: keep track ot the time (elapsed)
|*                var: client: WifiClient
|*                var: content: String to be send as post to php page
|*                out: nothing
*/
void handleHttpPost()
{
  
  // if n seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    readSenors = false;
    timer = millis();  
    digitalWrite(DO_STATUSLED, HIGH);
    client.stop();
    httpPostRequest();
   
    // print the time it took to post the data
    Serial.println("duration : "+ String(millis()-timer));
    digitalWrite(DO_STATUSLED, LOW);
  }
 }


/* function     : httpPostRequest(String)
|* 
|* Description  : this method makes a HTTP POST connection to the server
|*
|* In/Out       : in : String to be send as post to php page
|*                var: server: webserver to connect to
|*                var: postPage: page to post the string to
|*                var: client: WifiClient
|*                out: nothing
*/ 
void httpPostRequest()
{
  // connected?
  if (!client.connected()) {
    Serial.println("Disconnecting from server...");
    client.stop();
  }

  // if there's a successful connection
  if (client.connect(server, 80) || client.connected()) {
    Serial.println("Connecting...");

    /// DO NOT CHANGE! -- making the http header
    client.println("POST "+ String(postPage) + " HTTP/1.1");
    client.println("Host: "+ String(server));
    client.println("User-Agent: arduino-ethernet");
    client.println("Accept: */*");
    client.println("Content-Length: " + String(content.length()));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();
    client.print(content);
    /// do not change!
    
    // note the time that the connection was made
    lastConnectionTime = millis();

  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    delay(1000);
  }
}


/* function     : SetupWifi(void)
|* 
|* Description  : initialize ESP module
|*
|* In/Out       : in : nothing
|*                var: ssid: wifi ssid
|*                var: pass: wifi pass
|*                out: nothing
*/ 
void SetupWifi(){

    // serial wifi connection
  Serial1.begin(9600);
  Serial1.println("AT+UART_DEF=115200,8,1,0,0\r\n");
  Serial1.write("AT+RST\r\n");
  delay(100);
  Serial1.begin(115200);
  
   WiFi.init(&Serial1);    

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);    // don't continue
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
}

/* function     : printWifiStatus(void)
|* 
|* Description  : print the wifi status
|*
|* In/Out       : in : nothing
|*                out: nothing
*/ 
void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

#pragma endregion dontChangeCode

