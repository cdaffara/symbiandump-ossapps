
#ifndef QPROCESS_H
#define QPROCESS_H

class QProcess
{    
public:
    static bool startDetached(const QString &program){ QProcess::isRunning = true; } ;
    static bool isRunning;
};

#endif // QPROCESS_H