/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the definition file for the hard coded beeb sound for the class CPERingingTonePlayer. 
*                Beeb sound is played when no file based sounds can be played.
*
*/


#ifndef CPERINGINGTONEDEFINITION_H
#define CPERINGINGTONEDEFINITION_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION

/**
*
*  @lib audiohandling.lib
*  @since Series60_2.7
*/
class TLitPtrC8
    {
public:
    inline const TDesC8* operator&() const;
    inline operator const TDesC8&() const;
    inline const TDesC8& operator()() const;
    inline operator const __TRefDesC8() const;
public:
    TUint iTypeLength;
    const TUint8 *iPtr;
    };

// Template class TLitC8
inline const TDesC8* TLitPtrC8::operator&() const
    {
    return REINTERPRET_CAST(const TDesC8*,this);
    }
inline const TDesC8& TLitPtrC8::operator()() const
    {
    return *operator&();
    }
inline TLitPtrC8::operator const TDesC8&() const
    {
    return *operator&();
    }
inline TLitPtrC8::operator const __TRefDesC8() const
    {
    return *operator&();
    }

#define _LIT8DATA( name, data ) const static TLitPtrC8 name = { ( sizeof( data ) | ( 1 << 28 ) ), REINTERPRET_CAST( const TUint8*, data ) }

// Signature definitions
#define SEQ_SIG1 0x5153
#define SEQ_SIG2 0x434E    
#define SEQ_SIG    SEQ_SIG1,SEQ_SIG2, // Signature = "SQNC" ASCII

// Commands
#define SEQ_CMD_RET         -1
#define SEQ_CMD_STARTLOOP   -2,
#define SEQ_CMD_ENDLOOP     -3,

#endif //CPERINGINGTONEDEFINITION_H

// End of File
