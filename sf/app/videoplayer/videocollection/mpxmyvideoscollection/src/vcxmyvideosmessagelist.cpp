/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Groups messages and sends them to clients.*
*/




// INCLUDE FILES
#include <e32cmn.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosuids.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosmessagelist.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosMessageList* CVcxMyVideosMessageList::NewL(
    CVcxMyVideosCollectionPlugin& aCollection )
    {
    CVcxMyVideosMessageList* self = new (ELeave) CVcxMyVideosMessageList( aCollection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosMessageList::~CVcxMyVideosMessageList()
    {    
    delete iMessageList;
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosMessageList::CVcxMyVideosMessageList( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosMessageList::ConstructL ()
    {
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosMessageList::AddEventL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosMessageList::AddEventL( const TMPXItemId& aId,
        TMPXChangeEventType aChange, TInt32 aExtraInfo, CMPXMedia* aVideo )
    {
    MPX_FUNC("CVcxMyVideosMessageList::AddEventL");
    
    CreateMessageListL(); //does nothing if already created

#if _DEBUG
    MPX_DEBUG1("CVcxMyVideosMessageList:: ----------------------------------------------." );

    switch ( aChange )
        {
        case EMPXItemDeleted:
            MPX_DEBUG4("CVcxMyVideosMessageList:: MPXID(%d,%d), aChange = EMPXItemDeleted, aExtraInfo = %d",
                    aId.iId1, aId.iId2, aExtraInfo );
            break;
        case EMPXItemInserted:
            MPX_DEBUG4("CVcxMyVideosMessageList:: MPXID(%d,%d), aChange = EMPXItemInserted, aExtraInfo = %d",
                    aId.iId1, aId.iId2, aExtraInfo );
            break;
        case EMPXItemModified:
            MPX_DEBUG4("CVcxMyVideosMessageList:: MPXID(%d,%d), aChange = EMPXItemModified, aExtraInfo = %d",
                    aId.iId1, aId.iId2, aExtraInfo );
            break;
        }
    
    MPX_DEBUG1("CVcxMyVideosMessageList:: ----------------------------------------------'" );
#endif
    
    if ( aChange == EMPXItemModified && AlreadyInMessageListL( aId, aChange, aExtraInfo ) )
        {
        MPX_DEBUG1("CVcxMyVideosMessageList:: already in message list, skipping add");
        return;
        }
     
    CMPXMessage* message = CMPXMedia::NewL();
    CleanupStack::PushL( message ); // 1->

    message->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
    message->SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));        
    message->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, aChange);        
    message->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, aId);
    message->SetTObjectValueL<TMPXGeneralCategory>( KMPXMessageMediaGeneralCategory, EMPXCollection );
    
    if ( aVideo && aChange == EMPXItemInserted )
        {
        message->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, aVideo );
        }
        
    if ( aExtraInfo )
        {
        message->SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value, aExtraInfo );
        }
    
    MPX_DEBUG1("CVcxMyVideosMessageList:: appending to message array");
    
    iMessageArray->AppendL( message ); // ownership moves

    CleanupStack::Pop( message ); // <-1        

    iMessageCount++;
    iMessageList->SetTObjectValueL<TInt>( KMPXMessageArrayCount, iMessageCount );
    
    if ( iMessageArray->Count() > 100 )
        {
        SendL();
        }
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosMessageList::AddEventL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosMessageList::AddEventL( TInt aEvent )
    {
    TMPXItemId itemId;
    itemId.iId1 = KVcxCommandIdMyVideos;
    CMPXMedia* msg = CMPXMedia::NewL();
    CleanupStack::PushL( msg ); // 1->
    msg->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, itemId );
    msg->SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));        

    msg->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId, aEvent );
    AddL( msg );
    CleanupStack::Pop( msg ); // <-1
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosMessageList::AlreadyInMessageListL
// ----------------------------------------------------------------------------------------------------------
//
TBool CVcxMyVideosMessageList::AlreadyInMessageListL( const TMPXItemId& aId,
        TMPXChangeEventType aChange, TInt32 aExtraInfo )
    {
    TMPXItemId itemId;
    TMPXChangeEventType event;
    TInt32 extraInfo;
    CMPXMedia* media;    
    TInt count = iMessageArray->Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        media = iMessageArray->AtL( i );
        
        if ( media->IsSupported( KMPXMessageMediaGeneralId ) &&
                media->IsSupported( KMPXMessageChangeEventType ) )
            {
            itemId = media->ValueTObjectL<TMPXItemId>( KMPXMessageMediaGeneralId );
            event  = media->ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType );
            
            if ( aId == itemId && aChange == event )
                {
                extraInfo = 0;
                if ( media->IsSupported( KVcxMediaMyVideosInt32Value ) )
                    {
                    extraInfo = media->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
                    }
                    
                if ( extraInfo == EVcxMyVideosVideoListOrderChanged &&
                        aExtraInfo == 0 )
                    {
                    MPX_DEBUG1("CVcxMyVideosMessageList:: found similar message with EVcxMyVideosVideoListOrderChanged flag set, discarding this msg");
                    return ETrue;
                    }
                
                if ( aExtraInfo == EVcxMyVideosVideoListOrderChanged &&
                       extraInfo == 0 )
                    {
                    MPX_DEBUG1("CVcxMyVideosMessageList:: Turning the EVcxMyVideosVideoListOrderChanged flag on to the previous message");
                    MPX_DEBUG1("CVcxMyVideosMessageList:: and discarding this.");
                    media->SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value,
                            EVcxMyVideosVideoListOrderChanged );
                    return ETrue;
                    }
                     
                if ( aExtraInfo == extraInfo )
                    {
                    return ETrue;
                    }
                else
                    {
                    return EFalse;
                    }
                }
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosMessageList::AddL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosMessageList::AddL( CMPXMessage* aMessage )
    {
    if ( !aMessage )
        {
        MPX_DEBUG1("CVcxMyVideosMessageList::AddL aMessage is NULL!!");
        return;
        }

#ifdef _DEBUG
    if ( aMessage->IsSupported( KVcxMediaMyVideosCommandId ) )
        {
        switch ( aMessage->ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId ) )
            {
            case KVcxMessageMyVideosItemsAppended:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosItemsAppended");
                break;

            case KVcxMessageMyVideosListComplete:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosListComplete");
                break;

            case KVcxMessageMyVideosMoveOrCopyStarted:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosMoveOrCopyStarted");
                break;

            case KVcxMessageMyVideosDeleteStarted:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosDeleteStarted");
                break;

            case KVcxMessageMyVideosMoveResp:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosMoveResp");
                break;

            case KVcxMessageMyVideosGetMediasByMpxIdResp:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosGetMediasByMpxIdResp");
                break;

            case KVcxMessageMyVideosCopyResp:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosCopyResp");
                break;

            case KVcxMessageMyVideosDeleteResp:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL adding msg KVcxMessageMyVideosDeleteResp");
                break;
            default:
                MPX_DEBUG1("CVcxMyVideosMessageList::AddL unknown message");
                break;
            }
        }
