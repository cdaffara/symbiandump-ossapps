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

#ifndef CPECALLSETTERSIFMOCK_H
#define CPECALLSETTERSIFMOCK_H

// INCLUDES
#include <pevirtualengine.h>
#include "mpecallsettersif.h"

// CLASS DECLARATION

/**
*  Call for setting needed parameters before making a phone call.
* 
*  @lib phoneengine.lib
*  @since S60_10.1
*/
NONSHARABLE_CLASS( CPECallSettersIFMock ) : public CBase, public MPECallSettersIF
{
    public:    //New functions

        /**
        * Sets key code value to TPEBasicInfo-structure
        * @param aKeyCode is the key code value.
        * @return None.
        */
        void SetKeyCode( const TChar& aKeyCode ); 

        /**
        * Sets phone number to TPECallCommandInfo-structure
        * @param aPhoneNumber is the phone number
        * @return None.
        */
        void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber );

        /**
        * Sets call type information for following command message
        * Used for specifying the call type of the next attempted call
        * @param aCallType is the call type of the next attempted call
        */
        void SetCallTypeCommand( const TPECallType& aCallType );
        
        /**
        * Sets call Phonebook contact identifier.
        * @param aContactId is the contact identifier for a call
        */
        void SetContactId2( const TInt aContactId );
        
        /**
        * Sets service identifier.
        * @param aServiceId service id
        */
        void SetServiceIdCommand( TUint32 aServiceId );

    };

#endif // CPECALLSETTERSIFMOCK_H

// End of File
