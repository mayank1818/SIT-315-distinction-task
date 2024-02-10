// Define LED pin numbers
int led1 = 8;
int led2 = 9;
int led3 = 11;
int led4 = 12;

// Define PIR and Soil Moisture sensor pin numbers
int pir1 = 2; // PIR sensor 1
int pir2 = 3; // PIR sensor 2
int soilmoisture3 = 4; // Soil Moisture sensor

// Variables to store the state of PIR sensors and Soil Moisture sensor
int pirState1 = LOW;
int pirState2 = LOW;
int soilMoistureState = LOW;

void setup() {
  // Set LED pins as OUTPUT
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  // Set sensor pins as INPUT
  pinMode(pir1, INPUT); // PIR sensor 1
  pinMode(pir2, INPUT); // PIR sensor 2
  pinMode(soilmoisture3, INPUT); // Soil Moisture sensor

  // Start serial communication
  Serial.begin(9600);
  Serial.println("Setup complete");

  // Attach interrupts for PIR sensors and Soil Moisture sensor
  attachInterrupt(digitalPinToInterrupt(pir1), motionDetected1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pir2), motionDetected2, CHANGE);
  pciSetup(soilmoisture3);

  // Start timer with a frequency of 0.5 Hz (blinks every 2 seconds)
  startTimer(0.5);
}

// Function to set up Pin Change Interrupt for Soil Moisture sensor
void pciSetup(byte pin) {
  *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin)); // enable pin
  PCIFR |= bit(digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR |= bit(digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

// Interrupt Service Routine for Soil Moisture sensor
ISR(PCINT2_vect) {
  soilMoistureDetected();
}

// Function to start a timer with a given frequency
void startTimer(double timerFrequency) {
  noInterrupts();

  uint16_t ocrValue = (uint16_t)(F_CPU / 1024.0 / timerFrequency - 1);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = ocrValue;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);

  interrupts();
}

// Interrupt Service Routine for Timer
ISR(TIMER1_COMPA_vect) {
  digitalWrite(led4, digitalRead(led4) ^ 1);
}

// Interrupt Service Routine for PIR sensor 1
void motionDetected1() {
  int val = digitalRead(pir1);
  if (val == HIGH) {
    digitalWrite(led1, HIGH);
    if (pirState1 == LOW) {
      Serial.println("Motion detected on PIR 1!");
      Serial.println("LED ON");
      pirState1 = HIGH;
    }
  } else {
    digitalWrite(led1, LOW);
    if (pirState1 == HIGH) {
      Serial.println("Motion ended on PIR 1!");
      Serial.println("LED OFF");
      pirState1 = LOW;
    }
  }
}

// Interrupt Service Routine for PIR sensor 2
void motionDetected2() {
  int val = digitalRead(pir2);
  if (val == HIGH) {
    digitalWrite(led2, HIGH);
    if (pirState2 == LOW) {
      Serial.println("Motion detected on PIR 2!");
      Serial.println("LED ON");
      pirState2 = HIGH;
    }
  } else {
    digitalWrite(led2, LOW);
    if (pirState2 == HIGH) {
      Serial.println("Motion ended on PIR 2!");
      Serial.println("LED OFF");
      pirState2 = LOW;
    }
  }
}

// Interrupt Service Routine for Soil Moisture sensor
void soilMoistureDetected() {
  Serial.println("Soil Moisture interrupt triggered");
  int val = digitalRead(soilmoisture3);
  Serial.println("Soil Moisture Value: " + String(val));

  if (val == HIGH) {
    digitalWrite(led4, HIGH);
    if (soilMoistureState == LOW) {
      Serial.println("Soil Moisture detected!");
      Serial.println("LED ON");
      soilMoistureState = HIGH;
    }
  } else {
    digitalWrite(led4, LOW);
    if (soilMoistureState == HIGH) {
      Serial.println("Soil Moisture ended!");
      Serial.println("LED OFF");
      soilMoistureState = LOW;
    }
  }
}

void loop() {
  delay(1000); // Add any delay or other code here if needed
}