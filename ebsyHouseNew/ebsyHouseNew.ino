/*
made by: Andrzej Betiuk and Thom van der Vorst
21/04/2023

*/
#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"
#include <Wire.h>
#include <RFID.h>
#include "WiFiEsp.h"
#include <EEPROM.h>
#include <dht.h>;
#include "Keypad.h"
dht DHT;

//these values are for the web section of the code
//and are used to send and receive data to a specified website 
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

//region buzzer
  #define buzzer 8
  int buzzerStatus = LOW;
//endregion

/*
  these values are for the keypad
  first the collums are defined and then the button placement with the char array hexaKeys
  then the pins are defined with rowPins and colPins
  those values are used to make a Keypad class
  and as last 2 chars are defined that are used for the interface controls 
*/
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
  char SubKeyPadSelectIndex = 0;
//endregion

//region SensorValues
  int temp;
  int hum;
  int humCap = 80;
  #define DHT11_PIN 9
//endregion

/*
  these values are used for the rfid code
  rfidValue  = last scanned rfid
  rfidWeb    = the rfid from the web
  rfidEeprom = the rfid from the EEPROM

  the bools rifdSameAsWeb and rfidSameAsEeprom 
  will be true if a web or eeprom rfid is the same as scanned
*/
//region rfid values
  String rfidValue = "";
  String rfidWeb = "";

  bool rifdSameAsWeb = false;
  bool rfidSameAsEeprom = false;  
  //48 = pin / 49 = NRSTPD
  RFID rfid(48,49);
//endregion

//region EEPROM
  #define EEPROM_LENGTH 15
  String rfidEeprom = "";
//endregion

//region rgb led values
  const int redPin = 24;
  const int greenPin = 23;
  const int bluePin = 22; 
  String HexVal = "#ff00ff";
//endregion

//these enums are used as indexes for the interface code
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

// variables for user functions
long timer = 0;
SoftwareSerial softserial(A9, A8);

//region lcd
  LiquidCrystal_I2C lcd(0x27,16,2);
  char lcdInterface[16] = " T H R W E L B ";
//endregion

void setup() 
{
  Serial.begin(115200); 
  lcd.init();
  lcd.backlight();
  SPI.begin();
  rfid.init();
  SetupWifi();
}

void loop() 
{
  if(readSenors == true)
  {
    Displayhouse();
  }
  handleHttpPost();
  handleHttpResponce();
}

