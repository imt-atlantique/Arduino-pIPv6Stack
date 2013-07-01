#ifndef __ARDUINODEBUG_H__
#define __ARDUINODEBUG_H__

#define ARDUINO_DEBUG 1

  extern "C" {
extern void arduino_debug_init();
extern void arduino_debug(char* s);
extern void arduino_debug_hex(int b);
  }

#endif /* __ARDUINODEBUG_H__ */
