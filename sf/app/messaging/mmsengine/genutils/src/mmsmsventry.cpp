/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Wrapper for MMS specials in TMsvEntry.
*
*/




// INCLUDE FILES
#include    "mmsmsventry.h"
#include    "mmsconst.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic( TMmsPanic aPanic );

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================



// ---------------------------------------------------------
// TMmsMsvEntry::TMmsMsvEntry
// ---------------------------------------------------------
//
EXPORT_C TMmsMsvEntry::TMmsMsvEntry()
    : TMsvEntry()
    {
    iMtm = KUidMsgTypeMultimedia;
    iMtmData1 = 0;
    iMtmData2 = 0;
    iMtmData3 = 0;
    iSize = 0;
    }

// ---------------------------------------------------------
// TMmsMsvEntry::TMmsMsvEntry
// Note that this does not copy the contents of iDetails and iDescriptor.
// The destructor of the original CMsvEntry entry deletes the
// buffers pointed to by the TPtrC members.
// ---------------------------------------------------------
EXPORT_C TMmsMsvEntry::TMmsMsvEntry( const TMsvEntry& aGenericEntry ) 
    : TMsvEntry()
    {
    Mem::Copy( this, &aGenericEntry, sizeof( *this ) );
    }

// ----------------------------------------------------
// TMmsMsvEntry::==
// ----------------------------------------------------
//
EXPORT_C TBool TMmsMsvEntry::operator==( const TMsvEntry& aEntry ) const
    {
    TInt size = sizeof(*this) - 2 * sizeof( TPtrC );
    TBool equal = !(Mem::Compare(( TUint8* )&aEntry, size, 
                                 ( TUint8* )this, size ));
    equal &= ( aEntry.iDescription == this->iDescription );
    equal &= ( aEntry.iDetails == this->iDetails );
    return equal;
    }

// ----------------------------------------------------
// TMmsMsvEntry::==
// ----------------------------------------------------
//
EXPORT_C TBool TMmsMsvEntry::operator==( const TMmsMsvEntry& aEntry ) const
    {
    TInt size = sizeof(*this) - 2 * sizeof( TPtrC );
    TBool equal = !(Mem::Compare(( TUint8* )&aEntry, size, 
                                 ( TUint8* )this, size ));
    equal &= ( aEntry.iDescription == this->iDescription );
    equal &= ( aEntry.iDetails == this->iDetails );
    return equal;
    }

// ----------------------------------------------------
// TMmsMsvEntry::SetSubject
// ----------------------------------------------------
//
EXPORT_C void TMmsMsvEntry::SetSubject( const TDesC& aSubject )
    {
    __ASSERT_ALWAYS( iType == KUidMsvMessageEntry,
        gPanic( EMmsEntryTypeNotSupported ) );
    this->iDescription.Set( aSubject );
    }

// ----------------------------------------------------
// TMmsMsvEntry::Subject
// ----------------------------------------------------
//
EXPORT_C const TDesC& TMmsMsvEntry::Subject() const
    {
    __ASSERT_ALWAYS( iType == KUidMsvMessageEntry,
        gPanic( EMmsEntryTypeNotSupported ) );
    return this->iDescription;
    }

// ----------------------------------------------------
// TMmsMsvEntry::SetAttachmentFile
// ----------------------------------------------------
//
EXPORT_C void TMmsMsvEntry::SetAttachmentFile( const TDesC& aFile )
    {
    __ASSERT_ALWAYS( iType == KUidMsvAttachmentEntry,
        gPanic( EMmsEntryTypeNotSupported ) );
    this->iDetails.Set( aFile );
    }

// ----------------------------------------------------
// TMmsMsvEntry::AttachmentFile
// ----------------------------------------------------
//
EXPORT_C const TDesC& TMmsMsvEntry::AttachmentFile() const
    {
    __ASSERT_ALWAYS( iType == KUidMsvAttachmentEntry,
        gPanic( EMmsEntryTypeNotSupported ));
    return this->iDetails;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  

