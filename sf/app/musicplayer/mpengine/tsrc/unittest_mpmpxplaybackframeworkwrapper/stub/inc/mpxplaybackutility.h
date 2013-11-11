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
* Description: mpxplaybackutility stub for testing mpmpxframeworkwrapper
*
*/


#ifndef MMPXPLAYBACKUTILITY_H
#define MMPXPLAYBACKUTILITY_H

#include <mpxplaybackframeworkdefs.h>
#include <mpxcommonframeworkdefs.h>
#include <badesca.h>
#include <mpxattribute.h>
#include <mpxattributespecs.h>

//Forward declarations
class CMPXCollectionPlaylist;
class MMPXPlaybackObserver;  
class MMPXPlaybackCallback;

class MMPXSource
{
public:
    //MMPXSource interface for playback utility stub
    virtual CMPXCollectionPlaylist* PlaylistL()=0;
    virtual void MediaL(const TArray<TMPXAttribute>& aAttrs,
                    MMPXPlaybackCallback& aCallback)=0;

 
};

class MMPXPlaybackUtility: public MMPXSource
{
public:

    // Stub functions
    MMPXPlaybackUtility();
    ~MMPXPlaybackUtility();
    static MMPXPlaybackUtility* UtilityL(const TUid& aModeId = KPbModeDefault);
    void AddObserverL(MMPXPlaybackObserver& aObs);
    void RemoveObserverL(MMPXPlaybackObserver& aObs);
    void Close();
    void CommandL(TMPXPlaybackCommand aCmd,TInt aData=0);
    void CommandL(CMPXCommand& aCmd, MMPXPlaybackCallback* aCallback=NULL);
    void PropertyL(MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty);
    MMPXSource* Source();
     void SetL(TMPXPlaybackProperty aProperty, TInt aValue);
    TMPXPlaybackState StateL() const;
    // Stub function from MMPXSource
    CMPXCollectionPlaylist* PlaylistL();
    void MediaL(const TArray<TMPXAttribute>& aAttrs,
                    MMPXPlaybackCallback& aCallback);
    void InitL(RFile& aShareableFile);
    void InitL(const TDesC& aUri,const TDesC8* aType=NULL);
    

public:

    TInt                    iValue;
    TInt                    iComandData;
    TMPXPlaybackProperty    iProperty;
    MMPXPlaybackObserver*   iObs;
    TMPXPlaybackCommand     iCmd;
    CMPXCommand*            iCMPXCmd;
    TMPXPlaybackState       iState;
    RArray<TMPXAttribute>*  iAttrs;
    CMPXCollectionPlaylist* iCurrentPlaylist;
    bool                    iInitialized;
    bool                    iReturnSource;
    bool                    iSetPlaylistEnd;
};

#endif      // MMPXPLAYBACKUTILITY_H

