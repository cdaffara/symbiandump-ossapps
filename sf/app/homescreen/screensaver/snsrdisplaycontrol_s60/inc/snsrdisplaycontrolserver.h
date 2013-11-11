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
* Description: Header of Screensaver Display Control Server
*
*/

#ifndef C_CSNSRDISPLAYCONTROLSERVER_H
#define C_CSNSRDISPLAYCONTROLSERVER_H

#include <e32base.h>

/**
 *  Screensaver Display Control Server
 *  A server to act as capability proxy to allow controlling display power save
 *  mode from dialog plugin running in HbDeviceDialogServer process. That process
 *  has no WriteDeviceDialog capability and can't call CPowerSaveDisplayMode
 *  platform API directly.
 */
class CSnsrDisplayControlServer : public CServer2
    {
public:

    static CSnsrDisplayControlServer* NewLC();
    virtual ~CSnsrDisplayControlServer();

private: // from CServer2
    CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
    
private:

    CSnsrDisplayControlServer();
    void ConstructL();

private: // data

    };

#endif // C_CSNSRDISPLAYCONTROLSERVER_H
