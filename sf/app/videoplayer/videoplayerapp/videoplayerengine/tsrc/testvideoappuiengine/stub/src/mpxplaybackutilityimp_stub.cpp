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
* Description:  mpx playback utility stub for unit testing
*
*/

// Version : %version: 1 %


#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxmediageneralextdefs.h>

#include "mpxplaybackutilityimp_stub.h"
#include "mpxvideoplayerconstants.h"
#include "testutilities.h"
#include "testvideoappuiengine.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"


MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL( const TMPXCategory aCategory, const TUid& aModeId )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::UtilityL()"));
    
    TestUtilities* utility = reinterpret_cast<TestUtilities*>( UserSvr::DllTls(12345678) );
    
    if ( !utility ) 
    {
        utility = new (ELeave) TestUtilities;
    
        UserSvr::DllSetTls(12345678, utility);
    
        utility = reinterpret_cast<TestUtilities*>( UserSvr::DllTls(12345678) );        
    }    		
    

    if ( ! utility->iPlaybackUtility )
    {
        utility->iPlaybackUtility = new (ELeave) CMPXPlaybackUtility();
    }

    utility->iPlaybackUtility->AddRefCount();

    return utility->iPlaybackUtility;    
    
    
}

CMPXPlaybackUtility::~CMPXPlaybackUtility()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::~CMPXPlaybackUtility()"));

    iObservers.Close();
}

CMPXPlaybackUtility::CMPXPlaybackUtility()
{
}

void CMPXPlaybackUtility::AddRefCount()
{
    iRefCount++;
}

void CMPXPlaybackUtility::StartPdlPlaybackUtility()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::StartPdlPlaybackUtility()"));

/*    if ( iTestObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityPdlInstance;
        iTestObserver->HandleUtilityEvent( event );
    }
*/    
}

void CMPXPlaybackUtility::AddTestObserverL( MAppUiEngineUnitTestObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::AddTestObserverL()"));

    iTestObserver= aObserver;
}

void CMPXPlaybackUtility::RemoveTestObserverL()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::RemoveTestObserverL()"));

    iTestObserver = NULL;
}


// -------------------------------------------------------------------------------------------------
//   MMPXPlaybackUtility Implementation
// -------------------------------------------------------------------------------------------------

void CMPXPlaybackUtility::AddObserverL( MMPXPlaybackObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::AddObserverL()"));
    iObservers.AppendL(&aObs);
}

void CMPXPlaybackUtility::RemoveObserverL( MMPXPlaybackObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::RemoveObserverL()"));

    TInt i = iObservers.FindL(&aObs);
    iObservers.Remove(i);
}


void CMPXPlaybackUtility::GetClientsL( RArray<TProcessId>& /*aClients*/ )
{
}

void CMPXPlaybackUtility::UpdatePlaylistL( const CMPXCollectionPlaylist& /*aPlaylist*/ )
{
}

void CMPXPlaybackUtility::InitL( const CMPXCollectionPlaylist& /*aPlaylist*/, TBool /*aPlay*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitL(CMPXCollectionPlaylist)"));

    if ( iTestObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPlaybackUtilityInitPlaylist;
        iTestObserver->HandleUtilityEvent( event );
    }
    
}

void CMPXPlaybackUtility::InitL( const TDesC& aUri, const TDesC8* /*aType*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitL()"),
                   _L("aUri = %S"), &aUri );
    
    if ( iTestObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityInitFileName;
        event->iFileName = aUri;
        iTestObserver->HandleUtilityEvent( event );
    }
    
}

void CMPXPlaybackUtility::InitL( RFile& aShareableFile )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitL( RFile )"));

/*    if ( iTestObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPlaybackUtilityInitFileHandle;
        aShareableFile.FullName( event->iFileName );
        iTestObserver->HandleUtilityEvent( event );
    }
*/    
}

