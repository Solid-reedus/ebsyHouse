
#define DiDiBtnBlue 3
#define DoDoRedLed 2

int val;
int val2;
int buttonState;
bool DiBtnBlueIsOn;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(DoRedLed, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {


  val = digitalRead(DiBtnBlue);
  delay(10);
  val2 = digitalRead(DiBtnBlue);
  
  if(val == val2)
  {
    if(val != buttonState)
    {
      if(val == LOW)
      {
        if(DiBtnBlueIsOn)
        {
          DiBtnBlueIsOn = false;
        }
        else
        {
          DiBtnBlueIsOn = true;
        }
      }
    }
    buttonState = val;
  }




  if(DiBtnBlueIsOn)
  {
    digitalWrite(DoRedLed, HIGH);  // turn the LED on (HIGH is the voltage level)
  }
  else
  {
    digitalWrite(DoRedLed, LOW);   // turn the LED off by making the voltage LOW
  }

  /*
  digitalWrite(DoRedLed, HIGH);  // turn the LED on (HIGH is the voltage level)
  Serial.print(DoRedLed);
  delay(1000);                      // wait for a second
  digitalWrite(DoRedLed, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);      
  */                // wait for a second
}
