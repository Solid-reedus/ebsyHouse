
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "WiFiEsp.h"
#include <dht.h>;
dht DHT;

WiFiEspClient client; 

// variables for user functions
long timer = 0;

// WiFi Stuff

char ssid[] = "Arduino";
char pass[] = "Zp3Y8FKt";
int status = WL_IDLE_STATUS;   

// your server address
char server[] = "85307.hosting.ict-college.net";   
char postPage[] = "/projects/ebsyHouse/post.php";

/// vars used for timing/timeout
unsigned long lastConnectionTime = 0;
unsigned long postingInterval = 5000;

bool readSenors = true;
String content;
 
#define btnPin 3
#define redLed 2
#define DHT11_PIN 4

double temp;
double hum;
int light;
int tempCap = 50;

byte oldPinState = HIGH;

LiquidCrystal_I2C lcd(0x27,16,2);
// the setup function runs once when you press reset or power the board

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200); 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(redLed, OUTPUT);
  lcd.init();
  lcd.backlight();
  SetupWifi();
}

// the loop function runs over and over again forever
void loop() 
{
  byte pinState = digitalRead(btnPin);
  byte ledState = digitalRead(redLed);

/*
  if (pinState != oldPinState)
  {
    delay (10);  // debounce 
    if (pinState == LOW)
    {
      digitalWrite (redLed, !digitalRead (redLed));
    }
    oldPinState = pinState;
  }
  */

  if(readSenors == true)
  {
    digitalWrite(redLed, HIGH); 
  }
  else
  {
    digitalWrite(redLed, LOW);    
  }

  if(readSenors == true)
  {
    temp = DHT.temperature;
    hum = DHT.humidity;
    light = 1;
    MakeMessege();

    int chk = DHT.read11(DHT11_PIN);
    lcd.display();
    lcd.backlight();
    lcd.setCursor(0,0);
    
    lcd.print("Humidity ");
    lcd.print(DHT.humidity, 1);
    lcd.print("%");

    lcd.setCursor (0, 1);
    lcd.print ("Temp: ");
    lcd.print(DHT.temperature, 1);
    lcd.print(" C");
    
  }

  handleHttpPost();
  handleHttpResponce();
}


void handleHttpResponce()
{
  String readString; 
  // receive incommin wifi data
  while (client.available()) {
    char c = client.read();
    readString += c;
    //Serial.print(c); // debug only
  }
   if (readString.length() >0) 
   {
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

void parseString(String input)
{
  /*

double temp;
double hum;
int light;
int tempCap = 50;

  */

  int start = 0;
  int end = input.indexOf("#");

  while (end != -1) 
  {
    String substring = input.substring(start, end);
    int starIndex = substring.indexOf("*");
    String varName = substring.substring(0, starIndex);
    String varValue = substring.substring(starIndex + 1);

    if(varName == "temp")
    {
      temp = varValue.toInt();
    }
    else if(varName == "hum")
    {
      hum = varValue.toInt();
    }
    else if(varName == "light")
    {
      light = varValue.toInt();
    }


    start = end + 1;
    end = input.indexOf("#", start);
  }
}

void MakeMessege()
{
  String tmpString = "temperature=";
  String humString = "&humidity=";
  String lightString = "&light=";

  content = tmpString + temp + humString + hum + lightString + light;

  //rfid code
  //content += rfidCode()
}




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
      client.stop();
      httpPostRequest();
    
      // print the time it took to post the data
      Serial.println("duration : "+ String(millis()-timer));
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
  void SetupWifi()
  {

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




