#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"
#include <Wire.h>
#include <RFID.h>
#include "WiFiEsp.h"
#include <dht.h>;
#include "Keypad.h"
dht DHT;

long timer = 0;


//region webValues
  WiFiEspClient client; 
  char ssid[] = "Arduino";
  char pass[] = "Zp3Y8FKt";

  int status = WL_IDLE_STATUS;   

  char server[] = "85307.hosting.ict-college.net";   
  char postPage[] = "/projects/ebsyHouse/post.php";

  unsigned long lastConnectionTime = 0;
  unsigned long postingInterval = 5000;

  bool readSenors = true;
  String content;
//endregion

//region keypad values
  const byte ROWS = 4;
  const byte COLS = 4;
  char hexaKeys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  byte rowPins[ROWS] = {33, 35, 37,39}; 
  byte colPins[COLS] = {41, 43, 45, 47};
  Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

  char keyPadSelectIndex = 0;
//endregion

//region SensorValues
  double temp;
  double hum;
  int light;
  int tempCap = 50;
//endregion

//region rfid values
  String rfidValue = "";
  String rfidWeb;
  String HexVal = "#ff00ff";
  RFID rfid(48,49);
//endregion

//region grb led values
  const int redPin = 24;
  const int greenPin = 23;
  const int bluePin = 22; 
//endregion

//region display enums
  enum Display 
  {
    TEMPERATURE  = 0,
    HUMIDITY     = 1,
    RFID_DISPLAY = 2,
    WEB_RFID     = 3,
    EEPROM_FRID  = 4,
    LED_RGB      = 5,
    BUZZER_CAP   = 6
  };
//endregion

#define btnPin 3
#define redLed 2
#define DHT11_PIN 9
SoftwareSerial softserial(A9, A8); // RX, TX

LiquidCrystal_I2C lcd(0x27,16,2);
char lcdInterface[16] = " T H R W E L B ";

void setup() 
{
  Serial.begin(115200); 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(redLed, OUTPUT);
  lcd.init();
  lcd.backlight();
  SPI.begin();
  rfid.init();
    //SetupWifi();
}

void loop() 
{
  if(readSenors == true)
  {
    Displayhouse();
  }
  //handleHttpPost();
  //handleHttpResponce();
}

//region display

  void WhatToDisplay()
  {
    switch (keyPadSelectIndex) 
    {
      case TEMPERATURE:
      {
        DisplayTemp();
        //Serial.println("TEMPERATURE");
        break;
      }
      case HUMIDITY:
      {
        DisplayHum();
        //Serial.println("HUMIDITY");
        break;
      }
      case RFID_DISPLAY:
      {
        DisplayRfid();
        //Serial.println("RFID_DISPLAY");
        break;
      }
      case WEB_RFID:
      {
        break;
      }
      case EEPROM_FRID:
      {
        break;
      }
      case LED_RGB:
      {
        break;
      }
      case BUZZER_CAP:
      {
        DisplayBuzzerHumcap();
        break;
      }
      default:
      {
        break;
      }
    }
  }

  void Displayhouse()
  {
    ReadKeyPadInput();
    //update everything first
    UpdateDHTValues();
    Rfid();

    //then display
    lcd.display();
    lcd.backlight();

    WhatToDisplay();

    lcd.setCursor(0,0);

    lcd.setCursor(0,1);
    lcd.print(lcdInterface);
    //keyPadSelectIndex
    lcd.setCursor(keyPadSelectIndex * 2,1);
    lcd.print(">");
    lcd.setCursor(0,0);
    MakeMessege();
  }


//endregion

//region DHT
  void DisplayHum()
  {
    lcd.print("humidity:");
    lcd.print(DHT.humidity, 1);
    lcd.print("%");
  }

  void DisplayTemp()
  {
    lcd.print ("temperature:");
    lcd.print(DHT.temperature, 1);
    lcd.print("C");
  }

  void UpdateDHTValues()
  {
    int chk = DHT.read11(DHT11_PIN);
    temp = DHT.temperature;
    hum = DHT.humidity;
  }