//region display

  //this code will decide what will be displayed based on the value of keyPadSelectIndex
  //the display code will display difrent things based on the keypad code
  void WhatToDisplay()
  {
    switch (keyPadSelectIndex) 
    {
      case TEMPERATURE:
      {
        DisplayTemp();
        break;
      }
      case HUMIDITY:
      {
        DisplayHum();
        break;
      }
      case RFID_DISPLAY:
      {
        DisplayRfid();
        break;
      }
      case WEB_RFID:
      {
        DisplayWebRfid();
        break;
      }
      case EEPROM_FRID:
      {
        DisplayEeprom();
        break;
      }
      case LED_RGB:
      {
        DisplayRgbLed();
        break;
      }
      case BUZZER_CAP:
      {
        DisplayBuzzerhumCap();
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
    //first the code will check and update all the values
    ReadKeyPadInput();
    UpdateDHTValues();
    Rfid();
    CheckBuzzer();
    CompareRfid();
    ReadStringFromEEPROM(1);

    //then all the code will be displayed
    lcd.display();
    lcd.backlight();
    // the UI is displayed using WhatToDisplay
    WhatToDisplay();
    lcd.setCursor(0,0);

    lcd.setCursor(0,1);
    lcd.print(lcdInterface);

    //for the user to know where he is in the UI a cursor is displayed
    lcd.setCursor(keyPadSelectIndex * 2,1);
    lcd.print(">");
    lcd.setCursor(0,0);
    //this code will update the messege that will be send to the website
    MakeMessege();
  }

//endregion


//region buzzer

  //this code checks if the humidity is higher then its cap and if so
  //will activate the buzzer 
  void CheckBuzzer()
  {
    if(hum > humCap)
    {
      buzzerStatus = HIGH;
      digitalWrite(buzzer, HIGH);
    }
    else
    {
      buzzerStatus = LOW;
      digitalWrite(buzzer, LOW);
    }
  }

  //this is the UI code of the buzzer to be able to interact with the buzzer
  void DisplayBuzzerhumCap()
  {
    switch (SubKeyPadSelectIndex)
    {
      case 0:
      {
        lcd.print("B:VAL C:CHANGE");
        break;
      }
      case 1:
      {
        lcd.print("A:BCK ");
        lcd.print("humCap:");
        lcd.print(humCap);
        break;
      }
      case 2:
      {
        //changeBuzzerVal will change the humCap value 
        //by pressing 2 (-10) or 8 (+10)  
        changeBuzzerVal();
        lcd.print("A:BCK 2:^ 8:v ");
        lcd.print(humCap);
        break;
      }
    }
  }

//endregion


//region EEPROM

  void DisplayEeprom()
  {
    switch (SubKeyPadSelectIndex)
    {
      case 0:
      {
        lcd.print("B:VL C:CMP D:SET");
        break;
      }
      case 1:
      {
        lcd.print("A:BCK ");
        lcd.print("EEPROM:");
        if(rfidEeprom == "")
        {
          lcd.print("NaN");
        }
        else
        {
          lcd.print(rfidEeprom);
        }
        break;
      }
      case 2:
      {
        lcd.print("A:BCK ");
        lcd.print("THIS=EEP:");
        lcd.print(rfidSameAsEeprom);
        break;
      }
      case 3:
      {
        lcd.print("A:BCK ");
        lcd.print("1:CUR=EEP");
        SetEeprom();
        break;
      }
    }
  }

  void writeStringToEEPROM(int address) 
  {
    if (rfidValue.length() > EEPROM_LENGTH) 
    {
      rfidValue = rfidValue.substring(0, EEPROM_LENGTH);
    }
    
    for (int i = 0; i < rfidValue.length(); i++) 
    {
      EEPROM.write(address + i, rfidValue[i]);
    }
    EEPROM.write(address + rfidValue.length(), '\0');
  }

  void ReadStringFromEEPROM(int address) 
  {
    char buffer[EEPROM_LENGTH + 1];
    int i;
    for (i = 0; i < EEPROM_LENGTH; i++) 
    {
      char c = EEPROM.read(address + i);
      if (c == '\0') 
      {
        break;
      }
      buffer[i] = c;
    }
    buffer[i] = '\0';
    rfidEeprom = String(buffer);
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

  //this code will compare the scanned rfid tag 
  //and will turn bools true if they are the same in the eeprom and web
  void CompareRfid()
  {
    if(rfidValue == rfidWeb)
    {
      rifdSameAsWeb = true;
    }
    else
    {
      rifdSameAsWeb = false;
    }


    if(rfidValue == rfidEeprom)
    {

      rfidSameAsEeprom = true;
    }
    else
    {
      rfidSameAsEeprom = false;
    }

  }

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

  //this code will display the following
  // 0/A = main menu of the 
  // 1/b = display value
  // 2/c = 0 means it isnt equal 1 means its the same
  void DisplayWebRfid()
  {
    switch (SubKeyPadSelectIndex)
    {
      case 0:
      {
        lcd.print("B:VAL C:COMPARE");
        break;
      }
      case 1:
      {
        lcd.print("A:BCK ");
        lcd.print("WEB:");
        if(rfidValue == "")
        {
          lcd.print("NaN");
        }
        else
        {
          lcd.print(rfidValue);
        }
        break;
      }
      case 2:
      {
        lcd.print("A:BCK ");
        lcd.print("THIS=WEB:");
        lcd.print(rifdSameAsWeb);
        break;
      }
    }
  }

  //this code will read the rfid card and save it 
  //you can also save it in eeprom by going to eeprom->set->1 ( E->D->1 )
  void Rfid()
  {
    if(rfid.isCard())
    {
      //read rfid and if display rfid is true also display it
      if(rfid.readCardSerial())
      {
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

  void DisplayRgbLed()
  {
    lcd.print("hex code:");
    lcd.print(HexVal);
  }
  
//endregion


//region keyPad

  void changeBuzzerVal()
  {
    char customKey = customKeypad.getKey();
    if(customKey == '2' && humCap < 90)
    {
      humCap += 10;
    }
    else if(customKey == '8' && humCap > 10)
    {
      humCap -= 10;
    }
  }

    void SetEeprom()
    {
      char customKey = customKeypad.getKey();
      if(customKey == '1' && rfidValue != "")
      {
        writeStringToEEPROM(1);
      }
    }

  void ReadKeyPadInput()
  {
    char customKey = customKeypad.getKey();
    if (customKey)
    {
      switch (customKey) 
      {
        case '4':
        {
          if(keyPadSelectIndex > 0)
          {
            keyPadSelectIndex--;
            SubKeyPadSelectIndex = 0;
          }
          break;
        }
        case '6':
        {
          if(keyPadSelectIndex < 6)
          {
            keyPadSelectIndex++;
            SubKeyPadSelectIndex = 0;
            lcd.clear();
          }
          break;
        }
        case 'A':
        {
          SubKeyPadSelectIndex = 0;
          break;
        }
        case 'B':
        {
          SubKeyPadSelectIndex = 1;
          break;
        }
        case 'C':
        {
          SubKeyPadSelectIndex = 2;
          break;
        }
        case 'D':
        {
          SubKeyPadSelectIndex = 3;
          break;
        }
        default:
        {
          break;
        }
      }
      Serial.println(customKey);
      lcd.clear();
    }
  }


//endregion


//region messege handling

  /*
    this function will read the string of information that is send from the website
    the reading process will always start by reading the first letter, then it will go through the array and look for a *
    the string inside the beginning and the * will be varName. and the string inside * and ! is the value
    then the prosess will then start again after the ! until there are no indexes in the string
  */
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
      else if(varName == "humCap")
      {
        humCap = varValue.toInt();
        Serial.println("humCap");
      }
      else if(varName == "HexVal")
      {
        HexVal = varValue;
        hexToRGB(varValue);
        Serial.println("HexVal");
      }

      start = end + 1;
      end = input.indexOf("!", start);
    }
  }

  //this code will read the response given by the website and prase it
  //when the client is available the readSenors will be also true 
  //so the program has enough time to handle the response
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

  //this function will make a string that will be send out as a post
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