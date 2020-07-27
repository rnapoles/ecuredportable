#ifndef SLEEPHELPER_H
#define SLEEPHELPER_H
#include <QThread>

class SleepHelper : public QThread
{
public:
    static void msleep(int ms)
    {
        QThread::msleep(ms);
    }
};

#endif // SLEEPHELPER_H

