/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mpxcollectionuihelper stub for testing mpmpxframeworkwrapper
*
*/


#include <mpxmedia.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionplugin.hrh>
#include <e32debug.h>

#include "stub/inc/mpxcollectionuihelper.h"
const TInt KMPXPlaylistViewIndex = 1;


/*!
 Stub function.
*/
MMPXCollectionUiHelper::MMPXCollectionUiHelper():iIncAdd(EFalse),
                                                 iDelete(EFalse),
                                                 iCancel(EFalse),
                                                 iMedia(0),
                                                 iValidRename(EFalse),
                                                 iValidReorder(EFalse)
{
}

/*!
 Stub function.
*/
MMPXCollectionUiHelper::~MMPXCollectionUiHelper()
{   
}

/*!
 Stub function.
*/
void MMPXCollectionUiHelper::Close()
{
    delete this;
}

/*!
 Stub function.
*/
void MMPXCollectionUiHelper::IncAddL( const CMPXMedia& aMedia,
                          MMPXCHelperObserver* aObserver,
                          const TInt aSize)
{
    Q_UNUSED(aObserver);
    Q_UNUSED(aSize);
    iIncAdd = ETrue;
    delete iMedia;
    iMedia = 0;
    iMedia = CMPXMedia::NewL(aMedia);
}

/*!
 Stub function.
*/
void MMPXCollectionUiHelper::RenameL( const CMPXMedia& aMedia,
                              MMPXCHelperObserver* aObserver )
{
    Q_UNUSED(aObserver);
    iValidRename = EFalse;
    TMPXGeneralType mediaType =
            aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    TMPXGeneralCategory mediaCategory =
            aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);
    
    if ( mediaType == EMPXItem && mediaCategory == EMPXPlaylist ){
        iValidRename = ETrue;
    }
    iPlaylistId = aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    const TDesC& title = aMedia.ValueText(KMPXMediaGeneralTitle);
    iRenameTitle = QString::fromUtf16(title.Ptr(), title.Length());
}

/*!
 Stub function.
*/
void MMPXCollectionUiHelper::ReorderPlaylistL(const TMPXItemId& aPlaylistId,
                                  const TMPXItemId& aSongId,
                                  TUint aOriginalOrdinal,
                                  TUint aNewOrdinal,
                                  MMPXCHelperObserver* aObserver)
{
    Q_UNUSED(aOriginalOrdinal);
    Q_UNUSED(aNewOrdinal);
    Q_UNUSED(aObserver);
    iValidReorder = aPlaylistId != aSongId;
}

/*!
 Stub function.
*/
void MMPXCollectionUiHelper::DeleteL( CMPXCollectionPath& aPath,
                          MMPXCHelperObserver* aObserver )
{
    Q_UNUSED(aPath);
    Q_UNUSED(aObserver);
    iDelete = ETrue;
}

/*!
 Stub function.
*/
CMPXCollectionPath* MMPXCollectionUiHelper::MusicMenuPathL()
{
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( TUid::Uid(EMPXCollectionPluginMusic).iUid );
    path->AppendL( KMPXPlaylistViewIndex );
    //Fake some elements for the path to make it unique
    for(TInt i=0; i<10; i++){
        path->InsertL(TUid::Uid(EMPXCollectionPluginMusic).iUid,0);
    }
    CleanupStack::Pop( path );
        
    return path;
}

/*!
 Stub function.
*/
CMPXCollectionPath* MMPXCollectionUiHelper::MusicAllSongsPathL()
{
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( TUid::Uid(EMPXCollectionPluginMusic).iUid );
    path->AppendL( KMPXPlaylistViewIndex );
    //Fake some elements for the path to make it unique
    for(TInt i=0; i<5; i++){
        path->InsertL(TUid::Uid(EMPXCollectionPluginMusic).iUid,0);
    }
    CleanupStack::Pop( path );
        
    return path;
}

/*!
 Stub function.
*/
CMPXCollectionPath* MMPXCollectionUiHelper::MusicPlaylistPathL()
{
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( TUid::Uid(EMPXCollectionPluginMusic).iUid );
    path->AppendL( KMPXPlaylistViewIndex );
    //Fake some elements for the path to make it unique
    for(TInt i=0; i<3; i++){
        path->InsertL(TUid::Uid(EMPXCollectionPluginMusic).iUid,0);
    }
    CleanupStack::Pop( path );
        
    return path;
}

/*!
 Stub function.
*/
void MMPXCollectionUiHelper::Cancel()
{
    iCancel = ETrue;
}

//end of file
