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

// User includes
#include "cradioenginelogger.h"
#include "cradioaudiorouter.h"
#include "mradioaudioroutingobserver.h"
#include "cradioroutableaudio.h"
#include "radioengineutils.h"


const TInt KVisualRadioInitialRoutableAudioArraySize( 2 );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAudioRouter::CRadioAudioRouter( MRadioAudioRoutingObserver& aAudioRoutingObserver )
    : iAudioRoutingObserver( aAudioRoutingObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioAudioRouter* CRadioAudioRouter::NewL( MRadioAudioRoutingObserver& aAudioRoutingObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioAudioRouter* self = new( ELeave ) CRadioAudioRouter( aAudioRoutingObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioAudioRouter::~CRadioAudioRouter()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRoutableAudios.Close();
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAudioRouter::ConstructL()
    {
    RadioEngineUtils::InitializeL();
    LEVEL3( LOG_METHOD_AUTO );
    iRoutableAudios = RArray<CRadioRoutableAudio*>( KVisualRadioInitialRoutableAudioArraySize );
    }

// ---------------------------------------------------------------------------
// Sets audio route
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioAudioRouter::SetAudioRouteL( RadioEngine::TRadioAudioRoute aAudioRoute )
    {
    LEVEL3( LOG_METHOD_AUTO );

    for ( TInt i = 0; i < iRoutableAudios.Count(); ++i )
        {
        iRoutableAudios[i]->SetAudioRouteL( aAudioRoute );
        }
    iAudioRoutingObserver.AudioRouteChangedL( aAudioRoute );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioAudioRouter::RegisterRoutableAudio( CRadioRoutableAudio* aRoutableAudio )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRoutableAudios.Append( aRoutableAudio );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioAudioRouter::UnRegisterRoutableAudio( CRadioRoutableAudio* aRoutableAudio )
    {
    LEVEL3( LOG_METHOD_AUTO );

    TInt objectIndex = iRoutableAudios.Find( aRoutableAudio );

    __ASSERT_DEBUG( objectIndex != KErrNotFound, User::Panic( _L("VisualRadio" ), KErrAbort ) );

    if ( objectIndex != KErrNotFound )
        {
        iRoutableAudios.Remove( objectIndex );
        }
    }

