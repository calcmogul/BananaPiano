void setup() {
    Serial.begin(115200);

    for (int i = 0; i <= 11; i++) {
        pinMode(i, INPUT);
    }
}

void loop() {
    // G#, A, Bb, B, C, C#, D, Eb, E, F, F#, G
    for (int i = 0; i <= 11; i++) {
        if (analogRead(i) > 3.0) {
            Serial.print("0");
        }
        else {
            Serial.print("1");
        }
    }
    Serial.println("");
}

