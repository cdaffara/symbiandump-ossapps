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
#include <e32const.h>
#endif

// forward declaration
class HsClockWidget;

/**
* @test Implementation for the clock plugin test code
*/
class TestClockWidget : public QObject
{
    Q_OBJECT

    private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // hsclockwidget.cpp tests
    void createClockWidget();
    void toggleAnalogDigital();

    void testClockOnInitalize();
    void testClockOnShow();
    void testClockOnHide();
    void testOnUninitialize();
    void testOnSettingsChanged();
    void testClockShape();

    // hsanalogclockwidget.cpp tests    
    void createAnalogClockWidget();
    void analogClockWidgetTick();
    void analogClockWidgetPolish();
    void analogClockWidgetShape();
    
    // hsdigitalclocklabel.cpp tests    
    void createDigitalClockWidget();
    void digitalClockWidgetTick();
    void digitalClockWidgetPolish();
    void digitalClockWidgetShape();

    // hsclockwidgettimer.cpp
    void testHsClockWidgetTimer();
    
#ifdef Q_OS_SYMBIAN    
    // hsclocksettingsnotifier_symbian.cpp tests
    void testSettingOfClockAndTimeFormat();
#endif

private:

    HsClockWidget *mClockWidget;

#ifdef Q_OS_SYMBIAN
    TClockFormat mClockFormat;
    TTimeFormat mTimeFormat;
#endif
    
    };


