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
* Description:  Implementation of ECOM plug-in service interface. Provides
*                key event service.
*
*/


#ifndef CHTIKEYEVENTSERVICEPLUGIN_H
#define CHTIKEYEVENTSERVICEPLUGIN_H

// INCLUDES
#include <HtiServicePluginInterface.h>
#include <w32std.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>


// CONSTANTS
const TInt KKeyEventServiceUidValue = 0x1020DEC1; // ECOM Implementation UID
const TUid KKeyEventServiceUid = { KKeyEventServiceUidValue };

// FORWARD DECLARATIONS
class CKeyEventHandler;
class CPointerEventHandler;

// CLASS DECLARATION

/**
* Implementation of ECOM plug-in service interface.
*/
class CHtiKeyEventServicePlugin : public CHTIServicePluginInterface
                                 ,public MCenRepNotifyHandlerCallback
    {
public:

    static CHtiKeyEventServicePlugin* NewL();

    // Interface implementation
    void ProcessMessageL( const TDesC8& aMessage,
        THtiMessagePriority aPriority );

protected:

    CHtiKeyEventServicePlugin();
    void ConstructL();

    virtual ~CHtiKeyEventServicePlugin();

    // from MCenRepNotifyHandlerCallback
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );
    void HandleNotifyError( TUint32 aId, TInt error,
            CCenRepNotifyHandler* aHandler );

private:

    CKeyEventHandler* iKeyHandler;
    CPointerEventHandler* iPointerHandler;

    TInt iLightTimeout;
    CRepository* iCenRepSession;
    CCenRepNotifyHandler* iLightTimeoutSettingHandler;
    };

#endif // CHTIKEYEVENTSERVICEPLUGIN_H

