// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include "cimapstore.h"

#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

// IMAP STORE command
_LIT8(KCommandStore, "%d UID STORE %S %S %S\r\n");
_LIT8(KUseSilent, ".SILENT");

CImapStore*  CImapStore::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC8& aSequenceSet, const TDesC8& aMessageDataItems, const TDesC8& aValue, TBool aUseSilent, RArrayMessageFlagInfo& aOutMessageFlagInfo)
	{
	CImapStore* self = new (ELeave) CImapStore(aSelectedFolderData, aLogId, aOutMessageFlagInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceSet, aMessageDataItems, aValue, aUseSilent);
	CleanupStack::Pop(self);
	return self;
	}

CImapStore::CImapStore(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayMessageFlagInfo& aOutMessageFlagInfo) 
	: CImapFetchFlags(aSelectedFolderData, aLogId, aOutMessageFlagInfo)
	{}

CImapStore::~CImapStore()
	{
	delete iMessageDataItems;
	delete iValue;
	}
	
void CImapStore::ConstructL(const TDesC8& aSequenceSet, const TDesC8& aMessageDataItems, const TDesC8& aValue, TBool aUseSilent)
	{
	CImapFetchFlags::ConstructL(aSequenceSet);
	
	iMessageDataItems = aMessageDataItems.AllocL();
	iValue = aValue.AllocL();	
	iUseSilent = aUseSilent;
	}

/** 
Formats and sends the IMAP STORE command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be sent to the server
*/	
void CImapStore::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	TInt bufLength = KCommandStore().Length();
	bufLength += TagLength(aTagId);
	bufLength += iSequenceSet->Length();
	
	if(iUseSilent)
		//Append .SILENT option to message_data_items.
		{
		iMessageDataItems = iMessageDataItems->ReAlloc(iMessageDataItems->Length() + KUseSilent().Length());
		iMessageDataItems->Des().Append(KUseSilent);
		}
		
	bufLength += iMessageDataItems->Length();
	bufLength += iValue->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);
	iOutputBuffer->Des().Format(KCommandStore, aTagId, iSequenceSet, iMessageDataItems, iValue);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);		
	}
