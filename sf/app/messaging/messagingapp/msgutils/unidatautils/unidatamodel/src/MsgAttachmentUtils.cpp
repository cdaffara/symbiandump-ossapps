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
* Description: 
*
*/


// ========== INCLUDE FILES ================================

#include <txtetext.h>               // for CPlainText

#include "MsgAttachmentUtils.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

//----------------------------------------------------------------------------
// from: TBool CImRecvConvert::IsIllegalChar(const TUint aChar)
//----------------------------------------------------------------------------
LOCAL_C TBool IsIllegalChar(const TUint aChar)
    {
    return (
        aChar == '*'  ||
        aChar == '\\' ||
        aChar == '<'  ||
        aChar == '>'  ||
        aChar == ':'  ||
        aChar == '.'  ||
        aChar == '"'  ||
        aChar == '/'  ||
        aChar == '|'  ||
        aChar == '?'  ||
        aChar == CEditableText::EParagraphDelimiter  ||
        aChar == CEditableText::ELineBreak  ||
        aChar <  ' ' );
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::GetFileNameFromBuffer
//
//
// ---------------------------------------------------------
//
void MsgAttachmentUtils::GetFileNameFromBuffer(
    TFileName& aFileName,
    const TDesC& aBuffer,
    TInt aMaxLength,
    const TDesC* aExt /*= NULL*/ )
    {
    if ( aExt != NULL )
        {
        aMaxLength -= aExt->Length();
        }

    TInt len = aBuffer.Length();
    TInt max = Min( len, aMaxLength );

//    __ASSERT_DEBUG( max > 0, Panic( EMsgZeroLength ) );

    aFileName.Zero();

    TInt cc = 0;
    TUint ch;
    TUint ch1 = 0;
    TBool spaces = EFalse;
    for ( TInt i = 0; i < len && cc < max; ++i )
        {
        ch = aBuffer[i];

        // ignore spaces from beginning of the buffer until first
        // non-space is encountered.
        if ( !spaces && ch != ' ' )
            {
            spaces = ETrue;
            }

        if ( i > 0 )
            {
            ch1 = aBuffer[i - 1];
            }

        // strip illegal chars away.
        // checks also if previous and current chars are '.'
        if ( spaces && ! IsIllegalChar( ch ) )
            {
            if ( !( i > 0 && ch == '.' && ch1 == '.' ) )
                {
                aFileName.Append( ch );
                cc++;
                }
            }
        }

    aFileName.Trim();

    // If filename is empty at this point, do not append extension either.
    // Instead, empty filename is returned so that caller can use whatever
    // default s/he has for it.
    if ( aFileName.Length() > 0 && aExt != NULL )
        {
        aFileName.Append( *aExt );
        }
    }

// End of File
