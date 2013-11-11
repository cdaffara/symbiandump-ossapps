/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef TPHONECMDPARAMSPEEDDIAL_H
#define TPHONECMDPARAMSPEEDDIAL_H

//  INCLUDES
#include "tphonecommandparam.h"

// DATA TYPES

/**
* Phone number types
*
* EPhoneNumberTypeNotFound - phone number's type was not found
* EPhoneNumberTypeCS - CS call can be made to the phone number
* EPhoneNumberTypeVideo - video call can be made to the phone number
* EPhoneNumberTypeVoip - VoIP call can be made to the address.
*/
enum TPhoneNumberType
{
    EPhoneNumberTypeNotFound = -1,
    EPhoneNumberTypeCS = 0,
    EPhoneNumberTypeVideo,
    EPhoneNumberTypeVoip
};

// CLASS DECLARATION

/**
*  A concrete parameter class for one TDesC and TInt value.
*  
*/
class TPhoneCmdParamSpeedDial : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamSpeedDial();

    public: 
        
        /**
        * Sets the phone number.
        * @param aPhoneNumber is phone number in string format
        */
        IMPORT_C void SetPhoneNumber( TPtr* aPhoneNumber );

        /**
        * Returns the phone number.
        * @return Returns the carried phone number string
        */
        IMPORT_C TPtr* PhoneNumber();
        
        /**
        * Sets the phone number's type.
        * @param aNumberType number type
        */
        IMPORT_C void SetNumberType( TPhoneNumberType aNumberType );
        
        /**
        * Returns the phone number's type.
        * @return phone number's type
        */
        IMPORT_C TPhoneNumberType NumberType();
        
        /**
        * Sets the speed dial location.
        * @param aLocation location to be set
        */
        IMPORT_C void SetLocation( TInt aLocation );
        
        /**
        * Returns the speed dial location.
        * @return values from 1 to 9 for succesfull location.
        *         Values below 0 indicate unsuccesfull location fetch.
        */
        IMPORT_C TInt Location();

        /**
        * Sets the serviceId
        * @param aServiceId The serviceId user has selected for speeddial
        */
        IMPORT_C void SetServiceId( TUint aServiceId ); 

        /**
        * Returns the serviceId
        * @param aServiceId serviceId
        */
        IMPORT_C TUint ServiceId() const;

    private:   

        /**
        * The phone number
        */
        TPtr* iPhoneNumber;
        
        /**
        * The phone number's type
        */
        TPhoneNumberType iNumberType;
        
        /**
        * Speed dial location
        */
        TInt iLocation;
        /**
        * The serviceId
        */
        TUint iServiceId;
    };

#endif      // TPHONECMDPARAMSPEEDDIAL_H   
            
// End of File
