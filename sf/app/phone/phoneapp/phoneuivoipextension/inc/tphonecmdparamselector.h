/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*   See class description.
*
*/

#ifndef TPHONECMDPARAMSELECTOR_H
#define TPHONECMDPARAMSELECTOR_H

//  INCLUDES
#include "tphonecommandparamvoip.h"

// DATA TYPES

/**
* Phone number types
*
* EPhoneNumberTypeNotFound - phone number's type was not found
* EPhoneNumberTypeCS - CS call can be made to the phone number
* EPhoneNumberTypeVideo - video call can be made to the phone number
* EPhoneCallTypeVoip - voip call can be made to the number/address
*/
enum TPhoneSsCallType
{
    EPhoneCallTypeNotFound = -1,
    EPhoneCallTypeCS = 0,
    EPhoneCallTypeVideo,
    EPhoneCallTypeVoip
};

// CLASS DECLARATION

/**
*  A service selection parameter class.
*  
*/
class TPhoneCmdParamSelector : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamSelector();

    public: 
        
        /**
        * Sets the service id.
        * @param aServiceId Service id
        */
        IMPORT_C void SetServiceId( const TUint aServiceId );

        /**
        * Returns the service id.
        * @return Returns the service id.
        */
        IMPORT_C TUint ServiceId();
        
        /**
        * Sets the call type.
        * @param aCallType call type
        */
        IMPORT_C void SetCallType( TPhoneSsCallType aCallType );
        
        /**
        * Returns the call type.
        * @return phone call type
        */
        IMPORT_C TPhoneSsCallType CallType();
        
        /**
        * Sets the registration status.
        * @param aEnabled registration status to be set.
        */
        IMPORT_C void SetRegStatus( TBool aEnabled );
        
        /**
        * Returns the registration status.
        * @return ETrue if service registered.
        */
        IMPORT_C TBool RegStatus();


    private:   

        /**
        * The service id
        */
        TUint iServiceId;
        
        /**
        * The call type
        */
        TPhoneSsCallType iCallType;
        
        /**
        * Registration status
        */
        TBool iEnabled;
    };

#endif      // TPHONECMDPARAMSELECTOR_H   
// End of File
