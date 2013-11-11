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
MMPXPlaybackUtility::MMPXPlaybackUtility():iShuffle(-1),
                                           iRepeat(-1),
                                           iProperty(EPbPropertyVolume),
                                           iPlay(EFalse),
                                           iCurrentPath(0),
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
void MMPXPlaybackUtility::Close()
{
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::InitL(const CMPXCollectionPlaylist& aPlaylist, TBool aPlay)
{
    iCurrentPath = aPlaylist.Path().ContainerPathL();
    iPlay = aPlay;
}

/*!
 Stub function.
*/
void MMPXPlaybackUtility::InitL(const TDesC& aUri, const TDesC8* aType)
{
    Q_UNUSED(aUri);
    Q_UNUSED(aType);
    iPlay = ETrue;
}


/*!
 Stub function.
*/
void MMPXPlaybackUtility::SetL(TMPXPlaybackProperty aProperty, TInt aValue)
{
    iProperty = aProperty;
    if(iProperty == EPbPropertyRandomMode){
        iShuffle = aValue;
        TX_LOG_ARGS("PbUtility shuffle set to "<< aValue);
    }else if (iProperty == EPbPropertyRepeatMode){
        iRepeat = aValue;
        TX_LOG_ARGS("PbUtility repeat set to "<< aValue);
    }    
    
}

/*!
 Stub function.
*/
CMPXCollectionPlaylist* MMPXPlaybackUtility::PlaylistL()
{
    return iCurrentPlaylist;
}

/*!
 Stub function.
*/
MMPXSource* MMPXPlaybackUtility::Source()
{
    return this;
}

//end of file
