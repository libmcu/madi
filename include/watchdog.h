#ifndef WATCHDOG_H
#define WATCHDOG_H

void watchdog_init(void);
int watchdog_alloc(void);
void watchdog_feed(int id);

#endif
