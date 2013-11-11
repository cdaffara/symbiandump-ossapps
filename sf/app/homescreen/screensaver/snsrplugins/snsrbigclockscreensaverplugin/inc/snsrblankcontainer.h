/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for blank screen.
*
*/

#ifndef SNSRBLANKCONTAINER_H
#define SNSRBLANKCONTAINER_H

#include "snsrbigclockcontainer.h"

//SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverPlugin)

class SnsrBlankContainer : public SnsrBigClockContainer
{
    Q_OBJECT

public:

    SnsrBlankContainer();
    ~SnsrBlankContainer();

public slots:

    virtual void update();

public: // from base classes

    virtual Screensaver::ScreenPowerMode displayPowerMode();
    virtual int updateIntervalInMilliseconds();

protected: // from base classes

    virtual void loadWidgets();

private:

//    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverPlugin)
};

#endif // SNSRBLANKCONTAINER_H
