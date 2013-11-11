/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPELogExtensionWrapper class.
*
*/


#include "cpelogextensionwrapper.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::CPELogExtensionWrapper
// ---------------------------------------------------------------------------
//
CPELogExtensionWrapper::CPELogExtensionWrapper( TUid aImplementationUid )
    : iPluginUid( aImplementationUid )
    {

    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CPELogExtensionWrapper::ConstructL()
    {
    iPlugin = CreateLoggingExtensionL();
    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::NewL
// ---------------------------------------------------------------------------
//
CPELogExtensionWrapper* CPELogExtensionWrapper::NewL( 
        TUid aImplementationUid )
    {
    CPELogExtensionWrapper* self = 
        CPELogExtensionWrapper::NewLC( aImplementationUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::NewLC
// ---------------------------------------------------------------------------
//
CPELogExtensionWrapper* CPELogExtensionWrapper::NewLC( 
        TUid aImplementationUid )
    {
    CPELogExtensionWrapper* self = 
        new( ELeave ) CPELogExtensionWrapper( aImplementationUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::~CPELogExtensionWrapper
// ---------------------------------------------------------------------------
//
CPELogExtensionWrapper::~CPELogExtensionWrapper()
    {
    delete iPlugin;
    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::MatchByUid
// ---------------------------------------------------------------------------
//
TBool CPELogExtensionWrapper::MatchByUid( 
        const TUid* aKey, const CPELogExtensionWrapper& aWrapper )
    {
    if ( aKey )
        {
        return ( aWrapper.Identifier() == *aKey );
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------------------------
// From class CTelLoggingExtension.
// ---------------------------------------------------------------------------
//
void CPELogExtensionWrapper::InitializeL( TUint aServiceId, 
        const TDesC& aOrigAddress )
    {
    iPlugin->InitializeL( aServiceId, aOrigAddress );
    }


// ---------------------------------------------------------------------------
// From class CTelLoggingExtension.
// ---------------------------------------------------------------------------
//
TInt CPELogExtensionWrapper::GetPhoneNumber( RBuf& aPhoneNumber )
    {
    return iPlugin->GetPhoneNumber( aPhoneNumber );
    }


// ---------------------------------------------------------------------------
// From class CTelLoggingExtension.
// ---------------------------------------------------------------------------
//
TInt CPELogExtensionWrapper::GetVoipAddress( RBuf& aVoipAddress )
    {
    return iPlugin->GetVoipAddress( aVoipAddress );
    }


// ---------------------------------------------------------------------------
// From class CTelLoggingExtension.
// ---------------------------------------------------------------------------
//
TInt CPELogExtensionWrapper::GetMyAddress( RBuf& aMyAddress )
    {
    return iPlugin->GetMyAddress( aMyAddress );
    }


// ---------------------------------------------------------------------------
// From class CTelLoggingExtension.
// ---------------------------------------------------------------------------
//
TInt CPELogExtensionWrapper::GetRemotePartyName( RBuf& aRemotePartyName )
    {
    return iPlugin->GetRemotePartyName( aRemotePartyName );
    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::CreateLoggingExtensionL
// ---------------------------------------------------------------------------
//
CTelLoggingExtension* CPELogExtensionWrapper::CreateLoggingExtensionL() const
    {
    return CTelLoggingExtension::NewL( iPluginUid );
    }


// ---------------------------------------------------------------------------
// CPELogExtensionWrapper::Identifier
// ---------------------------------------------------------------------------
//
TUid CPELogExtensionWrapper::Identifier() const
    {
    return iPluginUid;
    }
