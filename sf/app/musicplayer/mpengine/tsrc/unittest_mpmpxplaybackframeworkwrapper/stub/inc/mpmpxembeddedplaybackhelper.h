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
* Description: embedded playback helper stub for testing mpmpxpbframeworkwrapper.
*
*/

#ifndef CMPMPXEMBEDDEDPLAYBACKHELPER_H
#define CMPMPXEMBEDDEDPLAYBACKHELPER_H

#include <QObject>
#include <e32base.h>

// FOWARD DECLARATION
class MMPXPlaybackUtility;

class MMpMpxEmbeddedPlaybackHelperObserver
{
public:

    virtual void HandleEmbeddedPlaybackError( TInt aError ) = 0;

};

class CMpMpxEmbeddedPlaybackHelper : public CBase
{

public:

    static CMpMpxEmbeddedPlaybackHelper* NewL( TUid aHostUid,
                                               MMPXPlaybackUtility* aPlaybackUtility,
                                               MMpMpxEmbeddedPlaybackHelperObserver* aObserver );
    virtual ~CMpMpxEmbeddedPlaybackHelper();

    void playL( const TDesC& aFilename );
    void triggerError();

private:

    CMpMpxEmbeddedPlaybackHelper( TUid aHostUid,
                                  MMPXPlaybackUtility* aPlaybackUtility,
                                  MMpMpxEmbeddedPlaybackHelperObserver* aObserver );

public:
    MMPXPlaybackUtility                   *iPlaybackUtility;
    MMpMpxEmbeddedPlaybackHelperObserver  *iObserver;
    TUid                                  iHostUid;
    QString                               iFileNameToPlay;
};

#endif // CMPMPXEMBEDDEDPLAYBACKHELPER_H

