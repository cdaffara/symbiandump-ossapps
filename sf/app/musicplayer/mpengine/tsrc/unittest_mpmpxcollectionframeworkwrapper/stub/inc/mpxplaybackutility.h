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
class CMPXCollectionPath;

class MMPXSource
{
public:
    // Stub functions
    virtual CMPXCollectionPlaylist* PlaylistL()=0;

};
class MMPXPlaybackUtility : MMPXSource
{
public:

    // Stub functions
    MMPXPlaybackUtility();
    ~MMPXPlaybackUtility();
    static MMPXPlaybackUtility* UtilityL(const TUid& aModeId = KPbModeDefault);
    void Close();
    void InitL(const CMPXCollectionPlaylist& aPlaylist, TBool aPlay=ETrue);
    void InitL(const TDesC& aUri, const TDesC8* aType=NULL);
    void SetL(TMPXPlaybackProperty aProperty, TInt aValue);
    CMPXCollectionPlaylist* PlaylistL();
    MMPXSource* Source();

public:

    TInt                    iShuffle;
    TInt                    iRepeat;
    TMPXPlaybackProperty    iProperty;
    TBool                   iPlay;
    CMPXCollectionPath      *iCurrentPath;
    CMPXCollectionPlaylist  *iCurrentPlaylist;
};

#endif      // MMPXPLAYBACKUTILITY_H

