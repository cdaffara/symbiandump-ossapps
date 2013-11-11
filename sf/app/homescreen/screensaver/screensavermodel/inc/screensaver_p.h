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
* Description:  The private implementation of the Screensaver.
*
*/

#ifndef SCREENSAVER_P_H
#define SCREENSAVER_P_H

#include "screensaver.h"

class QGraphicsWidget;

class ScreensaverPrivate
{
public:

    ScreensaverPrivate(Screensaver * const screensaverPublic);
    ~ScreensaverPrivate();

    ScreensaverState currentState();

public slots:

    void initialize();
    void foreground();
    void partialForeground();
    void background();
    void powerSave();
    void close();
    void handleActiveIndicators(const QList<HbIndicatorInterface*> &activeIndicators);
    void handleActivatedIndicator(HbIndicatorInterface *activatedIndicator);
    void handleDeactivatedIndicator(HbIndicatorInterface *deactivatedIndicator);

private:

    Q_DISABLE_COPY(ScreensaverPrivate)

public:

    Screensaver * const m_q;
    ScreensaverState mState;
};

#endif // SCREENSAVER_P_H
