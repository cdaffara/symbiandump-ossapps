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
* Description:  LiveComm Videotelephony Plugin Class
*
*/

#include <lcsession.h>
#include <lcuiprovider.h>
#include <lcsessionobserver.h>
#include "clcvtengine.h"
#include "clcvtsession.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtEngine* CLcVtEngine::NewL( TAny* aParams )
    {
    CLcVtEngine* self = new ( ELeave ) CLcVtEngine;
    CleanupStack::PushL( self );
    self->ConstructL(); 
    CleanupStack::Pop( self );
    return self;
    }
    
CLcVtEngine::~CLcVtEngine()
    {
    delete iSession;
    }
    
MLcSession& CLcVtEngine::Session()
    {
    return *iSession;
    }
 
void CLcVtEngine::ConstructL() 
    {
    iSession = CLcVtSession::NewL();
    }
 
CLcVtEngine::CLcVtEngine() : CLcEngine()
    {
    
    }

TBool CLcVtEngine::IsFeatureSupported( TLcFeature aLcFeature )
    {
    return iSession->IsFeatureSupported( aLcFeature );
    }
