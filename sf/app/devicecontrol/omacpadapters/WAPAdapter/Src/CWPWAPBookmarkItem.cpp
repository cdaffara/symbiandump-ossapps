/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  One bookmark item.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpwapadapterresource.rsg>
#include <CWPAdapter.h>
#include "CWPWAPBookmarkItem.h"
#include "WPWAPUtil.h"

// CONSTANTS
const TInt KUrlMaxLength = 255;
const TInt KNameMaxLength = 50;

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::CWPWAPBookmarkItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPBookmarkItem::CWPWAPBookmarkItem( const TDesC& aTitle, 
    const TDesC& aDefaultName, 
    CWPCharacteristic*& aLink)
: iTitle( aTitle ), iDefaultName( aDefaultName ), iLink( aLink )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::ConstructL( CWPCharacteristic& aCharacteristic )
    {
    aCharacteristic.AcceptL( *this );
    if( iName == KNullDesC )
        {
        iName.Set( iDefaultName );
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWAPBookmarkItem* CWPWAPBookmarkItem::NewL(  const TDesC& aTitle, 
    const TDesC& aDefaultName, 
    CWPCharacteristic& aCharacteristic, 
    CWPCharacteristic*& aLink)
    {
    CWPWAPBookmarkItem* self = new(ELeave) CWPWAPBookmarkItem( aTitle, 
        aDefaultName, aLink); 
    CleanupStack::PushL(self);
    self->ConstructL( aCharacteristic );
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CWPWAPBookmarkItem::~CWPWAPBookmarkItem()
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPBookmarkItem::Name() const
    {
    return iName;
    }


// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::Title
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPBookmarkItem::Title() const
    {
    return iTitle;
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::Title
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPBookmarkItem::Address() const
    {
    return iURI;
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::SaveID
// -----------------------------------------------------------------------------
//
const TDesC8& CWPWAPBookmarkItem::SaveData() const
    {
    if( iSaveItem().iSaveItemType != KWPWAPItemTypeNone )
        {
        return iSaveItem;
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::AddL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::AddL( RPointerArray<MWPWAPItemBase>& aShownItems,
                              RPointerArray<MWPWAPItemBase>& /*aHiddenItems*/ )
    {
    User::LeaveIfError( aShownItems.Append( this ) );
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::SaveL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::SaveL()
    {
    TPckgBuf<TUint32> uid;
    if( iLink && iLink->Data().Length() == uid.MaxLength() )
        {
        uid.Copy( iLink->Data() );
        iUID = uid();
        }

    CreateDbL();
    
    //CFavouritesItem* item = CFavouritesItem::NewL();
    //CleanupStack::PushL( item );
    
    //item->SetType( CFavouritesItem::EItem );
    //item->SetParentFolder( KFavouritesRootUid );
    //item->SetNameL( iName );
    //item->SetUrlL( iURI ); 
    
    //if( iUID != KWPWAPNoUID )
    //    {
    //    TFavouritesWapAp ap;
    //    ap.SetApId( iUID );
    //    item->SetWapAp( ap );
    //    } 
    //TInt err( iBookmarkDb.Add( *item, ETrue ) );
    //if( err == KErrLocked )
    //    {
     //   err = EWPBookmarksLocked;
    //    }
    //User::LeaveIfError( err );
    //iSaveItem().iSaveItemType = KWPWAPItemTypeBookmark;
    //iSaveItem().iUID = item->Uid();
    
    CleanupStack::PopAndDestroy(); // item
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPWAPBookmarkItem::CanSetAsDefault() const
    {
    // Startpage support. This can be set as default if this is to be 
    // interpreted as startpage.
    return iHasStartPage;
    }


// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::SetAsDefaultL()
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::ValidateL
// -----------------------------------------------------------------------------
//
TBool CWPWAPBookmarkItem::ValidateL()
    {
    return iURI.Length() <= KUrlMaxLength
        && iName.Length() <= KNameMaxLength
        && iURI.Length() > 0
        && WPWAPUtil::CheckURI( iURI );
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::VisitL( CWPCharacteristic& /*aCharacteristic*/ )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::VisitL( CWPParameter& aParameter )
    {
    switch( aParameter.ID() )
        {
        case EWPParameterURI:
            {
            iURI.Set( aParameter.Value() );
            break;
            }

        case EWPParameterName:
            {
            iName.Set( aParameter.Value() );
            break;
            }

        case EWPParameterStartPage:
            {
            iHasStartPage = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::VisitLinkL( CWPCharacteristic& /*aLink*/ )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBookmarkItem::CreateDbL
// -----------------------------------------------------------------------------
//
void CWPWAPBookmarkItem::CreateDbL()
    {
    //User::LeaveIfError( iSession.Connect() );
    //User::LeaveIfError( iBookmarkDb.Open( iSession, KBrowserBookmarks ) );
    }

//  End of File  
