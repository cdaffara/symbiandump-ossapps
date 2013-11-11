/*
* Copyright (c) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test code
*
*/



#include <QtTest/QtTest>

#ifdef Q_OS_SYMBIAN
#include "t_hsclockwidget.h"
#include "hsclocksettingsnotifier_symbian.h"

#include <bacntf.h> // CEnvironmentChangeNotifier


namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";
    const char TIME12[] = "TIME12";
    const char TIME24[] = "TIME24";
}


void TestClockWidget::testSettingOfClockAndTimeFormat()
    {
    HsClockSettingsNotifier clockSettingsNotifier;
    
    QVERIFY(clockSettingsNotifier.mDateTimeNotifier);
    QVERIFY(clockSettingsNotifier.mDateTimeNotifier->IsActive());
    
    TLocale locale;
    
    // ANALOG, TIME12
    locale.SetClockFormat(EClockAnalog);
    locale.SetTimeFormat(ETime12);
    locale.Set();
    qApp->processEvents();
    
    TClockFormat clockFormat = locale.ClockFormat();
    TTimeFormat timeFormat = locale.TimeFormat();

    QCOMPARE( clockSettingsNotifier.clockFormat(), QString(ANALOG));
    QCOMPARE( clockSettingsNotifier.timeFormat(), QString(TIME12));

    // ANALOG, TIME24
    locale.SetClockFormat(EClockAnalog);
    locale.SetTimeFormat(ETime24);
    locale.Set();
    qApp->processEvents();
    
    clockFormat = locale.ClockFormat();
    timeFormat = locale.TimeFormat();
        
    QCOMPARE( clockSettingsNotifier.clockFormat(), QString(ANALOG));
    QCOMPARE( clockSettingsNotifier.timeFormat(), QString(TIME24));
    
    // DIGITAL, TIME24
    locale.SetClockFormat(EClockDigital);
    locale.SetTimeFormat(ETime24);
    locale.Set();
    qApp->processEvents();
    
    clockFormat = locale.ClockFormat();
    timeFormat = locale.TimeFormat();
        
    QCOMPARE( clockSettingsNotifier.clockFormat(), QString(DIGITAL));
    QCOMPARE( clockSettingsNotifier.timeFormat(), QString(TIME24));

    // DIGITAL, TIME12
    locale.SetClockFormat(EClockDigital);
    locale.SetTimeFormat(ETime12);
    locale.Set();
    qApp->processEvents();
    
    clockFormat = locale.ClockFormat();
    timeFormat = locale.TimeFormat();
        
    QCOMPARE( clockSettingsNotifier.clockFormat(), QString(DIGITAL));
    QCOMPARE( clockSettingsNotifier.timeFormat(), QString(TIME12));

    clockSettingsNotifier.createObserver();
    QVERIFY(clockSettingsNotifier.mDateTimeNotifier);
    QVERIFY(clockSettingsNotifier.mDateTimeNotifier->IsActive());
    }



#endif

// end of file
