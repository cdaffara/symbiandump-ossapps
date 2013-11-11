/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES

#include <e32svr.h>

#include "CVSControllerImp.h"
#include "VSPanic.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Panics current thread.
// -----------------------------------------------------------------------------
//
void Panic( TVSPanic aPanicCode )
    {
    User::Panic( KVSPanicCategory(), aPanicCode );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVSController::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CVSController* CVSController::NewL( MVSControllerObserver* aObserver )
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSController::NewL() entering")));
    if ( aObserver == NULL )
        {
        User::Leave( KErrArgument );
        }
    CVSController* self = CVSControllerImp::NewL();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    __IF_DEBUG(Print(_L("VideoSource: CVSController::NewL() returning")));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::ConstructL
// -----------------------------------------------------------------------------
//
void CVSControllerImp::ConstructL( MVSControllerObserver* aObserver )
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::ConstructL() entering")));
    iObserver = aObserver;
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::ConstructL() returning")));
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::NewL
// -----------------------------------------------------------------------------
//
CVSControllerImp* CVSControllerImp::NewL()
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::NewL() entering")));
    CVSControllerImp* self = new (ELeave) CVSControllerImp;
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::NewL() returning")));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::~CVSControllerImp
// -----------------------------------------------------------------------------
//
CVSControllerImp::~CVSControllerImp()
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::~CVSControllerImp() entering")));
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::~CVSControllerImp() returning")));
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::CreateDataSourceL
// -----------------------------------------------------------------------------
//
CVSDataSource* CVSControllerImp::CreateDataSourceL( MVSDataSourceObserver* aObserver )
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::CreateDataSourceL() entering")));
    CVSDataSource* p = CVSDataSource::NewSourceL( aObserver );
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::CreateDataSourceL() returning")));
    return p;
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::ProvidersAvailable
// -----------------------------------------------------------------------------
//
TInt CVSControllerImp::ProvidersAvailable()
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::ProvidersAvailable() entering")));
    TInt count = CVSDataProvider::ProvidersAvailable();
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::ProvidersAvailable() returning")));
    return count;
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::ProviderInfoL
// -----------------------------------------------------------------------------
//
void CVSControllerImp::ProviderInfoL(
    TInt aProviderIndex,
    TVSDataProviderInfo& aInfo )
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::ProviderInfo() entering")));
    if (aProviderIndex >= ProvidersAvailable() || aProviderIndex < 0)
        {
        User::Leave( KErrNotSupported );
        }
    CVSDataProvider::ProviderInfoL( aProviderIndex, aInfo );
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::ProviderInfo() returning")));
    }

// -----------------------------------------------------------------------------
// CVSControllerImp::CreateDataProviderL
// -----------------------------------------------------------------------------
//
CVSDataProvider* CVSControllerImp::CreateDataProviderL(
    TInt aProviderIndex,
    MVSDataProviderObserver* aObserver,
    MVSBufferPool* aPool )
    {
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::CreateDataProviderL() entering")));
    if (aProviderIndex >= ProvidersAvailable() || aProviderIndex < 0)
        {
        User::Leave( KErrArgument );
        }
    if ( aObserver == NULL )
        {
        User::Leave( KErrArgument );
        }
    CVSDataProvider* p =
        CVSDataProvider::NewL( aProviderIndex, aObserver, aPool );
    __IF_DEBUG(Print(_L("VideoSource: CVSControllerImp::CreateDataProviderL() returning")));
    return p;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
