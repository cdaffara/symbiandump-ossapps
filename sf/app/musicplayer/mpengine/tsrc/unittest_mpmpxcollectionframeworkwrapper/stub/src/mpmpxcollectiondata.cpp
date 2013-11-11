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
* Description: MpMpxCollectionData stub for testing mpmpxframeworkwrapper
*
*/

#include "mptrace.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>


/*!
 Stub function.
 */
MpMpxCollectionData::MpMpxCollectionData()
    :mContainerMedia(0),
    mMediaArray(0),
    mMediaSet(EFalse),
    mContext (ECollectionContextUnknown),
    mCurrentAlbumIndex(0),
    mAlbumSongCount(0)
{
}

/*!
 Stub function.
*/
MpMpxCollectionData::~MpMpxCollectionData()
{
}

/*!
 Stub function.
*/
TCollectionContext MpMpxCollectionData::context()
{
    return mContext;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::count() const
{
    return 0;
}

/*!
 Stub function.
 */
QString MpMpxCollectionData::collectionTitle() const
{
    QString title;
    if ( mContainerMedia && mContainerMedia->IsSupported( KMPXMediaGeneralTitle ) ) {
        const TDesC& titleText = mContainerMedia->ValueText( KMPXMediaGeneralTitle );
        if ( titleText.Compare( KNullDesC ) != 0 ) {
            title = QString::fromUtf16( titleText.Ptr(), titleText.Length() );
        }
    }
    return title;
}
/*!
 Stub function.
*/
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    Q_UNUSED(index);
    if ( type == MpMpxCollectionData::Uri ) {
        return QString("Uri");
    }
    else {
        return QString();
    }
}

/*!
 Stub function.
 */
int MpMpxCollectionData::itemId( int index )
{
    CMPXMedia* currentMedia( mMediaArray->AtL( index ) );
    Q_ASSERT( currentMedia->IsSupported( KMPXMediaGeneralId ) );
    return currentMedia->ValueTObjectL<TInt>( KMPXMediaGeneralId );
}

/*!
 Stub function.
 */
int MpMpxCollectionData::currentAlbumIndex() const
{
    return mCurrentAlbumIndex;
}

/*!
 Stub function.
*/
void MpMpxCollectionData::setMpxMedia( const CMPXMedia& entries, bool reopen)
{
    Q_UNUSED(reopen);
    delete mContainerMedia;
    mContainerMedia = 0;
    mContainerMedia = CMPXMedia::NewL(entries);
    mMediaArray = const_cast<CMPXMediaArray*>(mContainerMedia->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    mMediaSet = ETrue;
}

/*!
 Stub function.
 */
void MpMpxCollectionData::incrementalOpenUpdate()
{
}

/*!
 Stub function.
*/
const CMPXMedia& MpMpxCollectionData::containerMedia()
{
    return *mContainerMedia;
}

/*!
 Stub function.
*/
void MpMpxCollectionData::setAlbumContent( const CMPXMedia& albumContent )
{
    //CMPXMediaArray* songArray(const_cast<CMPXMediaArray*>( albumContent.Value<CMPXMediaArray>(
    //                KMPXMediaArrayContents ) ) );
    CMPXMediaArray* songArray = albumContent.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( songArray );

    // Save the songs to the album so that we don't need to find them again
    // if the same album is selected again.
    mAlbumSongCount = songArray->Count();

    if ( mAlbumSongCount ) {
        CMPXMedia* albumMedia( mMediaArray->AtL( mCurrentAlbumIndex ) );
        albumMedia->SetCObjectValueL(KMPXMediaArrayContents, songArray);
        albumMedia->SetTObjectValueL<TInt>(KMPXMediaArrayCount, mAlbumSongCount);
    }
}

//end of file
