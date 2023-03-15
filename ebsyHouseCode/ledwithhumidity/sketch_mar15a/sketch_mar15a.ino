#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <dht.h>;
dht DHT;

 
#define btnBlue 3
#define redLed 2
#define DHT11_PIN 4

int val;
int val2;

int numy = 0;

int buttonState;
bool btnBlueIsOn;

LiquidCrystal_I2C lcd(0x27,16,2);
// the setup function runs once when you press reset or power the board


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(redLed, OUTPUT);

  lcd.init();
  lcd.backlight();
  /*
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello World");
  lcd.setCursor(2,1);
  lcd.print("Arduino is awesome!"); 
  */
}

// the loop function runs over and over again forever
void loop() {



  val = digitalRead(btnBlue);
  delay(10);
  val2 = digitalRead(btnBlue);
  
  if(val == val2)
  {
    if(val != buttonState)
    {
      if(val == LOW)
      {
        if(btnBlueIsOn)
        {
          btnBlueIsOn = false;
        }
        else
        {
          btnBlueIsOn = true;
        }
      }
    }
    buttonState = val;
  }

  if(!btnBlueIsOn)
  {
    lcd.backlight();
    lcd.noBacklight();
    return;
  }
  

    lcd.display();
    lcd.backlight();
    //lcd.setCursor(1,0);
    
    lcd.setCursor(3,0);
    int chk = DHT.read11(DHT11_PIN);
    lcd.print(DHT.humidity, 1);
    //lcd.print(numy);
    
    numy++;



////////////////

/*

byte lamp = digitalRead(redLed);
  val = digitalRead(btnBlue);
  delay(10);
  val2 = digitalRead(btnBlue);

  if(lamp == HIGH){
    lcd.setCursor(0, 1);

    int chk = DHT.read11(DHT11_PIN);

    lcd.print(DHT.humidity, 1);
  }
*/


    ////////////



  if(btnBlueIsOn)
  {
    digitalWrite(redLed, HIGH);  // turn the LED on (HIGH is the voltage level)
  }
  else
  {
    digitalWrite(redLed, LOW);   // turn the LED off by making the voltage LOW
  }

  /*
  digitalWrite(redLed, HIGH);  // turn the LED on (HIGH is the voltage level)
  Serial.print(redLed);
  delay(1000);                      // wait for a second
  digitalWrite(redLed, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);      
  */                // wait for a second
}
