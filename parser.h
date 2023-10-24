#ifndef PARSER_H
#define PARSER_H

#include <arduino.h>
#include "status_led.h"

struct Command {
  char mode;
  int value;
};

class Parser {
  QueueHandle_t queue;
  StatusLED* led;
  public:
  void parse(char* cline);
  Command parseDirect(char* cline);
  void connectStatusLED(StatusLED* led);
  Command getCommand();
  Parser();
  void init();
};

#endif
