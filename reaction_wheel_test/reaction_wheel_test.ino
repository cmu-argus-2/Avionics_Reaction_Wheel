#include <Wire.h>

#define MCF8315_I2C_ADDR 0x5A
#define AS5600_I2C_ADDR 0x36
#define DELAY_US_BETWEEN_BYTES 100

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(5000);

  Serial.println("Starting write now.");

  // === Configuration Registers ===

  // From TI GUI start
  writeRegister32(0x0084, 0x0B260340); // MOTOR_STARTUP1  
  writeRegister32(0x0086, 0x0B836004); // MOTOR_STARTUP2 
  writeRegister32(0x0088, 0x104187B8); // CLOSED_LOOP1    
  writeRegister32(0x008A, 0x0BACCCA9); // CLOSED_LOOP2    
  writeRegister32(0x008C, 0x22E15804); // CLOSED_LOOP3    
  writeRegister32(0x008E, 0x5FD7C71A); // CLOSED_LOOP4    
  writeRegister32(0x0090, 0x1A202087); // FAULT_CONFIG1   
  writeRegister32(0x0092, 0x75482340); // FAULT_CONFIG2   
  writeRegister32(0x00A4, 0x08000309); // PIN_CONFIG      
  writeRegister32(0x00A6, 0x05A00001); // DEVICE_CONFIG1  
  writeRegister32(0x00A8, 0x83E8F007); // DEVICE_CONFIG2  
  writeRegister32(0x00AA, 0x41CC5C00); // PERI_CONFIG1    (default CW direction)
  writeRegister32(0x00AC, 0x0C041100); // GD_CONFIG1      
  writeRegister32(0x00AE, 0x01840000); // GD_CONFIG2      
  writeRegister32(0x00A0, 0x09448005); // INT_ALGO_1      
  writeRegister32(0x00A2, 0x000001E0); // INT_ALGO_2      
  // From TI GUI end

  // Direction Set start
  setDirectionCCW();
  // Direction Set end

  Serial.println("Finish write in shadow now.");

  // === Trigger EEPROM Storage ===
  writeRegister32(0x0000EA, 0x8A500000); // EEPROM_PROG_TRIG
  delay(300); // Wait >300ms to allow EEPROM programming

  Serial.println("MCF8315C configured and EEPROM programmed.");

  Wire.end();
}

// uint32_t speed = 0x86660000;
// 0x8CCC0000
// 0x93330000
uint32_t speed = 0x86660000;
// Abt 2600RPM (rated) speed = 0x95C20000
void loop() {
  Wire.begin();
  Serial.begin(115200);
  delay(20000);

  Serial.println("Writing Speed.");
  // === Configuration Registers ===
  // writeRegister32(0x00EC, 0xCC000000); // MOTOR_STARTUP1 + force IPD, remove 1 for remove IPD
  // writeRegister32(0x00EC, 0xCC000000);

  writeRegister32(0x00EC, speed);
  // if (speed < 0x95C20000) {
  //   speed = speed + 0x00050000;
  // }
  
  Serial.print("Speed: ");
  Serial.println(speed, HEX);

  delay(2000); // Wait >300ms to allow EEPROM programming

  // delay(100000);
  // Serial.println("Slow here Speed.");
  // writeRegister32(0x00EC, 0x80000000);
  // delay(10000);

  // Do nothing
}

// Write 32-bit to register
void writeRegister32(uint16_t address, uint32_t data) {
  Wire.beginTransmission(MCF8315_I2C_ADDR);

  // I2C Header: [control][memory address]
  uint8_t ctrl[3];
  ctrl[0] = 0x10;             // OP_R/W=0 (write), CRC=0, DLEN=01 (32-bit), MEM_SEC=0
  ctrl[1] = 0x00;             // MEM_PAGE=0, 0x0 for adress beginning
  ctrl[2] = address & 0xFF;   // EEPROM register offset
  // target_ID already sent by wire.write
  Wire.write(ctrl[0]);
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);
  Wire.write(ctrl[1]);
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);
  Wire.write(ctrl[2]);
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);

  // Write 32-bit little-endian data
  Wire.write((uint8_t)(data & 0xFF));
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);
  Wire.write((uint8_t)((data >> 8) & 0xFF));
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);
  Wire.write((uint8_t)((data >> 16) & 0xFF));
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);
  Wire.write((uint8_t)((data >> 24) & 0xFF));
  delayMicroseconds(DELAY_US_BETWEEN_BYTES);

  uint8_t result = Wire.endTransmission();
  if (result != 0) {
    Serial.print("I2C write error at 0x");
    Serial.print(address, HEX);
    Serial.print(" → error code: ");
    Serial.println(result);
  } else {
    Serial.print("Write OK to 0x");
    Serial.println(address, HEX);
  }
}

// Direction control - write to shadow register at runtime (no EEPROM burn needed)
void setDirectionCW() {
    writeRegister32(0x00AA, 0x41CC5C00); // DIR_INPUT=1, clockwise
}

void setDirectionCCW() {
    writeRegister32(0x00AA, 0x41D45C00); // DIR_INPUT=2, counter-clockwise
}