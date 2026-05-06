#ifndef PLAYTIME_TIMER_H
#define PLAYTIME_TIMER_H

typedef struct PlaytimeTimer
{
    int timer;
    int secondsOnesPlace;
    int secondsTensPlace;
    int minutesOnesPlace;
    int minutesTensPlace;
    int hoursOnePlace;
    int hoursTensPlace;

}PlaytimeTimer;

void FramesToPlaytimeTimer(PlaytimeTimer* ptr_timer);

void PlaytimeTimerString(PlaytimeTimer* ptr_timer, char* ascii);


#endif /* PLAYTIME_TIMER_H */
