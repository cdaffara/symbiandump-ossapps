/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for oled digital clock.
*
*/

#ifndef SNSROLEDDIGITALCLOCKCONTAINER_H
#define SNSROLEDDIGITALCLOCKCONTAINER_H

#include "snsrbigclockcontainer.h"


SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverPlugin)

class SnsrLabel;
class SnsrOledTimeLabel;

class SnsrOledDigitalClockContainer : public SnsrBigClockContainer
{
    Q_OBJECT

public:

    SnsrOledDigitalClockContainer();
    ~SnsrOledDigitalClockContainer();

public slots:

    virtual void update();
    void updatePosition();

public: // from base classes

    virtual int updateIntervalInMilliseconds();
    virtual Screensaver::ScreenPowerMode displayPowerMode();
    virtual void getActiveScreenRows(int *firstActiveRow, int *lastActiveRow);
    virtual bool isOrientationLocked();
    
protected: // from base classes

    virtual void loadWidgets();

private:

    QGraphicsWidget *mClockContainer;

    SnsrLabel *mAmPmLabel;
    SnsrOledTimeLabel *mTimeLabel;
    SnsrLabel *mDateLabel;
    
    QPointF mDestPosition;
    bool mInitialize;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverPlugin)
};

#endif // SNSROLEDDIGITALCLOCKCONTAINER_H
