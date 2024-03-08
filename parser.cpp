#include "parser.h"
#include "status_led.h"
#include <string.h>

Parser::Parser() {
  queue = NULL;
  led = NULL;
}

void Parser::init() {
  queue = xQueueCreate(5, sizeof(Command));
  if(queue == NULL && led != NULL) {
    led -> setStatus(STATUS_ERR);
  }
}

void Parser::parse(char* cline) {
  char* c_type = strtok(cline, " ");
  if(strlen(c_type) != 1) {
    return;
  }
  char* value = strtok(NULL, " ");
  Command command;
  command.mode = c_type[0];
  command.value = atoi(value);
  xQueueSend(queue, &command, 100/portTICK_PERIOD_MS);
}

Command Parser::parseDirect(char *cline) {
  Command command;
  command.mode = 'S';
  command.value = -1;
  char* c_type = strtok(cline, " ");
  if(strlen(c_type) != 1) {
    return command;
  }
  char* value = strtok(NULL, " ");
  command.mode = c_type[0];
  command.value = atoi(value);
  return command;
}

MultiCommand Parser::parseMultiCommandDirect(char *cline) {
  MultiCommand command = {NULL, 0, NULL};
  char* c_str = strtok(cline, " ");
  if(strlen(c_str) < 2) {
    return command;
  }
  const int argc = atoi(strtok(NULL, " "));
  char* c_args[argc];
  for(int i=0; i<argc; i++) {
    c_args[i] = strtok(NULL, " ");
  }
  command.argc = argc;
  command.mode = c_str;
  command.argv = c_args;
  return command;
}

Command Parser::getCommand() {
  Command rxcomm;
  if(xQueueReceive(queue, &rxcomm, 100/portTICK_PERIOD_MS) == pdPASS) {
    return rxcomm;
  }
}
