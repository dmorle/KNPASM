#ifndef KNP_UTIL_H
#define KNP_UTIL_H

#include <stdint.h>
#include <stdbool.h>

#include <knpasm/knpcore.h>

bool iswhitespace(char c);
bool isalphanumeric(char c);
bool ishex(char c);
uint8_t hctoi(char c);
void touppercase(char* str);
size_t readline(char** pbuf, size_t n);

void printknpi(KNP_INSTRUCTION knpi);

#endif
