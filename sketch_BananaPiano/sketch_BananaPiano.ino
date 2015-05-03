void setup() {
    Serial.begin(115200);

    for (int i = 0; i <= 11; i++) {
        pinMode(i, INPUT);
    }

    for (int i = 42; i <= 53; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH);
    }
}

void loop() {
    // G#, A, Bb, B, C, C#, D, Eb, E, F, F#, G
    for (int i = 0; i <= 11; i++) {
        // '1' means pin is HIGH; '0' means pin is LOW
        if (analogRead(i) * 0.0049 > 3.5) {
            Serial.write('1');
        }
        else {
            Serial.write('0');
        }
    }
    Serial.write('\n');

    delay(40);
}

