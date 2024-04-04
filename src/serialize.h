#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "constants.h"

void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);

#endif