void CMPXPlaybackUtility::Close()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::Close()"),
                   _L("iRefCount = %d"), iRefCount );

    //
    //  When the ref count is 2, it should be the app ui engine closing the utility
    //
    if ( iRefCount == 2 )
    {
/*        if ( iTestObserver )
        {
            TCallbackEvent* event = new TCallbackEvent;
            event->iEvent = EPlaybackUtilityClose;
            iTestObserver->HandleUtilityEvent( event );
        }
*/        
    }

    if ( --iRefCount == 0 )
    {
        TestUtilities* utility = reinterpret_cast<TestUtilities*>( UserSvr::DllTls(12345678) );
        	
        utility->DeletePlaybackUtility();
    }
}

void CMPXPlaybackUtility::CancelRequest()
{
}

void CMPXPlaybackUtility::CommandL( TMPXPlaybackCommand aCmd, TInt /*aData*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::CommandL()"),
                   _L("aCmd = %d"), aCmd );

    switch ( aCmd )
    {
        case EPbCmdPlay:
        {
            if ( iTestObserver )
            {
                TCallbackEvent* event = new TCallbackEvent;
                event->iEvent = EPlaybackUtilityPlayCmd;
                iTestObserver->HandleUtilityEvent( event );
            }

            break;
        }
    }
}

void CMPXPlaybackUtility::CommandL( CMPXCommand& aCmd, MMPXPlaybackCallback* /*aCallback*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::CommandL( CMPXCommand )"));

    if ( aCmd.IsSupported( KMPXCommandGeneralId ) )
    {
        TInt cmdId = *aCmd.Value<TInt>(KMPXCommandGeneralId);

        MPX_DEBUG(_L("CMPXPlaybackUtility::CommandL() id = 0x%08x"), cmdId );

        switch ( cmdId )
        {
            case KMPXCommandIdPlaybackGeneral:
            {
                break;
            }
            case KMPXMediaIdVideoPlayback:
            {
                TMPXVideoPlaybackCommand videoCmd =
                        static_cast<TMPXVideoPlaybackCommand>(
                            *aCmd.Value<TInt>(KMPXMediaVideoPlaybackCommand) );

                switch ( videoCmd )
                {
                    case EPbCmdUpdateSeekable:
                    {
                        TBool seekable = *aCmd.Value<TBool>( KMPXMediaGeneralExtVideoSeekable );

                        if ( iTestObserver )
                        {
                            TCallbackEvent* event = new TCallbackEvent;
                            event->iEvent      = EAsxAttributeSeekUpdate;
                            event->iExtra      = seekable;
                            iTestObserver->HandleUtilityEvent( event );
                        }
                        
                        break;
                    }
                }

                break;
            }

        }
    }
}

TMPXPlaybackState CMPXPlaybackUtility::StateL() const
{
    return EPbStateNotInitialised;
}

MMPXSource* CMPXPlaybackUtility::Source()
{
    return this;
}

MMPXPlayerManager& CMPXPlaybackUtility::PlayerManager()
{
    return *this;
}

void CMPXPlaybackUtility::SetL( TMPXPlaybackProperty /*aProperty*/, TInt /*aValue*/ )
{
}

void CMPXPlaybackUtility::ValueL( MMPXPlaybackCallback& /*aCallback*/,
                                  TMPXPlaybackProperty /*aProperty*/ )
{
}

void CMPXPlaybackUtility::PropertyL( MMPXPlaybackCallback& /*aCallback*/,
                                   TMPXPlaybackProperty /*aProperty*/ )
{
}

CDesCArray* CMPXPlaybackUtility::SupportedMimeTypes()
{
    return NULL;
}

CDesCArray* CMPXPlaybackUtility::SupportedExtensions()
{
    return NULL;
}

CDesCArray* CMPXPlaybackUtility::SupportedSchemas()
{
    return NULL;
}

void CMPXPlaybackUtility::SetPriority( TInt /*aPriority*/ )
{
}

void CMPXPlaybackUtility::AddSubscriptionL( const CMPXSubscription& /*aSubscription*/ )
{
}

void CMPXPlaybackUtility::RemoveSubscriptionL( const CMPXSubscription& /*aSubscription*/ )
{
}

void CMPXPlaybackUtility::ClearSubscriptionsL()
{
}

