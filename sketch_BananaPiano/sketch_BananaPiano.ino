void setup() {
    Serial.begin(115200);

    for (int i = 0; i <= 11; i++) {
        pinMode(i, INPUT);
    }
}

void loop() {
    // C, C#, D, D#, E, F, F#, G, G#, A, A#, B
    for (int i = 0; i <= 11; i++) {
        if (analogRead(i) > 2.0) {
            Serial.print("1");
        }
        else {
            Serial.print("0");
        }
    }
    Serial.println("");
}

