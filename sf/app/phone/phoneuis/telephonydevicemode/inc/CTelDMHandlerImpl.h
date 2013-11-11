/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation class for for Telephony Device Mode
*
*/



#ifndef CTELDMHANDLERIMPL_H
#define CTELDMHANDLERIMPL_H

#include    <e32base.h>
#include    <cteldmhandler.h>
#include    <bldvariant.hrh>

// FORWARD DECLARATIONS

class MPhoneDeviceModeObserver;
class CTelDMCallStateListener;
class CTelDMGripHandler;
class CTelDMCommandHandler;
class CTelDMAudioAccessoryListener;
class MTelDMAccessory;


// CLASS DECLARATION
 
 
NONSHARABLE_CLASS( CTelDMHandlerImpl ) : public CTelDMHandler
    {
    public: // Constructors and destructors.

        /**
        * Two-phased constructor.
        *
        * @return new instance of the class.
        */
        static CTelDMHandlerImpl* NewL();
        
        /** D'tor. */
        virtual ~CTelDMHandlerImpl();

    public: // From base classes.

        /**
        * Set call event observer
        * @return void
        */
        void SetObserverL( MPhoneDeviceModeObserver* aCallHandler );
 
    private:

        /**
        * C++ constructor.
        */
        CTelDMHandlerImpl();
        
        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
    
    
    private:  
    
        /**
        * Grip handler, Owned 
        */
        CTelDMGripHandler* iGrip;
        
        /**
        * Accessory status, Owned 
        */
        CTelDMAudioAccessoryListener* iAccessory;
         
        /**
        * Call state listener, Owned 
        */
        CTelDMCallStateListener* iCallStateListener;

        /**
        * Commander, Owned 
        */      
        CTelDMCommandHandler* iCommandHandler;

    };


#endif      // CTELDMHANDLERIMPL_H  

// End of File
