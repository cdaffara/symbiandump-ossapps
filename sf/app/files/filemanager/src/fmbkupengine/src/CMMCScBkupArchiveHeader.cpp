/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CMMCScBkupArchiveHeader Implementation
*
*
*/

#include "CMMCScBkupArchiveHeader.h"

// User includes
#include "MMCScBkupArchiveUtils.h"

/**
 * NB. See MMCScBkupArchiveUtils.cpp for archive header file format.
 */


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::CMMCScBkupArchiveHeader()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveHeader::CMMCScBkupArchiveHeader( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver )
:   iDataInterface( aDataInterface ), iDriver( aDriver )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::~CMMCScBkupArchiveHeader()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveHeader::~CMMCScBkupArchiveHeader()
    {
    delete iPhoneModelIdentifier;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveHeader::ConstructL()
    {
    iPhoneModelIdentifier = KNullDesC8().AllocL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveHeader* CMMCScBkupArchiveHeader::NewL( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupArchiveHeader* self = new(ELeave) CMMCScBkupArchiveHeader( aDataInterface, aDriver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::StoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveHeader::StoreL(TBitFlags aCategory)
    {
    MMCScBkupArchiveUtils::WriteHeaderL( iDataInterface, aCategory );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::RestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveHeader::RestoreL()
    {
    MMCScBkupArchiveUtils::ReadHeaderL( iDataInterface, *this );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveHeader::SetPhoneModelIdentifierL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveHeader::SetPhoneModelIdentifierL( const TDesC8& aModel )
    {
    HBufC8* model = aModel.AllocL();
    delete iPhoneModelIdentifier;
    iPhoneModelIdentifier = model;
    }
