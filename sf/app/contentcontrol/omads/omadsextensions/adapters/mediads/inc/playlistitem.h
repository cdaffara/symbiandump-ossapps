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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#ifndef PLAYLISTITEM_H_
#define PLAYLISTITEM_H_

#include <e32base.h>
#include <s32mem.h>
#include <BADESCA.H>

class CPlaylistItem : public CBase
    {
public:
    static CPlaylistItem* NewLC();
    ~CPlaylistItem();
    
    void SetId( TInt aId );
    TInt Id() const;
    
    void SetTitleL( const TDesC& aTitle );
    const TDesC& Title() const;
    
    void SetUrlL( const TDesC& aUrl );
    const TDesC& Url() const;
    
    void AddItemL( const TDesC& aSongUri );
    TInt ItemCount() const;
    TPtrC16 ItemAt( TInt aIndex ) const;
    TInt FindItem( const TDesC16& aPtr, TInt& aPos, TKeyCmpText aTextComparisonType=ECmpFolded ) const;
    
    void ExportL( RBufWriteStream& aStream ) const;
    void ImportL( const TDesC& aBuffer );
    
private:
    CPlaylistItem();
    void ConstructL();
    
private:
    
    TInt ReadNextLine( const TDesC& aBuffer, TPtrC& aLine );
    
private:
    TInt iId;
    HBufC* iTitle;
    HBufC* iUrl;

    CDesCArrayFlat* iDescArray;
    };

#endif /* PLAYLISTITEM_H_ */