void CMPXPlaybackUtility::InitStreamingL( const TDesC& aUri,
                                          const TDesC8* /*aType*/,
                                          const TInt aAccessPoint )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitStreamingL()"),
                   _L("aUri = %S, aAccessPoint = %d"), &aUri, aAccessPoint );

    if ( iTestObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityInitStreamingUrl;
        event->iFileName = aUri;
        event->iApId     = aAccessPoint;
        iTestObserver->HandleUtilityEvent( event );
    }
    
}

void CMPXPlaybackUtility::InitStreamingL( RFile& aShareableFile, const TInt aAccessPoint )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitStreamingL( RFile )"),
                   _L("aAccessPoint = %d"), aAccessPoint );
/*
    if ( iTestObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityInitStreamingFileHandle;
        aShareableFile.FullName( event->iFileName );
        event->iApId     = aAccessPoint;
        iTestObserver->HandleUtilityEvent( event );
    }
*/    
}


// -------------------------------------------------------------------------------------------------
//   MMPXPlayerManager Implementation
// -------------------------------------------------------------------------------------------------

void CMPXPlaybackUtility::GetPlayerTypesL( RArray<TMPXPlaybackPlayerType>& /*aTypes*/ )
{
}

HBufC* CMPXPlaybackUtility::PlayerTypeDisplayNameL( TMPXPlaybackPlayerType /*aType*/ )
{
    return NULL;
}

void CMPXPlaybackUtility::GetPlayerListL( RArray<TUid>& /*aPlayers*/ )
{
}

void CMPXPlaybackUtility::GetPlayerListL( RArray<TUid>& /*aPlayers*/,
                                          TMPXPlaybackPlayerType /*aType*/ )
{
}

void CMPXPlaybackUtility::SubPlayerNamesL( MMPXPlaybackCallback& /*aCallback*/, TUid /*aPlayer*/ )
{
}

void CMPXPlaybackUtility::SelectPlayersL( TMPXPlaybackPlayerType /*aType*/ )
{
}

void CMPXPlaybackUtility::SelectSubPlayerL( TUid /*aPlayer*/, TInt /*aSubPlayerIndex*/ )
{
}

void CMPXPlaybackUtility::SelectPlayerL( TUid /*aPlayer*/ )
{
}

void CMPXPlaybackUtility::ClearSelectPlayersL()
{
}

void CMPXPlaybackUtility::GetSelectionL( TMPXPlaybackPlayerType& /*aType*/,
                                         TUid& /*aPlayer*/,
                                         TInt& /*aSubPlayerIndex*/,
                                         HBufC*& /*aSubPlayerName*/ )
{
}

MMPXPlayer* CMPXPlaybackUtility::CurrentPlayer()
{
    return this;
}


// -------------------------------------------------------------------------------------------------
//   MMPXPlayer Implementation
// -------------------------------------------------------------------------------------------------

TMPXPlaybackPlayerType CMPXPlaybackUtility::TypeL()
{
    return EPbUnknown;
}

HBufC* CMPXPlaybackUtility::TypeNameL()
{
    return NULL;
}

void CMPXPlaybackUtility::SubPlayerNamesL( MMPXPlaybackCallback& /*aCallback*/ )
{
}

TInt CMPXPlaybackUtility::SubPlayerL() const
{
    return 0;
}

TUid CMPXPlaybackUtility::UidL() const
{
    return KNullUid;
}


// -------------------------------------------------------------------------------------------------
//   MPXSource Implementation
// -------------------------------------------------------------------------------------------------

CMPXCollectionPlaylist* CMPXPlaybackUtility::PlaylistL()
{
    return NULL;
}

RFile* CMPXPlaybackUtility::FileL()
{
    return NULL;
}

HBufC* CMPXPlaybackUtility::UriL()
{
    return NULL;
}

void CMPXPlaybackUtility::MediaL( const TArray<TMPXAttribute>& /*aAttrs*/,
                                  MMPXPlaybackCallback& /*aCallback*/ )
{
}

void CMPXPlaybackUtility::MediaL( const TArray<TMPXAttribute>& /*aAttrs*/,
                                  MMPXPlaybackCallback& /*aCallback*/,
                                  CMPXAttributeSpecs* /*aSpecs*/ )
{
}

// End of file
