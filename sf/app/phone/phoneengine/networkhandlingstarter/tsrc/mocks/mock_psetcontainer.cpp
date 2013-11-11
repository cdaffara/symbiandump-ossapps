/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <smcmockclassincludes.h>
#include <mmtsy_names.h>
#include <mpsetdivertobs.h>
#include "PsetContainer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsetContainer::ConstructL
// -----------------------------------------------------------------------------
//
void CPsetContainer::ConstructL(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPsetContainer::NewL
// -----------------------------------------------------------------------------
//
CPsetContainer * CPsetContainer::NewL(  )
    {
    SMC_MOCK_NEWL0( CPsetContainer * )

    CPsetContainer* self = new( ELeave ) CPsetContainer;
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPsetContainer::~CPsetContainer
// -----------------------------------------------------------------------------
//
CPsetContainer::~CPsetContainer(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateCWObjectL
// -----------------------------------------------------------------------------
//
CPsetCallWaiting * CPsetContainer::CreateCWObjectL( 
        MPsetCallWaitingObserver & aObserver )
    {
    SMC_MOCK_METHOD1( CPsetCallWaiting *, MPsetCallWaitingObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateCFObjectL
// -----------------------------------------------------------------------------
//
CPsetCallDiverting * CPsetContainer::CreateCFObjectL( 
        MPsetDivertObserver & aObserver )
    {
    SMC_MOCK_METHOD1( CPsetCallDiverting *, MPsetDivertObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateCliObjectL
// -----------------------------------------------------------------------------
//
CPsetCli * CPsetContainer::CreateCliObjectL( 
        MPsetCliObserver & aObserver )
    {
    SMC_MOCK_METHOD1( CPsetCli *, MPsetCliObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateCBObjectL
// -----------------------------------------------------------------------------
//
CPsetCallBarring * CPsetContainer::CreateCBObjectL( 
        MPsetBarringObserver & aObserver )
    {
    SMC_MOCK_METHOD1( CPsetCallBarring *, MPsetBarringObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateNetworkObjectL
// -----------------------------------------------------------------------------
//
CPsetNetwork * CPsetContainer::CreateNetworkObjectL( 
        MPsetNetworkInfoObserver & aObserver )
    {
    SMC_MOCK_METHOD1( CPsetNetwork *, MPsetNetworkInfoObserver &, aObserver )
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateRefreshHandlerL
// -----------------------------------------------------------------------------
//
CPSetRefreshHandler * CPsetContainer::CreateRefreshHandlerL(  )
    {
    SMC_MOCK_METHOD0( CPSetRefreshHandler * )
    }


// -----------------------------------------------------------------------------
// CPsetContainer::CreateNetworkModeObjectL
// -----------------------------------------------------------------------------
//
CPsetNetwork * CPsetContainer::CreateNetworkModeObjectL( 
        MPsetNetworkModeObserver & aModeObserver )
    {
    SMC_MOCK_METHOD1( CPsetNetwork *, MPsetNetworkModeObserver &, aModeObserver )
    }


