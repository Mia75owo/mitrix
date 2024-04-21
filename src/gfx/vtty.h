#ifndef VTTY_H_
#define VTTY_H_

#include "util/types.h"

void vtty_init();
void vtty_render();
void vtty_render_last_line(u32 from);

#endif
