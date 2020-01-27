const int switchPin = 8;            // pin that the tilt switch is on
unsigned long previousTime;         // time when the previous led was lit or hourglass was last flipped
unsigned long currentTime = 0;      // current running time
unsigned long previousBlink = 0;    // time since last blink of waiting led
unsigned long previousLine = 0;     // time since last blink of the current led in the line

int switchState = 1;          // current state the tilt switch is in
int lastSwitchState = 1;      // last state the tilt switch was in
int led = 2;                  // current led
byte ledState = LOW;          // the state of the blinking led that's waiting to turn solid
const long interval = 5000;   // amount of time between lighting each led on the hourglass
const int blinkTime = 250;    // amount of time between each blink of the current led
const int lineTime = 50;      // amount of time between turning led on/off in a line after timer is up
byte lineState = LOW;         // the state of the line of LEDs during line blink

void setup() {
  // set led pins as output and tilt switch as input
  for (int x = 2; x < 8; x++) {
    pinMode(x, OUTPUT);
  }

  pinMode(switchPin, INPUT);
}

void loop() {
  // get the current time
  currentTime = millis();

  // make the waiting led blink
  led_blink();

  if (currentTime - previousTime > interval) {
    previousTime = currentTime;

    digitalWrite(led, HIGH);

    // run the line of LEDs on and off as a notification that time is up if all are lit
    if (led == 7) {
      // wait to do anything until the tilt switch is flipped
      line_blink();
    }
    
    led++;

    // turn all LEDs off to start over
    if (switchState != lastSwitchState) {
      for (int x = 2; x < 8; x++) {
        digitalWrite(x, LOW);
      }

      led = 2;
      // swapping this to millis() from currentTime fixed the first light turning on randomly
      previousTime = millis();
    }
  }

  // keep track of the current switch state
  lastSwitchState = switchState;    
}

void led_blink() {
  // if LED is off and blink time has passed, light it up (blink on)
  if (ledState == LOW) {
    if (currentTime - previousBlink >= blinkTime) {
      digitalWrite(led, HIGH);
      ledState = HIGH;
      previousBlink += blinkTime;
    }
  }
  // if LED is on and blink time has passed, turn it off (blink off)
  else {
    if (currentTime - previousBlink >= blinkTime) {
      digitalWrite(led, LOW);
      ledState = LOW;
      previousBlink += blinkTime;
    }
  }
}

void line_blink() {
  // turns LEDs in the line on and off in a wave pattern
  previousLine = currentTime;
  led = 2;
  // run this loop until we see the switchState change
  while (switchState == lastSwitchState) {
    if (millis() - previousLine > lineTime && lineState == LOW) {
       digitalWrite(led, LOW);
       previousLine += lineTime;
       // if we hit the final LED on pin 7, start the line over and turn them on
       if (led == 7) {
        led = 2;
        lineState = HIGH;
       }
       else {
        led++;
       }
    }
    // if it's time for another light in the line to turn off
    else if (millis() - previousLine > lineTime && lineState == HIGH) {
      digitalWrite(led, HIGH);
      previousLine += lineTime;
      // if we hit the final LED on pin 7, start the line over and turn them off
      if (led == 7) {
        led = 2;
        lineState = LOW;
      }
      else {
        led++;
      }
    }

  // check if the tilt switch has changed and we can break the loop
  switchState = digitalRead(switchPin);
  }
}
