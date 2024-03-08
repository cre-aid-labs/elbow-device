#ifndef PARSER_H
#define PARSER_H

#include <arduino.h>
#include "status_led.h"

struct Command {
  char mode;
  int value;
};

struct MultiCommand {
  char* mode;
  int argc;
  char** argv;
};

class Parser {
  QueueHandle_t queue;
  StatusLED* led;
  public:
  void parse(char* cline);
  bool isMultiCommand(char* cline);
  Command parseDirect(char* cline);
  MultiCommand parseMultiCommandDirect(char* cline);
  void connectStatusLED(StatusLED* led);
  Command getCommand();
  Parser();
  void init();
};

#endif
