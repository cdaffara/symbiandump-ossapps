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
* Description: Platform agnostic Qt wrapper class for controlling display
*              power save mode. The power save mode gets actually activated
*              only on Symbian environment; in other environments the provided
*              functions do nothing.
*              The use of Symbian display power save API has been wrapped with
*              client-server pair because HbDeviceDialogs don't have enough
*              capabilities to call that API directly.
*/

#ifndef SNSRDEVICEDIALOGDISPLAYCONTROL_H
#define SNSRDEVICEDIALOGDISPLAYCONTROL_H

#ifdef Q_OS_SYMBIAN
#include "snsrdisplaycontrolclient.h"
#endif

class SnsrDeviceDialogDisplayControl
{

public:

    SnsrDeviceDialogDisplayControl();
    ~SnsrDeviceDialogDisplayControl();

    void setDisplayFullPower();
    void setDisplayLowPower( int startRow, int endRow );
    void setDisplayOff();

private:
    
#ifdef Q_OS_SYMBIAN
    RSnsrDisplayControlClient mDisplayClient;
#endif
};

#endif // SNSRDEVICEDIALOGDISPLAYCONTROL_H
