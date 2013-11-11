/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  handler for saving and deleting flash messages
 *
 */

#include "flashmsgnotifier_p.h"
#include <xqconversions.h>
#include <msvids.h>
#include <txtrich.h>
#include <smuthdr.h>
#include <debugtraces.h>
#include <ccsrequesthandler.h>
#include <tmsvsmsentry.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// FlashMsgNotifierPrivate::FlashMsgNotifierPrivate
// constructor.
// ---------------------------------------------------------
//
FlashMsgNotifierPrivate::FlashMsgNotifierPrivate(FlashMsgNotifier* notifier) :
    iMsvSession(NULL), iRequestHandler(NULL),iNotifier(notifier)
{
    InitL();
}

// ---------------------------------------------------------
// FlashMsgNotifierPrivate::~FlashMsgNotifierPrivate
// Destructor.
// ---------------------------------------------------------
//
FlashMsgNotifierPrivate::~FlashMsgNotifierPrivate()
{
    if (iMsvSession) {
        delete iMsvSession;
        iMsvSession = NULL;
    }
    if(iRequestHandler) {
        delete iRequestHandler;
        iRequestHandler = NULL;
    }
    iNotifier = NULL;

}

// ---------------------------------------------------------
// InitL( )
// @see header
// ---------------------------------------------------------
void FlashMsgNotifierPrivate::InitL()
{
    iMsvSession = CMsvSession::OpenSyncL(*this);
    iRequestHandler = CCSRequestHandler::NewL();
}

// ---------------------------------------------------------
// FlashMsgNotifierPrivate::HandleSessionEventL()
// @see header
// ---------------------------------------------------------
//
void FlashMsgNotifierPrivate::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/,
    TAny* /*aArg2*/, TAny* /*aArg3*/)
{

}

// ---------------------------------------------------------
// FlashMsgNotifierPrivate::DeleteMessage()
// @see header
// ---------------------------------------------------------
//
void FlashMsgNotifierPrivate::DeleteMessage(TMsvId aMsgId)
{
    iMsvSession->RemoveEntry(aMsgId);
}

// ---------------------------------------------------------
// FlashMsgNotifierPrivate::SaveMessageL()
// @see header
// ---------------------------------------------------------
//
void FlashMsgNotifierPrivate::SaveMessageL(TMsvId aMsgId)
{
    QDEBUG_WRITE_FORMAT("SaveMessageL Begin", aMsgId);
    
    CMsvEntry* msgEntry = iMsvSession->GetEntryL(aMsgId);
    CleanupStack::PushL( msgEntry );
    
    TMsvEntry entry = msgEntry->Entry();
    
    // If the entry is readonly we need to make it read...
    if(entry.ReadOnly())
    {
        entry.SetReadOnly(EFalse);
        entry.SetUnread(EFalse);
		entry.SetNew(EFalse);
        msgEntry->ChangeL(entry);                
    }
    
    CPlainText* nullString = CPlainText::NewL();
    CleanupStack::PushL( nullString );

    CSmsHeader* smsheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *nullString);
    CleanupStack::PushL( smsheader );
    
    CMsvStore* store = msgEntry->EditStoreL();
    CleanupStack::PushL( store );
    
    smsheader->RestoreL(*store);
    smsheader->Message().SmsPDU().SetClass(ETrue, TSmsDataCodingScheme::ESmsClass1 );
    smsheader->StoreL( *store );
    store->Commit();
    
    TMsvSmsEntry entry1 = msgEntry->Entry();
    entry1.SetClass(ETrue, TSmsDataCodingScheme::ESmsClass1 );            
    msgEntry->ChangeL(entry1);
    CleanupStack::PopAndDestroy(4);
    QDEBUG_WRITE_FORMAT("SaveMessageL End", aMsgId);
}

// ---------------------------------------------------------
// FlashMsgNotifierPrivate::GetConversationIdFromAddressL()
// @see header
// ---------------------------------------------------------
//
TInt FlashMsgNotifierPrivate::GetConversationIdFromAddressL(
        TDesC& aContactAddress)
{
    return 
    iRequestHandler->GetConversationIdFromAddressL(aContactAddress);
}

// End of file
