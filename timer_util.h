#pragma once

#define TIMER_PERIODIC  (1<<0)
typedef void (*timer_cb)(void *usp);
void *isync_timer_start(int ms, int flags, timer_cb cb, void *usp);
void isync_timer_delete(void *ptr);
void isync_timer_cleanup(void);
int isync_timer_init(void);
