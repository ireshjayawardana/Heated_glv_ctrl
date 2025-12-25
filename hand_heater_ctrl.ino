#define led3 9
#define led4 6
#define led5 10

#define FET 3

#define temp A3
#define button 4

bool button_push = false;

int adc_raw = 0;


void setup() {
  // put your setup code here, to run once:

  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode (FET, OUTPUT);
  pinMode (temp,INPUT);
  pinMode (button,INPUT);
  Serial.begin(115200);

  
}

void loop() {
  // put your main code here, to run repeatedly:
   
  adc_raw = analogRead(temp);
  int temp_read = vlt_to_temp (adc_raw);
  int button_state = digitalRead(button);
  Serial.print ("adc ");
  Serial.print (adc_raw);
  Serial.print (" temp ");
  Serial.print (temp_read);
  Serial.print(" button ");
  Serial.println (button_state);

  digitalWrite(led3,HIGH);
  digitalWrite(led4,HIGH);
  digitalWrite(led5,HIGH);
  delay(500);
  digitalWrite(led3,LOW);
  digitalWrite(led4,LOW);
  digitalWrite(led5,LOW);
  delay(500);

// digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
//   delay(1000);                      // wait for a second
//   digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
//   delay(1000); 
}

int vlt_to_temp (int adc){

  float x = (adc/1024.0) * 3.3;
  int y = 100 * x - 52;
  return y;
}
