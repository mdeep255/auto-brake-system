const int TRIG_PIN   = 9;
const int ECHO_PIN   = 10;
const int STEP_PIN   = 3;   // A4988 Step Pin
const int DIR_PIN    = 4;   // A4988 Direction Pin
const int ENABLE_PIN = 5;   // A4988 Enable Pin
const int LED_GREEN  = 6;   
const int LED_YELLOW = 7;   
const int LED_RED    = 8;   
const int BUZZER_PIN = 11;  

const int WARN_DISTANCE = 40; 
const int STOP_DISTANCE = 15; 

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Enable the driver and set direction
  digitalWrite(ENABLE_PIN, LOW); // A4988 Enable is ACTIVE LOW
  digitalWrite(DIR_PIN, HIGH);   // Forward direction
}

void loop() {
  long distance = readDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= STOP_DISTANCE && distance > 0) {
    emergencyBrake();
  } 
  else if (distance <= WARN_DISTANCE && distance > STOP_DISTANCE) {
    applyWarningMode(distance);
  } 
  else {
    applyNormalDrive();
  }
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

// Generates step pulses to turn the stepper motor
void stepMotor(int stepDelayUs) {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelayUs);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(stepDelayUs);
}

void applyNormalDrive() {
  digitalWrite(ENABLE_PIN, LOW); // Motor Active
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER_PIN);

  // Fast rotation (small delay between steps = high speed)
  for(int i = 0; i < 20; i++) {
    stepMotor(1000); 
  }
}

void applyWarningMode(long dist) {
  digitalWrite(ENABLE_PIN, LOW); // Motor Active
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  tone(BUZZER_PIN, 1000, 50);

  // Slow down speed based on distance (larger delay = slower rotation)
  int stepDelay = map(dist, STOP_DISTANCE, WARN_DISTANCE, 4000, 2000);
  for(int i = 0; i < 10; i++) {
    stepMotor(stepDelay); 
  }
}

void emergencyBrake() {
  // DISABLE MOTOR INSTANTLY (Brake / Cut Power)
  digitalWrite(ENABLE_PIN, HIGH); // A4988 Disable is HIGH
  
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);
  tone(BUZZER_PIN, 2000);
}