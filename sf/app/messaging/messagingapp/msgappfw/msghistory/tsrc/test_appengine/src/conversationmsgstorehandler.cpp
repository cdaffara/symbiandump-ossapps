/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/

#include "conversationmsgstorehandler.h"

// SYSTEM INCLUDES
#include <StringLoader.h>
#include <ccsdefs.h> 
#include <msvids.h>
#include <mmsconst.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------
//
ConversationMsgStoreHandler::ConversationMsgStoreHandler():
    iMsvSession(NULL)
    {
    TRAP_IGNORE(InitL());
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
ConversationMsgStoreHandler::~ConversationMsgStoreHandler()
    {
    if(iMsvSession)
        {
        delete iMsvSession;
        iMsvSession = NULL;
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::InitL( )
    {
    iMsvSession = CMsvSession::OpenSyncL(*this);
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::HandleSessionEventL()
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
        TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    // Nothing to do
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::MsgStatus
// ---------------------------------------------------------
//
TCsMmsNotificationMsgState ConversationMsgStoreHandler::
MmsNotificationStatus( TInt aMsvId )
    {
    TMsvId dummy = 0x0;
    
    TMsvEntry entry;
    iMsvSession->GetEntry(aMsvId, dummy, entry);
    
    TCsMmsNotificationMsgState status = EMsgStatusNull;

    // operationMask includes operation type. It is not bitmap but ordinal number. 
    // It does not include operation status and result
    TInt operationMask = (entry.iMtmData2 & KMmsOperationIdentifier) ;

    // Note! Ongoing operation resets every bit of operation type, operation status
    // and result. E.g. If message has been forwarded and then fetching starts, 
    // information about forwarding is lost

    if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd )
        {
        if(     operationMask == KMmsOperationFetch 
                &&  OperationOngoing( entry ) )
            { 
            // It's in retrieving state
            status = EMsgStatusRetrieving;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationOngoing( entry ) )
            { 
            // It's in forwarding state
            status = EMsgStatusForwarding;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationFinished( entry )
                &&  !( entry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been forwarded succesfully
            status = EMsgStatusForwarded;
            }
        else if(    operationMask == KMmsOperationFetch 
                &&  OperationFinished( entry )
                &&   (  entry.iMtmData2 & KMmsOperationResult 
                ||  entry.iError ) )
            { 
            // Fetch has been failed
            status = EMsgStatusFailed;
            }
        else if(    operationMask == KMmsOperationDelete
                &&  OperationFinished( entry )
                &&  !( entry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been deleted succesfully
            status = EMsgStatusDeleted;
            }
        else 
            {   // Normal waiting state
            status = EMsgStatusReadyForFetching;
            }
        }

    return status;
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::OperationOngoing
// ---------------------------------------------------------
//
TBool ConversationMsgStoreHandler::OperationOngoing( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::OperationFinished
// ---------------------------------------------------------
//
TBool ConversationMsgStoreHandler::OperationFinished( 
    const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::MarkMessagesReadL
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::MarkMessagesReadL(RArray<TInt>& aIdArray)
    {    
    for ( int index = 0; index < aIdArray.Count(); index++ )
        {
        TMsvId id = aIdArray[index];
        CMsvEntry* cEntry = NULL;
        TRAPD(err, cEntry = iMsvSession->GetEntryL(id));
        if ( err == KErrNotFound )
            continue; // Entry is already deleted.
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
            {
            // Mark the entry as read
            entry.SetUnread( EFalse );
            cEntry->ChangeL( entry );
            }
        delete cEntry;
        }
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteMessages
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::DeleteMessages(RArray<TInt>& aIdArray)
{
    for(int index=0;index < aIdArray.Count() ;++index)
    {
        TMsvId id = aIdArray[index];
        iMsvSession->RemoveEntry(id);
    }   
}

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteMessagesL
// ---------------------------------------------------------
//
CMsvSession& ConversationMsgStoreHandler::GetMsvSession()
{
    return *iMsvSession;
}

// End of file
