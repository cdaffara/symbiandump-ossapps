/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   ?Description
*
*/

#include <glxperformancelog.h>

#define GLX_LOG_PRINT_ITERATION 20
QFile GlxPerformanceLog::mLogFile;
qint16 GlxPerformanceLog::mObjectCounter = 0;

GlxPerformanceLog::GlxPerformanceLog(char *str) : mSpendTime(0), mTotalTime(0), mIteration(0), mInfoMsg (str)
{ 
    mTime.start();
    ++mObjectCounter;
    if( GlxPerformanceLog::mLogFile.isOpen() == FALSE) {
	    GlxPerformanceLog::mLogFile.setFileName("glxperlog.txt");
	    QDir::setCurrent("c:\\data\\");
	    if(GlxPerformanceLog::mLogFile.open(QIODevice::Append)) {
			 GlxPerformanceLog::mLogFile.setTextModeEnabled(TRUE);
		}
    }
}

void GlxPerformanceLog::start()
{
    mTime.restart();
}

void GlxPerformanceLog::stop()
{
    mSpendTime = mTime.elapsed();
    mTotalTime += mSpendTime;
    ++mIteration;
    if ( (mIteration % GLX_LOG_PRINT_ITERATION )== 1) {
        appendPerformanceLog();
    }
}

void GlxPerformanceLog::end()
{
    appendPerformanceLog();
    mSpendTime = mTotalTime = mIteration = 0;
}

void GlxPerformanceLog::done(char *msg )
{
    mSpendTime = mTime.elapsed();
    mTotalTime += mSpendTime;
    ++mIteration;
    appendPerformanceLog(msg);
    mSpendTime = mTotalTime = mIteration = 0;
}

qreal GlxPerformanceLog::getAverageTime()
{
    if( mIteration == 0) {
        return -1;
    }
    return ( (mTotalTime + 0.0) / mIteration );
}

quint32 GlxPerformanceLog::getTime()
{
    return mSpendTime;
}

void GlxPerformanceLog::appendPerformanceLog(char *msg)
{
    char logText[255] = {0};
    sprintf(logText, "%s : %s last time %u : average time : %f iteration %u \n", mInfoMsg, msg, mSpendTime, getAverageTime(), mIteration);
    GlxPerformanceLog::mLogFile.write(logText);
}

void GlxPerformanceLog::appendTimeStamp(char *msg)
{
    char logText[255] = {0};
    QTime t = QTime::currentTime();
    sprintf(logText, "%s : Time Stamp %d:%d:%d:%d \n", msg, t.hour(), t.minute(), t.second(), t.msec());
    GlxPerformanceLog::mLogFile.write(logText);
}

GlxPerformanceLog::~GlxPerformanceLog()
{
    --mObjectCounter;
    if ( mObjectCounter == 0) {
        GlxPerformanceLog::mLogFile.close();
    }
}