#endif

    CreateMessageListL(); //does nothing if already created
    iMessageCount++;
    iMessageList->SetTObjectValueL<TInt>( KMPXMessageArrayCount, iMessageCount );
    iMessageArray->AppendL( aMessage ); // ownership moves
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosMessageList::SendL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosMessageList::SendL()
    {
    MPX_DEBUG1("CVcxMyVideosMessageList::SendL");
    
    if ( iMessageCount == 0 || !iMessageList )
        {
        MPX_DEBUG1("CVcxMyVideosMessageList:: iMessageList is empty or NULL, skipping send");
        return;
        }
            
    iCollection.SendMessages( *iMessageList );
    delete iMessageList;
    iMessageList  = NULL;
    iMessageArray = NULL;
    iMessageCount = 0;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosMessageList::CreateMessageListL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosMessageList::CreateMessageListL()
    {
    if ( !iMessageList )
        {
        iMessageList  = TVcxMyVideosCollectionUtil::CreateEmptyMessageListL();
        iMessageArray = iMessageList->Value<CMPXMessageArray>( KMPXMessageArrayContents ); 

        iMessageList->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KVcxCommandIdMyVideos);
        iMessageList->SetTObjectValueL<TInt>(KVcxMediaMyVideosCommandId, KVcxMessageMyVideosMessageArray );
        iMessageList->SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));
        iMessageCount = 0;
        }
    }
// End of file

