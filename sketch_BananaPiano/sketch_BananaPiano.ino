void setup() {
    Serial.begin(115200);

    for (int i = 0; i <= 11; i++) {
        pinMode(i, INPUT_PULLUP);
    }
}

void loop() {
    // G#, A, Bb, B, C, C#, D, Eb, E, F, F#, G
    for (int i = 0; i <= 11; i++) {
        // '1' means pin is HIGH; '0' means pin is LOW
        if (analogRead(i) * 0.0049 > 3.0) {
            Serial.write('1');
        }
        else {
            Serial.write('0');
        }
    }
    Serial.write('\n');

    delay(50);
}

