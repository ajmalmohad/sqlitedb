#ifndef PAGER_H
#define PAGER_H

#include "constants.h"

void *get_page(Pager *pager, uint32_t page_num);
Pager *pager_open(const char *filename);
void pager_flush(Pager *pager, uint32_t page_num);

#endif