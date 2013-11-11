/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for creating Telephony Device Mode related classes. 
*
*/



#ifndef CTELDMHANDLER_H
#define CTELDMHANDLER_H

#include    <e32base.h>

// FORWARD DECLARATIONS

class MPhoneDeviceModeObserver;
class CTelDMGripHandler;

// CLASS DECLARATION
 

/**
*  Main class for creating Telephony Device Mode classes.
*
*  @since Series60 3.0
*/
class CTelDMHandler : public CBase
    { 
    public: // New functions

        /**
        * Set call event observer
        * @return void
        */
        virtual void SetObserverL( MPhoneDeviceModeObserver* aCallHandler ) = 0;
       
    };

/**
* Creates CTelDMFactory. Position 1 in DLL's lookup table.
* @return Reference to CTelDMFactory object. Ownership is transferred.
*/
IMPORT_C CTelDMHandler* CTelDMHandlerCreateL();
 
 
#endif      // CTELDMHANDLER_H  

// End of File
