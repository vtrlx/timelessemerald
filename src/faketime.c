#include "global.h"
#include "event_data.h"
#include "faketime.h"

COMMON_DATA struct Time gLocalTime = {0};

void
FakeTimeLoad(void)
{
    /*
     * If the RTC cannot be reset and the clock has been set, then the game is
     * being loaded from an existing Pokémon Emerald Version save file into
     * Timeless Emerald, and needs to be migrated.
     */
    if (!CanResetRTC() && FlagGet(FLAG_SYS_CLOCK_SET)) {
        /*
         * This flag goes completely unset in non-Japanese versions of Emerald,
         * which makes it perfect as a flag to note that the game save was made
         * with Timeless Emerald.
         *
         * Using this flag also allows the save to migrate from Timeless Emerald
         * back into Pokémon Emerald Version by enabling the user to reset the
         * RTC in the base game. Timeless Emerald in turn has disabled the
         * ability to reset the RTC (as it has none), so this flag will only
         * ever get cleared when migrating back and resetting the RTC.
         */
        EnableResetRTC();
        /*
         * Timeless Emerald uses this saved time for a different purpose than
         * the base game, so it must be cleared before loading.
         */
        ClearTime(&gSaveBlock2Ptr->localTimeOffset);
    }
    /*
     * Set the time to the last berry time plus the additional time elapsed
     * since the last update when the game was saved.
     */
    AddTimes(&gLocalTime,
             &gSaveBlock2Ptr->lastBerryTreeUpdate,
             &gSaveBlock2Ptr->localTimeOffset);
}

void
FakeTimeSave(void)
{
    /*
     * Save the difference between the local time and the berry time into the
     * save file, so that time doesn't "go backwards" when loading the save.
     */
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
    EnableResetRTC();
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
/* Overflows seconds into minutes, minutes into hours, and hours into days. */
{
    if (time->seconds >= SECONDS_PER_MINUTE * 2) {
        time->seconds -= SECONDS_PER_MINUTE;
        time->minutes++;
    }
    if (time->minutes >= MINUTES_PER_HOUR) {
        time->minutes -= MINUTES_PER_HOUR;
        time->hours++;
    }
    if (time->hours >= HOURS_PER_DAY) {
        time->hours -= HOURS_PER_DAY;
        time->days++;
    }
}

void
ClearTime(struct Time *time)
{
    time->seconds = 0;
    time->minutes = 0;
    time->hours = 0;
    time->days = 0;
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

