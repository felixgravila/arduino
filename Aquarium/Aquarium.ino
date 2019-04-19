
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

//plug relays
#define main_light 9
#define secondary_light 8
#define carbon_dioxide 7
#define pump 6

//buttons
#define pin_on 3
#define pin_feed 2

#define feedtime 900 //15 minutes
#define ontime 7200 //2 hours

volatile boolean feed_locked;
volatile boolean on_locked;
volatile unsigned long action_time_end;
volatile unsigned long uxtime;
DateTime dtNow;
unsigned long lastmillis = 0L;
unsigned long debounce_time;

void setup () {
  feed_locked = false;
  on_locked = false;
  action_time_end = 0L;
  debounce_time = 0L;
  uxtime = 0L;
  Serial.begin(2000000);
  Wire.begin();
  pinMode(main_light, OUTPUT);
  pinMode(secondary_light, OUTPUT);
  pinMode(carbon_dioxide, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(pin_on, INPUT_PULLUP);
  pinMode(pin_feed, INPUT_PULLUP);
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void checkandapply (int pin, int beginhour, int endhour) {
  if(dtNow.hour()>beginhour && dtNow.hour()<endhour) {
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, HIGH);
  }
}


void loop () {
    if(millis() - lastmillis >= 1000){
      lastmillis = millis();
      dtNow = RTC.now();
      uxtime = dtNow.unixtime();
      print_status();
      if(action_time_end < uxtime){
        Serial.println("IN LOOP");
        setdefaults();
        feed_locked = false;
        on_locked = false;
      }
    }
    if(digitalRead(pin_feed) == LOW && millis() - debounce_time >= 300){
      debounce_time = millis();
      feed();
    }
    if(digitalRead(pin_on) == LOW && millis() - debounce_time >= 300){
      debounce_time = millis();
      light_on();
    }
}

void feed() {
  feed_locked = !feed_locked;
  if(feed_locked) {
    on_locked = false;
    action_time_end = uxtime + feedtime;
    digitalWrite(main_light, LOW);
    digitalWrite(pump, HIGH);
  } else {
    action_time_end = 0L;
    digitalWrite(pump, LOW);
    setdefaults();
  }
}

void light_on() {
  on_locked = !on_locked;
  if(on_locked){
    feed_locked = false;
    action_time_end = uxtime + ontime;
    digitalWrite(main_light, LOW);
    digitalWrite(secondary_light, LOW);
    digitalWrite(pump, LOW);
  } else {
    action_time_end = 0L;
    setdefaults();
  }
}

void setdefaults(){
  checkandapply(main_light, 8, 18);
  checkandapply(secondary_light, 10, 15);
  checkandapply(carbon_dioxide, 7, 18);
  digitalWrite(pump, LOW);
}

void print_status(){
  Serial.println();
  
  Serial.print(dtNow.year(), DEC);
  Serial.print('/');
  Serial.print(dtNow.month(), DEC);
  Serial.print('/');
  Serial.print(dtNow.day(), DEC);
  Serial.print(' ');
  Serial.print(dtNow.hour(), DEC);
  Serial.print(':');
  Serial.print(dtNow.minute(), DEC);
  Serial.print(':');
  Serial.println(dtNow.second(), DEC);

  Serial.print("feed_locked: ");
  Serial.println(feed_locked);
  Serial.print("on_locked:   ");
  Serial.println(on_locked);

  Serial.print("Program supposed to end at uxtime ");
  Serial.print(action_time_end);
  Serial.print(", currently ");
  Serial.println(uxtime);

  Serial.println("=================================");
  Serial.println();
}



