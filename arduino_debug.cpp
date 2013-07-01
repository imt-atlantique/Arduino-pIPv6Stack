#include <HardwareSerial.h>
#include "arduino_debug.h"

void arduino_debug_init() {
    Serial.begin(9600);
}

void arduino_debug(char* s) {
    Serial.println(s);
}

void arduino_debug_hex(int b) {
    Serial.println(b, HEX);
}
    

    
    
