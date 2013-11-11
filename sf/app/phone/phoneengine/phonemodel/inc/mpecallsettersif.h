/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the interface for setters needed needed for dialling a phone call.
*
*/

#ifndef MPECALLSETTERSIF_H
#define MPECALLSETTERSIF_H

// INCLUDES
#include <pevirtualengine.h>

// CLASS DECLARATION

/**
*  Call for setting needed parameters before making a phone call.
* 
*  @lib phoneengine.lib
*  @since S60_10.1
*/
NONSHARABLE_CLASS( MPECallSettersIF ) 
{
    public:    //New functions

        /**
        * Sets key code value to TPEBasicInfo-structure
        * @param aKeyCode is the key code value.
        * @return None.
        */
        virtual void SetKeyCode( const TChar& aKeyCode ) = 0; 

        /**
        * Sets phone number to TPECallCommandInfo-structure
        * @param aPhoneNumber is the phone number
        * @return None.
        */
        virtual void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber ) = 0;

        /**
        * Sets call type information for following command message
        * Used for specifying the call type of the next attempted call
        * @param aCallType is the call type of the next attempted call
        */
        virtual void SetCallTypeCommand( const TPECallType& aCallType ) =  0;
        
        /**
        * Sets call Phonebook contact identifier.
        * @param aContactId is the contact identifier for a call
        */
        virtual void SetContactId2( const TInt aContactId ) = 0;
        
        /**
        * Sets service identifier.
        * @param aServiceId service id
        */
        virtual void SetServiceIdCommand( TUint32 aServiceId ) = 0;

    };

#endif // MPECALLSETTERSIF_H

// End of File
