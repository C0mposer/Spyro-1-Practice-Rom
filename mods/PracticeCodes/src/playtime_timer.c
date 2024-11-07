#include <common.h>
#include <playtime_timer.h>

PlaytimeTimer playtime_timer;

char global_playtime_string[15] = { 0 };

void FramesToPlaytimeTimer(PlaytimeTimer* ptr_timer)
{
    ptr_timer->hoursOnePlace = (ptr_timer->timer / 216000) % 10;
    ptr_timer->hoursTensPlace = (ptr_timer->timer / 216000) / 10;

    ptr_timer->minutesOnesPlace = (ptr_timer->timer / 3600) % 10;
    ptr_timer->minutesTensPlace = ((ptr_timer->timer / 3600) / 10) % 6;

    ptr_timer->secondsOnesPlace = (ptr_timer->timer / 60) % 10;
    ptr_timer->secondsTensPlace = ((ptr_timer->timer / 60) / 10) % 6;
}

void PlaytimeTimerString(PlaytimeTimer* ptr_timer, char* ascii) {

    sprintf(ascii, "%d%d.%d%d.%d%d", ptr_timer->hoursTensPlace, ptr_timer->hoursOnePlace, ptr_timer->minutesTensPlace, ptr_timer->minutesOnesPlace, ptr_timer->secondsTensPlace, ptr_timer->secondsOnesPlace);
}


void CalculatePlaytimeUpdate(void)
{
    playtime_timer.timer = _globalTimer;
    FramesToPlaytimeTimer(&playtime_timer);

    char playtime_string[15] = { 0 };
    PlaytimeTimerString(&playtime_timer, global_playtime_string);
}