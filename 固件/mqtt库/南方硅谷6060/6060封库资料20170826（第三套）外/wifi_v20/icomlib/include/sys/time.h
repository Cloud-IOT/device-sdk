#ifndef __TIME_H__
#define __TIME_H__

struct icommtm {
  int tm_mday;   // Day of the month [1, 31]
  int tm_mon;    // Months since January [0, 11]
  int tm_year;   // Years since 1900
};

struct aliyuntm {
    struct icommtm tm;
    int tm_wday;
    int tm_hour;
    int tm_min;
    int tm_sec;
};

void icomm_localtime(unsigned long *timer, struct icommtm *tmbuf);
void aliyun_time(unsigned long *timer, struct aliyuntm *tmbuf);

#endif
