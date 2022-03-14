#define dbg(myFixedText, variableName) \
  Serial.print( F(#myFixedText " "  #variableName"=") ); \
  Serial.println(variableName);
// usage: dbg("1:my fixed text",myVariable);
// myVariable can be any variable or expression that is defined in scope

#define dbgi(myFixedText, variableName,timeInterval) \
  do { \
    static unsigned long intervalStartTime; \
    if ( millis() - intervalStartTime >= timeInterval ){ \
      intervalStartTime = millis(); \
      Serial.print( F(#myFixedText " "  #variableName"=") ); \
      Serial.println(variableName); \
    } \
  } while (false);



void PrintFileNameDateTime() {
  Serial.println( F("Code running comes from file ") );
  Serial.println( F(__FILE__) );
  Serial.print( F("  compiled ") );
  Serial.print( F(__DATE__) );
  Serial.print( F(" ") );
  Serial.println( F(__TIME__) );
}



boolean TimePeriodIsOver (unsigned long &expireTime, unsigned long TimePeriod) {
  unsigned long currentMillis  = millis();
  if ( currentMillis - expireTime >= TimePeriod ) {
    expireTime = currentMillis; // set new expireTime
    return true;                // more time than TimePeriod) has elapsed since last time if-condition was true
  }
  else return false;            // not expired
}

const byte OnBoard_LED = 13;

void BlinkHeartBeatLED(int IO_Pin, int BlinkPeriod) {
  static unsigned long MyBlinkTimer;
  pinMode(IO_Pin, OUTPUT);

  if ( TimePeriodIsOver(MyBlinkTimer, BlinkPeriod) ) {
    digitalWrite(IO_Pin, !digitalRead(IO_Pin) );
  }
}


//declare pins
const byte motor1pin1 = 2;
const byte motor1pin2 = 3;

const byte motor2pin1 = 4;
const byte motor2pin2 = 5;

const byte motor1EnablePin =  9;
const byte motor2EnablePin = 10;

// define constants for state-machine
const byte motor1Start   = 0;
const byte motor1Run     = 1;
const byte motor1Stop    = 2;
const byte motor1WaitOff = 3;

const byte motor2Start   = 4;
const byte motor2Run     = 5;
const byte motor2Stop    = 6;
const byte motor2WaitOff = 7;

byte MyStateNr = motor1Start;

// define contants for non-blocking timing
const unsigned long motor1OnPeriod  =  500;
const unsigned long motor1OFFPeriod = 2000;

const unsigned long motor2OnPeriod  =  500;
const unsigned long motor2OFFPeriod = 2000;

unsigned long MyMotorTimer = 0;                   // Timer-variables MUST be of type unsigned long

byte interationCounter = 1;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup-Start");
  PrintFileNameDateTime();

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  pinMode(motor1EnablePin, OUTPUT);
  pinMode(motor2EnablePin, OUTPUT);

  //Controlling speed (0 = off and 255 = max speed):
  analogWrite(motor1EnablePin, 140); //ENA pin
  analogWrite(motor2EnablePin, 140); //ENB pin
}


void loop() {
  // blink onboard-LED to show loop is looping
  BlinkHeartBeatLED(OnBoard_LED, 100);

  MotorSequenceStateMachine();
}

// define functions for motors on / OFF
void Motor1ONccW() {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
}

void Motor1ONcW()
{
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
}

void Motor1OFF() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
}

void Motor2ONcW() {
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void Motor2ONccW() {
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void Motor2OFF() {
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}



void MotorSequenceStateMachine() {

  switch (MyStateNr) {

    // Motor 1
    case motor1Start:
      MyMotorTimer = millis(); // store time-stamp when motor is switched on
      Motor1ONccW();
      Motor2ONccW();
      MyStateNr = motor1Run;
      dbg("motor1Start", MyMotorTimer);
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    case motor1Run:
      if ( TimePeriodIsOver(MyMotorTimer, motor1OnPeriod) ) {
        MyStateNr = motor1Stop;
      }
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    case motor1Stop:
      MyMotorTimer = millis(); // store time-stamp when motor is switched OFF
      Motor1OFF();
      Motor2OFF();
      MyStateNr = motor1WaitOff;
      dbg("motor1Stop", MyMotorTimer);
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    case motor1WaitOff:
      if ( TimePeriodIsOver(MyMotorTimer, motor1OFFPeriod) ) {
        if (interationCounter <= 1) {
          interationCounter++;
          MyStateNr = motor1Start;
        }
        else {
          interationCounter = 1;
          MyStateNr = motor2Start;          
        }
      }
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    // 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
    // Motor 2
    case motor2Start:
      MyMotorTimer = millis(); // store time-stamp when motor is switched on
      Motor2ONcW();
      Motor1ONcW();
      MyStateNr = motor2Run;
      dbg("motor2Start", MyMotorTimer);
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    case motor2Run:
      if ( TimePeriodIsOver(MyMotorTimer, motor2OnPeriod) ) {
        MyStateNr = motor2Stop;
      }
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    case motor2Stop:
      MyMotorTimer = millis(); // store time-stamp when motor is switched OFF
      Motor2OFF();
      Motor1OFF();
      MyStateNr = motor2WaitOff;
      dbg("motor2Stop", MyMotorTimer);
      break; // jump immidiately to the code marked with "xx-EXIT-xx"

    case motor2WaitOff:
      if ( TimePeriodIsOver(MyMotorTimer, motor2OFFPeriod) ) {
        if (interationCounter <= 1) {
          interationCounter++;
          MyStateNr = motor2Start;
        }
        else {
          interationCounter = 1;
          MyStateNr = motor1Start;          
        }
      }
      break; // jump immidiately to the code marked with "xx-EXIT-xx"
  } // xx-EXIT-xx
}