//endregion

//region rfid

  void DisplayRfid()
  {
    lcd.print("rfidValue:");
    if(rfidValue == "")
    {
      lcd.print("NaN");
    }
    else
    {
      lcd.print(rfidValue);
    }
  }


  void Rfid()
  {
    if(rfid.isCard())
    {
      //read rfid and if display rfid is true also display it
      if(rfid.readCardSerial())
      {
        // lcd.print(rfid.serNum[0]);
        // lcd.print(".");
        // lcd.print(rfid.serNum[1]);
        // lcd.print(".");
        // lcd.print(rfid.serNum[2]);
        // lcd.print(".");
        // lcd.print(rfid.serNum[3]);
        // lcd.print(".");
        // lcd.print(rfid.serNum[4]);

        rfidValue = (String) rfid.serNum[0] 
          + (String) rfid.serNum[1] 
          + (String) rfid.serNum[2] 
          + (String) rfid.serNum[3] 
          + (String) rfid.serNum[4];
      }
      rfid.selectTag(rfid.serNum);
    }
    rfid.halt();
  }
//endregion

//region rgb led
  void hexToRGB(String hexCode)
  {
    hexCode.remove(0, 1);

    int red = strtol(hexCode.substring(0, 2).c_str(), NULL, 16);
    int green = strtol(hexCode.substring(2, 4).c_str(), NULL, 16);
    int blue = strtol(hexCode.substring(4, 6).c_str(), NULL, 16);
    
    analogWrite(redPin, red); 
    analogWrite(greenPin, green); 
    analogWrite(bluePin, blue); 
  }
//endregion


//region buzzer
  void DisplayBuzzerHumcap()
  {
    lcd.print("tempCap:");
    lcd.print(tempCap);
  }

//endregion

//region keyPad

  void ReadKeyPadInput()
  {
    /*
      0 = 48
      1 = 49
      2 = 50
      ect
      9 = 57
      A = 65
      B = 66
      C = 67
    */
    char customKey = customKeypad.getKey();
    if (customKey)
    {
      if(customKey == 52)
      {
        keyPadSelectIndex--;
        lcd.clear();
      }
      if(customKey == 54)
      {
        keyPadSelectIndex++;
        //Serial.println(keyPadSelectIndex);
        lcd.clear();
        //Serial.println("big chungus");
        //lcdInterface
      }
      Serial.println(customKey);
    }
  }


//endregion


//region messege handling

  void parseString(String input)
  {
    int start = 0;
    int end = input.indexOf("!");

    while (end != -1) 
    {
      String substring = input.substring(start, end);
      int starIndex = substring.indexOf("*");
      String varName = substring.substring(0, starIndex);
      String varValue = substring.substring(starIndex + 1);

      if(varName == "rfidWeb")
      {
        rfidWeb = varValue;
        Serial.println("rfidWeb");
      }
      else if(varName == "tempCap")
      {
        tempCap = varValue.toInt();
        Serial.println("tempCap");
      }
      else if(varName == "HexVal")
      {
        hexToRGB(varValue);
        Serial.println("HexVal");
      }

      start = end + 1;
      end = input.indexOf("!", start);
    }
  }

  void handleHttpResponce()
  {
    String readString; 
    while (client.available()) 
    {
      char c = client.read();
      readString += c;
    }
    if (readString.length() >0) 
    {
      Serial.print(readString);
      parseString(readString);

      client.stop();
      readSenors = true;
    } 
  }

  void MakeMessege()
  {
    String tmpString = "temperature=";
    String humString = "&humidity=";

    content = tmpString + temp + humString + hum;

    if(rfidValue != "")
    {
      String rfidString = "&rfidValue=";
      content = content + rfidString + rfidValue;
    }
  }
  
//endregion


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