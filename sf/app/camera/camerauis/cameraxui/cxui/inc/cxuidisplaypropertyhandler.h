/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef CXUIDISPLAYPROPERTYHANDLER_H
#define CXUIDISPLAYPROPERTYHANDLER_H

#include <QObject>
#include <QTimer>
#include <XQUtils>

#include "cxeviewfindercontrol.h"

/**
 * CxuiDisplayPropertyHandler
 *
 * Utility class for display visibility settings.
 * Can be used for enabling / disabling screensaver and
 * setting backlight constantly on or to normal timeout mode.
 */
class CxuiDisplayPropertyHandler : public QObject
{
    Q_OBJECT

public:

    CxuiDisplayPropertyHandler();
    virtual ~CxuiDisplayPropertyHandler();

    void setDisplayAlwaysVisible(bool visible);

protected slots:

    void simulateActivity();
    void handleVfStateChanged(CxeViewfinderControl::State newState, CxeError::Id error);

private: // Data

    QTimer mBacklightTimer;
    XQUtils mUtility;
};

#endif // CXUIDISPLAYPROPERTYHANDLER_H
