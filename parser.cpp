#include "parser.h"
#include "status_led.h"
#include <string.h>

Parser::Parser() {
  queue = NULL;
  led = NULL;
}

void Parser::init() {
  queue = xQueueCreate(5, sizeof(Command));
  if(queue == NULL) {
    if(led != NULL)
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
  command.value = 0;
  char* c_type = strtok(cline, " ");
  if(strlen(c_type) != 1) {
    return command;
  }
  char* value = strtok(NULL, " ");
  command.mode = c_type[0];
  command.value = atoi(value);
  return command;
}

Command Parser::getCommand() {
  Command rxcomm;
  if(xQueueReceive(queue, &rxcomm, 100/portTICK_PERIOD_MS) == pdPASS) {
    return rxcomm;
  }
}
