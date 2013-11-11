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
* Description: Header for server side session object of
*              Screensaver Display Control Server 
*
*/

#ifndef C_CSNSRDISPLAYCONTROLSESSION_H
#define C_CSNSRDISPLAYCONTROLSESSION_H

#include <e32base.h>

class CPowerSaveDisplayMode;


/**
 *  Server side session object for Screensaver Display Control Server
 */
class CSnsrDisplayControlSession : public CSession2
    {
public:

    IMPORT_C static CSnsrDisplayControlSession* NewL();
    virtual ~CSnsrDisplayControlSession();

public: // from CSession2
    void ServiceL( const RMessage2& aMessage );
    
private:

    CSnsrDisplayControlSession();
    void ConstructL();

private: // data

    CPowerSaveDisplayMode* iPowerSave;
    HBufC16* iPowerSavePixelBuffer;
    };

#endif // C_CSNSRDISPLAYCONTROLSESSION_H
