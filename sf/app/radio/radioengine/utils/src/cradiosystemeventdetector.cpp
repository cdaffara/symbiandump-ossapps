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

#include <e32def.h> // Define before audiopolicy defs
#include <e32std.h> // Define before audiopolicy defs
#include <eikdef.h>

#ifndef __WINS__

//#include <audiopolicypubsubdata.h>
//#include <audiosw_pubsubkeys.h>
#include <AudioClientsListPSKeys.h>

#endif //__WINS__

#include <ctsydomainpskeys.h>
#include <publicruntimeids.hrh>
#include <sacls.h>

// User includes
#include "cradioenginelogger.h"
#include "cradiosystemeventdetector.h"
#include "mradiosystemeventdetectorobserver.h"

/** Granularity for audio category arrays. */
//const TInt KVRAudioCategoryArrayGranularity = 3;

// This has to be the last include. 
#ifdef STUB_CONSTELLATION
#   include "RadioStubManager.h"
#   define KUidSystemCategory KStub_KUidSystemCategory
#   define KPSUidCtsyCallInformation KStub_KPSUidCtsyCallInformation
#   define KPSUidVoiceUiAccMonitor KStub_KPSUidVoiceUiAccMonitor
#   define KPSUidMMFAudioServer KStub_KPSUidMMFAudioServer
#endif //STUB_CONSTELLATION

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventDetector::CRadioSystemEventDetector( MRadioSystemEventDetectorObserver&  aObserver )
    : iObserver( aObserver )
    , iIsMobileNetworkCoverage( EFalse )
    , iIsCallActive( EFalse )
    , iIsAudioResourcesAvailable( ETrue )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::ConstructL()
    {
    LOG_METHOD_AUTO;
    iNetworkStatusObserver = CRadioPropertyObserver::NewL( *this,
                                                           KUidSystemCategory,
                                                           KUidNetworkStatusValue,
                                                           CRadioPropertyObserver::ERadioPropertyInt );
    iNetworkStatusObserver->ActivateL();

    iIsMobileNetworkCoverage = iNetworkStatusObserver->ValueInt() == ESANetworkAvailable;

    // Initialize call state observer.
    iCallStatusObserver = CRadioPropertyObserver::NewL( *this,
                                                        KPSUidCtsyCallInformation,
                                                        KCTsyCallState,
                                                        CRadioPropertyObserver::ERadioPropertyInt );
    iCallStatusObserver->ActivateL();
    iIsCallActive = iCallStatusObserver->ValueInt() != EPSCTsyCallStateNone;
/*
#ifndef __WINS__
    // Define audio types for not resuming.
    //TODO: Check these audio resume categories!
    iNoAutoResumeAudioCategories = RArray<TInt>( KVRAudioCategoryArrayGranularity );
    iNoAutoResumeAudioCategories.AppendL( ECatMediaPlayer );
    iNoAutoResumeAudioCategories.AppendL( ECatMobileTv );
    iNoAutoResumeAudioCategories.AppendL( ECatUnknownPlayer );
    iNoAutoResumeAudioCategories.Compress();
    // Start listening audio client events.
    iAudioPolicyObserver = CRadioPropertyObserver::NewL( *this, KPSUidMMFAudioServer, KAudioPolicyAudioClients, CRadioPropertyObserver::ERadioPropertyByteArray );
    iAudioPolicyObserver->ActivateL();
#endif
*/
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventDetector::~CRadioSystemEventDetector()
    {
    LEVEL3( LOG_METHOD_AUTO );
    delete iCallStatusObserver;
    delete iNetworkStatusObserver;
    delete iAudioPolicyObserver;

    iNoAutoResumeAudioCategories.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventDetector* CRadioSystemEventDetector::NewL( MRadioSystemEventDetectorObserver&  aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioSystemEventDetector* self = new ( ELeave ) CRadioSystemEventDetector( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Handling of the int property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory,
                                                       const TUint aKey,
                                                       const TInt aValue )
    {
    LEVEL2( LOG_METHOD_AUTO );
    if ( aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState )
        {
        if ( (!iIsCallActive ) && ( aValue > EPSCTsyCallStateNone ))
            {
            iIsCallActive = ETrue;
            iObserver.CallActivatedCallbackL();
            }
        else if ( ( iIsCallActive ) && ( aValue <= EPSCTsyCallStateNone ))
            {
            iIsCallActive = EFalse;
            iObserver.CallDeactivatedCallbackL();
            }
        }
    }

#ifndef __WINS__
// ---------------------------------------------------------------------------
// Handling of the byte array property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory,
                                                       const TUint aKey,
                                                       const TDesC8& aValue )
    {
#if 0
    LEVEL2( LOG_METHOD_AUTO );
    LOG_FORMAT( "Category: %d, Key: %d", aCategory, aKey );
    if ( aCategory == KPSUidMMFAudioServer )
        {
        if ( aKey == KAudioPolicyAudioClients )
            {
            TBool atLeastOneAutoResumeAudioPlaying( EFalse );
            TBool atLeastOneNoAutoResumeAudioPlaying( EFalse );
            TBool radioPlaying( EFalse );
            TAudioClientList audioClients;
            audioClients.Copy( aValue );
            // Check all playing audios!
            for ( TInt i = 0; i < audioClients().iNumOfProcesses ; ++i )
                {
                TInt cat = audioClients().iClientCategoryList[i];
                LOG_FORMAT( "Check audio cat %x", cat );
                if ( cat == ECatFmRadio )
                    {
                    radioPlaying = ETrue;
                    }
                else if ( iNoAutoResumeAudioCategories.Find( cat ) != KErrNotFound )
                    {
                    atLeastOneNoAutoResumeAudioPlaying = ETrue;
                    }
                else
                    {
                    atLeastOneAutoResumeAudioPlaying = ETrue;
                    }
                }

            if ( !radioPlaying )
                {
                // Decide audio resource availability from audio category info.
                if ( atLeastOneNoAutoResumeAudioPlaying )
                    {
                    LOG( "Audio resources not available. Change informed." );
                    iIsAudioResourcesAvailable = EFalse;
                    iObserver.AudioAutoResumeForbiddenL();
                    }
                else if ( !atLeastOneAutoResumeAudioPlaying )
                    {
                    if ( !iIsVoiceUiActive )
                        {
                        LOG( "Audio resources available. Change informed." );
                        iIsAudioResourcesAvailable = ETrue;
                        iObserver.AudioResourcesAvailableL();
                        }
                    else
                        {
                        LOG( "Audio resources available. Change not informed." );
                        iIsAudioResourcesAvailable = ETrue;
                        }
                    }
                else
                    {
                    LOG( "Audio resources not available. Change not informed." );
                    iIsAudioResourcesAvailable = EFalse;
                    }
                }
            else // audio resources are considered to be available when radio is playing
                {
                iIsAudioResourcesAvailable = ETrue;
                }
            }
        }
#endif
    }

#else //__WINS__
// ---------------------------------------------------------------------------
// Dummy version for WINS in order to avoid compiler warnings.
// The real implementation of function is above.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/,
                                                       const TUint /*aKey*/,
                                                        const TDesC8& /*aValue*/)
    {
    }
#endif

// ---------------------------------------------------------------------------
// Handling of the text property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/,
                                                       const TUint /*aKey*/,
                                                       const TDesC& /*aValue*/)
    {
    }

// ---------------------------------------------------------------------------
// This is a callback function which is called when a P&S components returns
// an error
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeErrorL( const TUid& /*aCategory*/,
                                                            const TUint /*aKey*/,
                                                            TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsMobileNetworkCoverage() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iIsMobileNetworkCoverage;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsNetworkCoverage() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iIsMobileNetworkCoverage;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsCallActive() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iIsCallActive;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsAudioResourcesAvailable() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iIsAudioResourcesAvailable;
    }
