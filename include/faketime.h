#ifndef GUARD_FAKETIME_H
#define GUARD_FAKETIME_H

#define HOURS_PER_DAY       24
#define MINUTES_PER_HOUR    60
#define SECONDS_PER_MINUTE  60

extern struct Time gLocalTime;

void FakeTimeLoad(void);
void FakeTimeSave(void);
void FakeTimeReset(s8 hours, s8 minutes);
void FakeTimeTick(void);
void FakeTimeAdvanceDayNight(void);
void FixTime(struct Time *time);
void AddTimes(struct Time *result, struct Time *t1, struct Time *t2);
void CalcTimeDifference(struct Time *result, struct Time *t1, struct Time *t2);

#endif // GUARD_FAKETIME_H
