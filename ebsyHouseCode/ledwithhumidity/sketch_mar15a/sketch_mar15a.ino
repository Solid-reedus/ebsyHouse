
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <dht.h>;
dht DHT;

 
#define btnPin 3
#define redLed 2
#define DHT11_PIN 4

byte oldPinState = HIGH;

LiquidCrystal_I2C lcd(0x27,16,2);
// the setup function runs once when you press reset or power the board

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(redLed, OUTPUT);
  lcd.init();
  lcd.backlight();

}

// the loop function runs over and over again forever
void loop() {
  
  byte pinState = digitalRead(btnPin);
  byte ledState = digitalRead(redLed);

  if (pinState != oldPinState)
    {
    delay (10);  // debounce 
    if (pinState == LOW)
      digitalWrite (redLed, !digitalRead (redLed));
    oldPinState = pinState;
    }

  if(ledState == LOW)
  {
    lcd.noBacklight();
    return;
  }
  
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