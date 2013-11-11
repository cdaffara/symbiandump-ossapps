/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Phone Keys
*
*/


// INCLUDE FILES
#include    "cphonekeys.h"
#include    "phonelogger.h"

// CONSTANTS
_LIT( KPhoneValidChars, "0123456789*#+pwPW" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneKeys::IsNumberKey
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::IsNumberKey(
    const TKeyEvent& aKeyEvent,
    TEventCode /*aType*/ )
    {
    // Convert key code to western.
    TBuf<1> buffer; // one character
    buffer.Append( aKeyEvent.iCode );
   
    TBool result = EFalse;

    switch ( buffer[ 0 ] ) // first char
        {
        // Digits, 0 .. 9
        case KPhoneDtmf0Character:
        case KPhoneDtmf1Character:
        case KPhoneDtmf2Character:
        case KPhoneDtmf3Character:
        case KPhoneDtmf4Character:
        case KPhoneDtmf5Character:
        case KPhoneDtmf6Character:
        case KPhoneDtmf7Character:
        case KPhoneDtmf8Character:
        case KPhoneDtmf9Character:
            result = ETrue;
            break;
        default:
            break;
        }
__PHONELOG1( 
        EBasic, 
        EPhonePhoneapp, 
        "PhoneKeys::IsNumberKey: result: %d", 
        result );
        
    return result;
    }

// ---------------------------------------------------------
// CPhoneKeys::IsNumericKey
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::IsNumericKey( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TBool result = IsNumberKey( aKeyEvent, aType );

    if ( !result )
        {
        switch( aKeyEvent.iCode )
            {
            case KPhoneDtmfHashCharacter:      // Hash
            case KPhoneDtmfStarCharacter:      // Asterisk
                // the following key can be got via taps of *
            case KPhoneDtmfPauseCharacter:     // DTMF Soft-Pause character
            case KPhoneDtmfWaitCharacter:      // DTMF Wait character
                result = ETrue;
                break;
            default:
                break;
            }
        }

__PHONELOG2( 
        EBasic, 
        EPhonePhoneapp, 
        "PhoneKeys::IsNumericKey: result: %d, code %d", 
        result,
        aKeyEvent.iCode );

    return result;
    }       

// ---------------------------------------------------------
// CPhoneKeys::IsEscapeKey
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::IsEscapeKey(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return 
        ( aType == EEventKey ) && 
        ( aKeyEvent.iCode == EKeyEscape );
    }

// ---------------------------------------------------------
// CPhoneKeys::IsDTMFTone
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::IsDtmfTone( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TBool result = IsNumberKey( aKeyEvent, aType );

    if ( !result )
        {
        switch( aKeyEvent.iCode )
            {
            case KPhoneDtmfHashCharacter:  // Hash
            case KPhoneDtmfStarCharacter:  // Asterisk
                result = ETrue;
                break;
            default:
                break;
            }
        }

__PHONELOG2( 
        EBasic, 
        EPhonePhoneapp, 
        "PhoneKeys::IsDtmfTone: result: %d, code %d", 
        result,
        aKeyEvent.iCode );
        
    return result;
    }

// ---------------------------------------------------------
// CPhoneKeys::IsSecondHashKey
// Checks if keyevent is Second "#" key, i.e, "##".
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::IsSecondHashKey(
    const TDes& aText )
    {
    TInt length = aText.Length();
    TBool result = EFalse;

    // * or # character entered
    if ( ( length >= KSecondKeyEvent ) 
        && ( aText[0] == KPhoneDtmfHashCharacter ) 
        && ( aText[1] == KPhoneDtmfHashCharacter ) )
        {
        result = ETrue;
        }
    
    __PHONELOG1( 
        EBasic, 
        EPhonePhoneapp, 
        "PhoneKeys::IsSecondHashKey: result: %d", 
        result );
        
    return result;
    }

// -----------------------------------------------------------------------------
// PhoneKeys::IsExtraChar
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::IsExtraChar(
        TInt aChar )
    {
    TBool result = EFalse;
    switch( aChar )
        {
        case '*': // Asterisk
            // the following key can be got via taps of *
        case '+': // Plus
        case 'p': // P character
        case 'w': // W character
            result = ETrue;
            break;
        default:
            break;
        }

    return result;
    }

// ---------------------------------------------------------
// CPhoneKeys::Validate
//
// Go through the whole string and validate each character.
// If character is invalid, check if we have reached end of
// string (in which case, input.Peek() returns 0).
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneKeys::Validate( const TDesC& aString )
    {
    TLex input( aString );
    TPtrC valid( KPhoneValidChars );

    while ( valid.Locate( input.Peek() ) != KErrNotFound )
        {
        input.Inc();
        }
    
    return !input.Remainder().Length();
    }

//  End of File  
