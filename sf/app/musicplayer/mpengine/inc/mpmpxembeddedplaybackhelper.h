/*
* Copyright ( c ) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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
* Description: embedded playback helper.
*
*/

#ifndef CMPMPXEMBEDDEDPLAYBACKHELPER_H
#define CMPMPXEMBEDDEDPLAYBACKHELPER_H

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelperobserver.h>

// FOWARD DECLARATION
class MMPXPlaybackUtility;
class MMPXCollectionUtility;
class MMPXCollectionUiHelper;

class MMpMpxEmbeddedPlaybackHelperObserver
{
public:

    virtual void HandleEmbeddedPlaybackError( TInt aError ) = 0;

};

class CMpMpxEmbeddedPlaybackHelper : public CBase,
                                     public MMPXCollectionObserver,
                                     public MMPXCHelperEmbeddedOpenObserver
{

public:

    static CMpMpxEmbeddedPlaybackHelper* NewL( TUid aHostUid,
                                               MMPXPlaybackUtility* aPlaybackUtility,
                                               MMpMpxEmbeddedPlaybackHelperObserver* aObserver );
    virtual ~CMpMpxEmbeddedPlaybackHelper();

    void playL( const TDesC& aFilename );

private: // from base class

    // From MMPXCollectionObserver
    void HandleOpenL( const CMPXMedia& aEntries,
                     TInt aIndex,TBool aComplete,TInt aError );
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

    // From MMPXCHelperEmbeddedOpenObserver
    void HandleEmbeddedOpenL( TInt aErr, TMPXGeneralCategory aCategory );

private:

    CMpMpxEmbeddedPlaybackHelper( TUid aHostUid,
                                  MMPXPlaybackUtility* aPlaybackUtility,
                                  MMpMpxEmbeddedPlaybackHelperObserver* aObserver );
    void ConstructL();

    void DoHandleCollectionMessageL( const CMPXMessage& aMsg );

private:
    MMPXPlaybackUtility                   *iPlaybackUtility;
    MMPXCollectionUtility                 *iCollectionUtility;   //Owned
    MMPXCollectionUiHelper                *iCollectionUiHelper;  //Owned
    MMpMpxEmbeddedPlaybackHelperObserver  *iObserver;
    TUid                                  iHostUid;
};

#endif // CMPMPXEMBEDDEDPLAYBACKHELPER_H

