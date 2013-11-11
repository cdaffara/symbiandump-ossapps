/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "iradvertisement.h"
#include "irdebug.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Function : NewLC
// Function returns an instance of CIRAdvertisement
// Two phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRAdvertisement* CIRAdvertisement::NewLC()
    {
    IRLOG_DEBUG( "CIRAdvertisement::NewLC - Entering" );
    CIRAdvertisement* self = new( ELeave )CIRAdvertisement;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRAdvertisement::NewLC - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRAdvertisement::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRAdvertisement* CIRAdvertisement::NewL()
    {
    IRLOG_DEBUG( "CIRAdvertisement::NewL - Entering" );
    CIRAdvertisement* self = CIRAdvertisement::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRAdvertisement::NewL - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRAdvertisement::ConstructL()
// standard second phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAdvertisement::ConstructL() const
    {
    IRLOG_DEBUG( "CIRAdvertisement::ConstructL" );
    return;
    }


// ---------------------------------------------------------------------------
// CIRAdvertisement::~CIRAdvertisement()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
CIRAdvertisement::~CIRAdvertisement()
    {
    IRLOG_DEBUG( "CIRAdvertisement::~CIRAdvertisement - Entering" );
    delete  iUrl;
    iUrl = NULL;
    delete iAdvInUse;
    iAdvInUse = NULL;
    IRLOG_DEBUG( "CIRAdvertisement::~CIRAdvertisement - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRAdvertisement::SetAdvInUse()
// Function to set the advertisement in use
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAdvertisement::SetAdvInUse( const TDesC& aAdvInUse )
    {
    IRLOG_DEBUG( "CIRAdvertisement::SetAdvInUse - Entering" );
    iAdvInUse = aAdvInUse.Alloc();
    IRLOG_DEBUG( "CIRAdvertisement::SetAdvInUse - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRAdvertisement::SetUsage()
// Function to set the number of times the global adv to be shown
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAdvertisement::SetUsage( const TDesC& aUsageVisibleTimes )
    {
    IRLOG_DEBUG( "CIRAdvertisement::SetUsage - Entering" );
    TLex var( aUsageVisibleTimes );
    var.Val( iUsageVisibleTimes );
    IRLOG_DEBUG( "CIRAdvertisement::SetUsage - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRAdvertisement::SetVisibleTime()
// Function to set the duration of the advertisement to be displayed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAdvertisement::SetVisibleTime( const TDesC& aVisibleTime )
    {
    IRLOG_DEBUG( "CIRAdvertisement::SetVisibleTime - Entering" );
    TLex var( aVisibleTime );
    var.Val( iVisibleTime );
    IRLOG_DEBUG( "CIRAdvertisement::SetVisibleTime - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRAdvertisement::SetURL()
// Function to set the url of the global advertisement
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAdvertisement::SetURL( const TDesC& aUrl )
    {
    IRLOG_DEBUG( "CIRAdvertisement::SetURL - Entering" );
    iUrl = aUrl.Alloc();
    IRLOG_DEBUG( "CIRAdvertisement::SetURL - Exiting." );
    return;
    }
