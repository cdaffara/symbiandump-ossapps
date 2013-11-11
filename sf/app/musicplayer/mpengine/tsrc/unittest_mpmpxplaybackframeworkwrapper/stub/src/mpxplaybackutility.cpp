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

#include <Qt>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mptrace.h>

#include "stub/inc/mpxplaybackutility.h"

/*!
 Stub function.
*/
MMPXPlaybackUtility::MMPXPlaybackUtility():iValue(0),
                                           iComandData(0),
                                           iProperty(EPbPropertyVolume),
                                           iObs(0),
                                           iCMPXCmd(0),
                                           iState(EPbStateNotInitialised),
                                           iAttrs(0),
                                           iInitialized(false),
                                           iReturnSource(true),
                                           iSetPlaylistEnd(false),
                                           iCurrentPlaylist(0)
{
}

/*!
 Stub function.
*/
MMPXPlaybackUtility::~MMPXPlaybackUtility()
{
}

/*!
 Stub function.
 */
MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL(const TUid& aModeId)
{
    Q_UNUSED(aModeId);
    static MMPXPlaybackUtility playbackUtilility;
    return &playbackUtilility;
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::AddObserverL(MMPXPlaybackObserver& aObs)
{
    iObs = &aObs;
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::RemoveObserverL(MMPXPlaybackObserver& aObs)
{
   if(iObs == &aObs){
       iObs = 0;
   }   
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::Close()
{
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::CommandL(TMPXPlaybackCommand aCmd,TInt aData)
{
    iComandData = aData;
    iCmd = aCmd;
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::CommandL(CMPXCommand& aCmd, MMPXPlaybackCallback* aCallback)
{
    Q_UNUSED(aCallback);
    if(iCMPXCmd){
        delete iCMPXCmd;
        iCMPXCmd = 0;
    }
    iCMPXCmd = CMPXCommand::NewL(aCmd);
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::PropertyL(MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty)
{
    Q_UNUSED(aCallback);
    iProperty = aProperty;
}

/*!
 Stub function.
*/
MMPXSource* MMPXPlaybackUtility::Source()
{
    if (iReturnSource) {
        return this;
    }
    else {
        return NULL;
    }
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::SetL(TMPXPlaybackProperty aProperty, TInt aValue)
{
    iProperty = aProperty;
    iValue = aValue;
}

/*!
 Stub function.
*/
TMPXPlaybackState MMPXPlaybackUtility::StateL() const
{
    return iState;
}

/*!
 Stub function.
*/
CMPXCollectionPlaylist* MMPXPlaybackUtility::PlaylistL()
{
    CMPXCollectionPath *testPath = CMPXCollectionPath::NewL();
    RArray<TMPXItemId> pathItems;
    CleanupStack::PushL(testPath);
    testPath->AppendL(5);
    for (int i = 0; i < 5; i++ ) {
        pathItems.AppendL( TMPXItemId( 300 + i ) );
    }
    testPath->AppendL( pathItems.Array() );
    //Create testPlaylist
    iCurrentPlaylist = CMPXCollectionPlaylist::NewL( *testPath );
    if (iSetPlaylistEnd){
        iCurrentPlaylist->SetToLast();
    }
    CleanupStack::PopAndDestroy( testPath );
    return iCurrentPlaylist;
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::MediaL(const TArray<TMPXAttribute>& aAttrs,
                    MMPXPlaybackCallback& aCallback)
{
    Q_UNUSED(aCallback);
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);
    for(TInt i = 0; i < aAttrs.Count(); i++){
        attrs.AppendL(aAttrs[i]);
    }
    iAttrs = new RArray<TMPXAttribute>(attrs);
    CleanupStack::PopAndDestroy(&attrs);
   
}

void MMPXPlaybackUtility::InitL(const TDesC& aUri,const TDesC8* aType)
{
    Q_UNUSED(aType);
    if ( aUri.Length() )
    {
        iInitialized = true;
    }
}

void MMPXPlaybackUtility::InitL(RFile& aShareableFile)
{
    Q_UNUSED(aShareableFile)
    iInitialized = true;
}
//end of file
