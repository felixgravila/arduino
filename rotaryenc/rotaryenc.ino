//#include <Keyboard.h>

const int PinA = 2; //CLK 
const int PinB = 6; //DT  
const int PinSW = 15; //button switch

// Simple PWM LED pin
#define PinLED 13

// Keep track of last rotary value
int lastCount = 25;

// Updated by the ISR (Interrupt Service Routine)
volatile int virtualPosition = 25;

// ------------------------------------------------------------------
// INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT 
// ------------------------------------------------------------------
void isr ()  {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(PinB) == LOW)
    {
      //virtualPosition-- ;
//      Keyboard.press(0xE9);
//      delay(100);
//      Keyboard.releaseAll();
    }
    else {
      //virtualPosition++ ;
//      Keyboard.press(0xEA);
//      delay(100);
//      Keyboard.releaseAll();
    }

    // Restrict value from 0 to +50
    virtualPosition = min(50, max(0, virtualPosition));

    // Keep track of when we were here last (no more than every 5ms)
    lastInterruptTime = interruptTime;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PinA, INPUT_PULLUP);
  pinMode(PinB, INPUT_PULLUP);
  pinMode(PinSW, INPUT_PULLUP);
  Serial.println("Start");
  
  attachInterrupt(digitalPinToInterrupt(PinA), isr, LOW);
}

void loop() {

  // Is someone pressing the rotary switch?
  if ((!digitalRead(PinSW))) {
    virtualPosition = 25;
    while (!digitalRead(PinSW))
      delay(10);
    Serial.println("Reset");
  }

  // If the current rotary switch position has changed then update everything
  if (virtualPosition != lastCount) {
    
    // Our LED gets brighter or dimmer
    analogWrite(PinLED, map(virtualPosition, 0, 50, 0, 255));

    // Write out to serial monitor the value and direction
    Serial.print(virtualPosition > lastCount ? "Up  :" : "Down:");
    Serial.println(virtualPosition);

    // Keep track of this new value
    lastCount = virtualPosition ;
  }
}
