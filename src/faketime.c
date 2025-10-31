#include "global.h"
#include "faketime.h"

COMMON_DATA struct Time gLocalTime = {0};

void
FakeTimeLoad(void)
{
    AddTimes(&gLocalTime,
             &gSaveBlock2Ptr->lastBerryTreeUpdate,
             &gSaveBlock2Ptr->localTimeOffset);
}

void
FakeTimeSave(void)
{
    CalcTimeDifference(&gSaveBlock2Ptr->localTimeOffset,
                       &gSaveBlock2Ptr->lastBerryTreeUpdate,
                       &gLocalTime);
}

void
FakeTimeReset(s8 hours, s8 minutes)
{
    gLocalTime.days = 0;
    gLocalTime.hours = hours;
    gLocalTime.minutes = minutes;
    gLocalTime.seconds = 0;
    gSaveBlock2Ptr->lastBerryTreeUpdate = gLocalTime;
    FakeTimeSave();
}

void
FakeTimeTick(void)
/* Ticks exactly one second of in-game time. */
{
    gLocalTime.seconds++;
    FixTime(&gLocalTime);
}

void
FakeTimeAdvanceDayNight(void)
{
    if (gLocalTime.hours < 6) {
        gLocalTime.hours = 6;
        gLocalTime.minutes = 0;
        gLocalTime.seconds = 0;
    } else if (gLocalTime.hours < 18) {
        gLocalTime.hours = 18;
        gLocalTime.minutes = 0;
        gLocalTime.seconds = 0;
    } else {
        gLocalTime.days++;
        gLocalTime.hours = 6;
        gLocalTime.minutes = 0;
        gLocalTime.seconds = 0;
    }
}

void
FixTime(struct Time *time)
{
    if (time->seconds > SECONDS_PER_MINUTE) {
        time->seconds -= SECONDS_PER_MINUTE;
        time->minutes++;
    }
    if (time->minutes > MINUTES_PER_HOUR) {
        time->minutes -= MINUTES_PER_HOUR;
        time->hours++;
    }
    if (time->hours > HOURS_PER_DAY) {
        time->hours -= HOURS_PER_DAY;
        time->days++;
    }
}

void
AddTimes(struct Time *result, struct Time *t1, struct Time *t2)
{
    result->seconds = t2->seconds + t1->seconds;
    result->minutes = t2->minutes + t1->minutes;
    result->hours = t2->hours + t1->hours;
    result->days = t2->days + t1->days;
    FixTime(result);
}

// Lifted directly from rtc.c.
void
CalcTimeDifference(struct Time *result, struct Time *t1, struct Time *t2)
{
    result->seconds = t2->seconds - t1->seconds;
    result->minutes = t2->minutes - t1->minutes;
    result->hours = t2->hours - t1->hours;
    result->days = t2->days - t1->days;

    if (result->seconds < 0)
    {
        result->seconds += SECONDS_PER_MINUTE;
        --result->minutes;
    }

    if (result->minutes < 0)
    {
        result->minutes += MINUTES_PER_HOUR;
        --result->hours;
    }

    if (result->hours < 0)
    {
        result->hours += HOURS_PER_DAY;
        --result->days;
    }
}

