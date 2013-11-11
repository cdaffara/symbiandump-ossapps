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
*     It defines class that contains static helper functions.
*
*/


#ifndef CPHONEKEYS_H
#define CPHONEKEYS_H

// INCLUDES
#include    <w32std.h>
#include    "phoneconstants.h"

// CONSTANTS

// CLASS DECLARATION

/**
* It defines some helper functions.
*/
class CPhoneKeys: public CBase
    {
    public: // New functions

        /**
        * Checks if the key is a number key.
        */
        IMPORT_C static TBool IsNumberKey(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );
        
        /**
        * Checks if the key is a numeric key.
        */
        IMPORT_C static TBool IsNumericKey( 
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * Checks if keyevent is escape.
        */
        IMPORT_C static TBool IsEscapeKey(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * Checks if keyevent is DTMF tone.
        */
        IMPORT_C static TBool IsDtmfTone(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * Checks if keyevent is Second "#" key, i.e, "##".
        */
        IMPORT_C static TBool IsSecondHashKey(
            const TDes& aText );
        
        /**
        * Checks if keyevent is "*" key or following via taps of
        */    
        IMPORT_C static TBool IsExtraChar(
            TInt aChar ); 
        
        /**
        * Validates number entry string.
        *
        * String is ok if it contains only characters from 
        * set { 0, .., 9, *, #, +, p, w, P, W }.
        *
        * @param aString string to be validated.
        * @return ETrue iff string is ok.
        */
        IMPORT_C static TBool Validate( const TDesC& aString );           
    };

#endif      // CPHONEKEYS_H
            
// End of File
