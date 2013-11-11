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
* Description:  MFSMailIterator implementation.
*
*/



#include <e32std.h>

#include "baseplugincommonutils.h"
#include "MailIterator.h"


/**
 *
 */
HMailIterator::HMailIterator(
    CBasePlugin& aPlugin,
    CMsgStoreSortResultIterator* aIter,
    TFSMailMsgId aMailBox )
    : iIter( aIter ), iPlugin( aPlugin ), iMailBox( aMailBox )
    
    {        
    __LOG_CONSTRUCT( "baseplugin", "HMailIterator" )
    }


/**
 *
 */
/*virtual*/ HMailIterator::~HMailIterator()
    {
    delete iIter;
    __LOG_DESTRUCT
    }


/**
 *
 */
TBool HMailIterator::NextL(
     TFSMailMsgId aCurrentMessageId, 
     TUint aCount,
     RPointerArray<CFSMailMessage>& aMessages )
        
     {
     return DoNextL( &aCurrentMessageId, NULL, aCount, aMessages );
     };

/**
 *
 */
TBool HMailIterator::NextL(
   const TDesC& aStartWith,
   TUint aCount, 
   RPointerArray<CFSMailMessage>& aMessages )

   {
   return DoNextL( NULL, &aStartWith, aCount, aMessages );
   };

/**
 *
 */
TBool HMailIterator::PreviousL(
   TFSMailMsgId aCurrentMessageId, 
   TUint aCount,
   RPointerArray<CFSMailMessage>& aMessages )

   {
   return DoPreviousL( &aCurrentMessageId, NULL, aCount, aMessages );
   };

/**
 *
 */
TBool HMailIterator::PreviousL(
   const TDesC& aStartWith, 
   TUint aCount, 
   RPointerArray<CFSMailMessage>& aMessages )
   
   {
   return DoPreviousL( NULL, &aStartWith, aCount, aMessages );
   };


#pragma mark -
#pragma mark PRIVATE METHODS


/**
 *
 */
TBool HMailIterator::DoNextL(
    TFSMailMsgId* /*ref*/ aCurrentMessageId,
    const TDesC* /*ref*/ aStartWith,
    TUint aCount,
    RPointerArray<CFSMailMessage>& aMessages )

    {
    __LOG_ENTER( "DoNextL" )

    RPointerArray<CMsgStorePropertyContainer> msMsgs;
    CleanupResetAndDestroyClosePushL( msMsgs );

    TBool result = 
        NULL == aStartWith
        ? iIter->NextL( (*aCurrentMessageId).Id(), aCount, msMsgs )
        : iIter->NextL( *aStartWith, aCount, msMsgs );
        
    __LOG_WRITE8_FORMAT1_INFO( "MsgStore's NextL returned %d messages.", msMsgs.Count() )

    for ( TInt i = 0; i < msMsgs.Count(); i++ )
        {
        CFSMailMessage* fsMsg = CFSMailMessage::NewLC(
            TFSMailMsgId( iPlugin.GetPluginId(), msMsgs[i]->Id() ) );

        fsMsg->SetMailBoxId( iMailBox );
        fsMsg->SetContentType( KNullDesC() );
        fsMsg->SetContentDescription( KNullDesC() );
        fsMsg->SetContentDisposition( KNullDesC() );
        
        iPlugin.TranslateMsgStorePropsL( iMailBox, *msMsgs[i], *fsMsg );

        aMessages.AppendL( fsMsg );
        CleanupStack::Pop( fsMsg );
        }

    CleanupStack::PopAndDestroy( &msMsgs );
    __LOG_EXIT
    return result;
    }

    
/**
 *
 */
TBool HMailIterator::DoPreviousL(
    TFSMailMsgId* /*ref*/ aCurrentMessageId,
    const TDesC* /*ref*/ aStartWith,
    TUint aCount,
    RPointerArray<CFSMailMessage>& aMessages )
   
    {
    __LOG_ENTER( "DoPreviousL" )

    RPointerArray<CMsgStorePropertyContainer> msMsgs;
    CleanupResetAndDestroyClosePushL( msMsgs );

    TBool result;
    if ( NULL != aCurrentMessageId )
        {
        TMsgStoreId id = ( 0 == aCurrentMessageId->Id()
            ? KMsgStoreSortResultBottom : aCurrentMessageId->Id() );
            
        result = iIter->PreviousL( id, aCount, msMsgs );
        }
    else
        {
        result = iIter->PreviousL( *aStartWith, aCount, msMsgs );
        }

    for ( TInt i = 0; i < msMsgs.Count(); i++ )
        {
        CFSMailMessage* fsMsg = CFSMailMessage::NewLC(
            TFSMailMsgId( iPlugin.GetPluginId(), msMsgs[i]->Id() ) );
            
        fsMsg->SetMailBoxId( iMailBox );
        fsMsg->SetContentType( KNullDesC() );
        fsMsg->SetContentDescription( KNullDesC() );
        fsMsg->SetContentDisposition( KNullDesC() );

        iPlugin.TranslateMsgStorePropsL( iMailBox, *msMsgs[i], *fsMsg );

        aMessages.AppendL( fsMsg );
        CleanupStack::Pop( fsMsg );
        }

    CleanupStack::PopAndDestroy( &msMsgs );
    __LOG_EXIT
    return result;
    }
