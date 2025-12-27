#define led3 6
#define led4 9
#define led5 10

#define FET 3

#define temp A3
#define button 4
#define v72 A0
#define v33 A1

#define led_brightness 100

#define LOW_BAT_THRESHOLD 7.3

bool button_push = false;

int adc_raw = 0;

typedef struct {
  float temperature;  //  °C
  float v7_2;         // 7.2V rail
  float v3_3;         // 3.3V rail
} SensorData;


unsigned long lastBlinkTime = 0;
bool ledState = false;
uint8_t blinkStep = 0;


int lastButtonState = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;


enum State {
  IDLE,
  ON,
  HEAT_1,
  HEAT_2,
  HEAT_3,
  LOW_BAT
};

SensorData data;
State currentState;
void setup() {
  // put your setup code here, to run once:

  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode (FET, OUTPUT);
  pinMode (temp,INPUT);
  pinMode (button,INPUT);
  pinMode (v72, INPUT);
  pinMode (v33, INPUT);
  Serial.begin(115200);

  
}

void loop() {
  // put your main code here, to run repeatedly:
   readSensors(&data);
  
 // int button_state = digitalRead(button);
  Serial.print ("temp ");
  Serial.print (data.temperature);
  Serial.print (" bat ");
  Serial.print (data.v7_2);
  Serial.print(" 3.3 ");
  Serial.print (data.v3_3);
  Serial.print (" state ");
  bool button_state = button_pushed();
  Serial.println (currentState);

  if (data.v7_2 < LOW_BAT_THRESHOLD) {
    currentState = LOW_BAT;
  }

  
  if (button_state == true){
    advanceState();
  }

  updateleds();
  switch (currentState) {

  case IDLE:
    // everything off
    set_heater(0);
    if (data.temperature < 5){
      analogWrite(led5,100);
    }
    break;

  case ON:
    // system powered but no heat
    if (data.temperature < 10){
      set_heater(150);
    }
    else{
      set_heater(100);
    }
    break;

  case HEAT_1:
    // low heating
      if (data.temperature < 10){
      set_heater(200);
    }
    else{
      set_heater(150);
    }
    break;

  case HEAT_2:
    // medium heating
        if (data.temperature < 10){
      set_heater(220);
    }
    else{
      set_heater(200);
    }
    break;

  case HEAT_3:
    // max heating
    set_heater(230);
    break;

  case LOW_BAT:
    // disable heater, warn user
    set_heater(0);
    break;
}



}

int vlt_to_temp (int adc){

  float x = (adc/1024.0) * 3.3;
  float y = 100.0 * x - 52.0;
  return y;
}


bool button_pushed(){
  int button_state = digitalRead(button);

  if (button_state == 0){
    //lastDebounceTime = millis();
    delay(50);
    if (button_state == 0 && lastButtonState == 1){
      lastButtonState = button_state;
      return true;
    }

    else{
      return false;
    }
  }
  else{
    lastButtonState = button_state;
    return false;
  }

  // if (lastButtonState == 1 && button_state == 0){
  //   lastButtonState = button_state;
  //   return true;
  // }
  // else{
  //   //lastButtonState = button_state;
  //   return false;
  // }
  

}

void readSensors(SensorData *data){
  int adc_raw = analogRead(temp);
  data->temperature = vlt_to_temp (adc_raw);
  adc_raw = analogRead(v72);
  float v7_2 = adc_raw/1024.0 * 3.3 * 11.0;

  data->v7_2 = v7_2;
  adc_raw = analogRead(v33);
  float v3_3 = adc_raw/1024.0 * 3.3 * 11.0;
  data->v3_3 = v3_3;
}

void advanceState() {
  currentState = (State)((currentState + 1) % LOW_BAT);
}



void updateleds() {
  unsigned long now = millis();

  switch (currentState) {

    case IDLE:  // slow blink led3
      if (now - lastBlinkTime >= 2000) {
        lastBlinkTime = now;
        ledState = !ledState;
        if (ledState == true){
           analogWrite(led3, led_brightness);
        }
        else{
          analogWrite(led3, 0);
        }
       
      }
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      break;

    case ON:  // solid led3 + led4
      analogWrite(led3, led_brightness);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, LOW);
      break;

    case HEAT_1:  // fast blink led3
      if (now - lastBlinkTime >= 300) {
        lastBlinkTime = now;
        ledState = !ledState;
        if (ledState == true){
           analogWrite(led3, led_brightness);
        }
        else{
          analogWrite(led3, 0);
        }
      }
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      break;

    case HEAT_2:  // double blink led4
      if (now - lastBlinkTime >= 200) {
        lastBlinkTime = now;
        blinkStep++;
        digitalWrite(led4, (blinkStep == 1 || blinkStep == 3));
        if (blinkStep >= 4) blinkStep = 0;
      }
      analogWrite(led3, 0);
      digitalWrite(led5, LOW);
      break;

    case HEAT_3:  // very fast blink led5
      if (now - lastBlinkTime >= 150) {
        lastBlinkTime = now;
        ledState = !ledState;
        digitalWrite(led5, ledState);
      }
      analogWrite(led3, 0);
      digitalWrite(led4, LOW);
      break;

    case LOW_BAT:  // triple blink all leds
      if (now - lastBlinkTime >= 200) {
        lastBlinkTime = now;
        blinkStep++;
        bool on = (blinkStep == 1 || blinkStep == 3 || blinkStep == 5);
        //digitalWrite(led3, on);
        digitalWrite(led4, on);
        digitalWrite(led5, on);
        if (blinkStep >= 6) blinkStep = 0;
      }
      break;
  }
}

void set_heater(int x){
  if (x > 255){
    x = 255;
  }
  else if (x < 0){
    x = 0;
  }

  analogWrite (FET,x);
}
