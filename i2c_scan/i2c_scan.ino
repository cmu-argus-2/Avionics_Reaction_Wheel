#include <Wire.h>
void setup() {
    Wire.begin();
    Serial.begin(115200);
    delay(2000);
    Serial.println("Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t result = Wire.endTransmission();
        if (result == 0) {
            Serial.print("Found device at 0x");
            Serial.println(addr, HEX);
        }
    }
    Serial.println("Scan done.");
}
void loop() {}