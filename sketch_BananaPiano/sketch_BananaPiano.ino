void setup() {
  Serial.begin(115200);
  
  for (int i = 22; i <= 26; i++) {
    pinMode(i, INPUT);
  }
}

void loop() {
  if (PINA & (1 << 22)) {
    Serial.println("A");
  }
  if (PINA & (1 << 23)) {
    Serial.println("B");
  }
  if (PINA & (1 << 24)) {
    Serial.println("C");
  }
  if (PINA & (1 << 25)) {
    Serial.println("D");
  }
  if (PINA & (1 << 26)) {
    Serial.println("E");
  }
}

