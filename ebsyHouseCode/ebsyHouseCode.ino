
#define btnBlue 3
#define redLed 2

int val;
int val2;
int buttonState;
bool btnBlueIsOn;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(redLed, OUTPUT);
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
