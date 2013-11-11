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
* Description: 
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMEMERGENCYCALLHEADERDATA_H
#define __TPHONECMDPARAMEMERGENCYCALLHEADERDATA_H

//  INCLUDES

#include <w32std.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for emergency call header information.
*/
class TPhoneCmdParamEmergencyCallHeaderData : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamEmergencyCallHeaderData();

    public: 
        
        /**
        * Sets the call header label text.
        * @param aLabelText is the label
        */
        IMPORT_C void SetLabelText( const TDesC& aLabelText );

        /**
        * Sets the call header text
        * @param aHeaderText is the short label
        */
        IMPORT_C void SetHeaderText( const TDesC& aHeaderText );

        /**
        * Sets the call header voice privacy status.
        * @param aCiphering is ETrue if voice privacy is on.
        */
        IMPORT_C void SetCiphering( TBool aCiphering );

        /**
        * Sets the call header own phone number text
        * @param aOwnNumberText is the user's phone number
        */
        IMPORT_C void SetOwnPhoneNumberText( 
            const TDesC& aOwnPhoneNumberText );

        /**
        * Returns the call header label text
        * @return Returns the label
        */
        IMPORT_C const TDesC& LabelText() const;

        /**
        * Returns the call header text.
        * @return Returns the header
        */
        IMPORT_C const TDesC& HeaderText() const;

        /**
        * Returns the call header ciphering status.
        * @return Returns ETrue if ciphering is on 
        */
        IMPORT_C TBool Ciphering() const;

        /**
        * Returns the own phone number text.
        * @return Returns the own phone number text
        */
        IMPORT_C const TDesC& OwnPhoneNumberText() const;

        /**
        * Sets the ciphering indicator allowed or hidden
        * @param aAllowed EFalse to prevent ciphering indicator from showing
        *                 ETrue to enable
        */
        IMPORT_C void SetCipheringIndicatorAllowed( TBool aAllowed );
        
        /**
        * Returns the ciphering indicator allowed status
        * @return Returns EFalse when ciphering indicator is wanted to be disabled
        *         ETrue to be allowed
        */
        IMPORT_C TBool CipheringIndicatorAllowed() const;
        
    private:    
        
        /**
        * Call header label text
        */
        TPtrC iLabelText;

        /**
        * Call header text
        */
        TPtrC iHeaderText;

        /**
        * Call header ciphering status
        */
        TBool iCiphering;

        /**
        * Call header own phone number Text
        */
        TPtrC iOwnPhoneNumberText;

        /**
        * Should the ciphering indicator be visible or not
        */
        TBool iCipheringIndicatorAllowed;
        
    };

#endif // __TPHONECMDPARAMEMERGENCYCALLHEADERDATA_H   
            
// End of File
