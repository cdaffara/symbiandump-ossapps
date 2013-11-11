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

#ifndef MPXPLAYBACKUTILITYIMP_STUB_H_
#define MPXPLAYBACKUTILITYIMP_STUB_H_

#include <mpxplaybackutility.h>

#include "testvideoappuiengine.h"


NONSHARABLE_CLASS( CMPXPlaybackUtility ) : public CBase,
                                           public MMPXPlaybackUtility,
                                           public MMPXPlayer,
                                           public MMPXPlayerManager,
                                           public MMPXSource
{
    public:

        static MMPXPlaybackUtility* UtilityL();

        CMPXPlaybackUtility();

        void AddRefCount();

    public:

        ~CMPXPlaybackUtility();

        void AddTestObserverL( MAppUiEngineUnitTestObserver* aObserver );
        void RemoveTestObserverL();
        void StartPdlPlaybackUtility();

        //
        //  MMPXPlaybackUtility Implementation
        //
        void AddObserverL( MMPXPlaybackObserver& aObs );
        void RemoveObserverL( MMPXPlaybackObserver& aObs );
        void GetClientsL( RArray<TProcessId>& aClients );
        void UpdatePlaylistL(const CMPXCollectionPlaylist& aPlaylist);
        void InitL( const CMPXCollectionPlaylist& aPlaylist, TBool aPlay=ETrue );
        void InitL( const TDesC& aUri, const TDesC8* aType = NULL );
        void InitL( RFile& aShareableFile );
        void Close();
        void CancelRequest();
        void CommandL( TMPXPlaybackCommand aCmd, TInt aData = 0 );
        void CommandL( CMPXCommand& aCmd, MMPXPlaybackCallback* aCallback = NULL );
        TMPXPlaybackState StateL() const;
        MMPXSource* Source();
        MMPXPlayerManager& PlayerManager();
        void SetL( TMPXPlaybackProperty aProperty, TInt aValue );
        void ValueL( MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty );
        void PropertyL( MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty );
        CDesCArray* SupportedMimeTypes();
        CDesCArray* SupportedExtensions();
        CDesCArray* SupportedSchemas();
        void SetPriority( TInt aPriority );
        void AddSubscriptionL( const CMPXSubscription& aSubscription );
        void RemoveSubscriptionL( const CMPXSubscription& aSubscription );
        void ClearSubscriptionsL();
        void InitStreamingL( const TDesC& aUri, const TDesC8* aType, const TInt aAccessPoint );
        void InitStreamingL( RFile& aShareableFile, const TInt aAccessPoint );


        //
        //  MMPXPlayerManager Implementation
        //
        void GetPlayerTypesL( RArray<TMPXPlaybackPlayerType>& aTypes );
        HBufC* PlayerTypeDisplayNameL( TMPXPlaybackPlayerType aType );
        void GetPlayerListL( RArray<TUid>& aPlayers );
        void GetPlayerListL( RArray<TUid>& aPlayers, TMPXPlaybackPlayerType aType );
        void SubPlayerNamesL( MMPXPlaybackCallback& aCallback, TUid aPlayer );
        void SelectPlayersL( TMPXPlaybackPlayerType aType );
        void SelectSubPlayerL( TUid aPlayer, TInt aSubPlayerIndex );
        void SelectPlayerL( TUid aPlayer );
        void ClearSelectPlayersL();
        void GetSelectionL( TMPXPlaybackPlayerType& aType,
                            TUid& aPlayer,
                            TInt& aSubPlayerIndex,
                            HBufC*& aSubPlayerName );
        MMPXPlayer* CurrentPlayer();

        //
        //  MMPXPlayer Implementation
        //
        TMPXPlaybackPlayerType TypeL();
        HBufC* TypeNameL();
        void SubPlayerNamesL( MMPXPlaybackCallback& aCallback );
        TInt SubPlayerL() const;
        TUid UidL() const;

        //
        //  MPXSource Implementation
        //
        CMPXCollectionPlaylist* PlaylistL();
        RFile* FileL();
        HBufC* UriL();
        void MediaL( const TArray<TMPXAttribute>& aAttrs, MMPXPlaybackCallback& aCallback );
        void MediaL( const TArray<TMPXAttribute>& aAttrs,
                     MMPXPlaybackCallback& aCallback,
                     CMPXAttributeSpecs* aSpecs );

    private:

        RArray<MMPXPlaybackObserver*>   iObservers;
        MAppUiEngineUnitTestObserver*   iTestObserver;
        TInt                            iRefCount;
};

#endif /*MPXPLAYBACKUTILITYIMP_STUB_H_*/
