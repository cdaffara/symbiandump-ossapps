/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Request fetcher implementation.
*
*/



#include "BasePlugin.h"


/**
 * Note that aPreferredByteCount is ignored.
 */
EXPORT_C void CBasePlugin::FetchMessagePartsL(
    const TFSMailMsgId& aMailBoxId,
	const TFSMailMsgId& /*aFolderId*/,
	const TFSMailMsgId& aMessageId,
    const RArray<TFSMailMsgId>& aMessagePartIds,
	MFSMailRequestObserver& aOperationObserver,
	const TInt aRequestId,
	const TUint /*aPreferredByteCount*/ )
    
    {
    CFetchRequester* req = CFetchRequester::NewL(   
        *this, aMailBoxId, aMessageId, aMessagePartIds, aOperationObserver, aRequestId );
    CleanupStack::PushL( req );
    iReqs.AppendL( req );
    CleanupStack::Pop( req );
    }


#pragma mark CFetchRequester


/**
 *
 */
CFetchRequester::CFetchRequester(
    CBasePlugin& aPlugin,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    MFSMailRequestObserver& aObserver,
    TInt aRequestId )
    :
    iPlugin( aPlugin ),
    iObserver( aObserver ),
    iMailBoxId( aMailBoxId ),
    iMessageId( aMessageId ),
    iRequestId( aRequestId )
    {
    }


/**
 *
 */
CFetchRequester* CFetchRequester::NewL(
    CBasePlugin& aPlugin,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& iMessageId,
    const RArray<TFSMailMsgId>& aMessages,
    MFSMailRequestObserver& aObserver,
    TInt aRequestId )
   
    {
    CFetchRequester* self = new (ELeave) CFetchRequester(
        aPlugin, aMailBoxId, iMessageId, aObserver, aRequestId );
   
    CleanupStack::PushL( self );
    self->ConstructL( aMessages );
    CleanupStack::Pop( self );
   
    return self;
    }


/**
 *
 */
void CFetchRequester::ConstructL( const RArray<TFSMailMsgId>& aMessages )
    {
    __LOG_CONSTRUCT( "baseplugin", "CFetchRequester" )
   
    __LOG_ENTER( "ConstructL." );

    __LOG_WRITE_FORMAT1_INFO( "iMailBoxId = 0x%X.", iMailBoxId.Id() );
    __LOG_WRITE_FORMAT1_INFO( "iMessageId = 0x%X.", iMessageId.Id() );
    __LOG_WRITE_FORMAT1_INFO( "iRequestId = 0x%X.", iRequestId );

   TInt count = aMessages.Count();
   
   for ( TInt i = 0; i < count; i++ )
      {
      iParts.AppendL( aMessages[i] );
      __LOG_WRITE_FORMAT1_INFO( "iMessages[%d] =", i );
      __LOG_WRITE_FORMAT1_INFO( "0x%X.", aMessages[i].Id() );
      }   
   
   __LOG_EXIT;
   }


/**
 *
 */
CFetchRequester::~CFetchRequester() 
    {
    iParts.Close();
    __LOG_DESTRUCT
    }
