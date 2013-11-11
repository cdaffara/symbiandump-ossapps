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


#include "cimapcapability.h"
#include "cimapcapabilityinfo.h"
#include "ctransportmanager.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "imappaniccodes.h"

_LIT8(KTxtCapabilityFormat, "%d CAPABILITY\r\n");

/**
The factory constructor. Part of two phased construction.
@param aCapabilityInfo Output parameter that will hold the capability information upon successful completion of this command.
*/
CImapCapability* CImapCapability::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapCapabilityInfo& aCapabilityInfo)
	{
	CImapCapability* self = new(ELeave) CImapCapability(aSelectedFolderData, aLogId, aCapabilityInfo);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**

*/
void CImapCapability::ConstructL()
	{
	}
	
/**
Constructor.
@param aCapabilityInfo Output parameter that will hold the capability information upon successful completion of this command.
*/
CImapCapability::CImapCapability(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapCapabilityInfo& aCapabilityInfo)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iCapabilityInfo(aCapabilityInfo)
	{
	}
		
/**
Destructor.
*/
CImapCapability::~CImapCapability()
	{
	}
	
/**
Responsible for sending the IMAP capability command to the remote server to 
perform the desired action the IMAP client wishes. The data will be sent 
to the remote server on the output stream provided. 
It is assumed the output stream has already been set up and ready to use.

@param aTagId	Used to generate the IMAP tag that identifies the message.
@param aStream	The output stream on which the message will be sent.
*/
void CImapCapability::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	TInt bufferLength = KTxtCapabilityFormat.iTypeLength - 2 + TagLength(aTagId); // -2 for "%d"

	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufferLength);
	iOutputBuffer->Des().Format(KTxtCapabilityFormat, iTagId);
	
	// Send the data on the output stream
	aStream.SendDataReq(*iOutputBuffer);
	}

CImapCommand::TParseBlockResult CImapCapability::ParseUntaggedResponseL()
	{
	TParseBlockResult result = ENotRecognised;
	if (GetNextPart().CompareF(KTxtImapCapability) == 0)
		{
		ParseCapabilityDataL(iUnparsedData);
		result = ECompleteUntagged;
		}
		
	return result;
	}
	
/**
Parses capability-data, which can be received 
as part of an untagged CAPABILITY response,
or as part of the response-code of a tagged LOGIN OK response.
	(i.e. as the data in the square brackets)
It assumes that the "CAPABILITY" item has already been identified
@param the capability-data, but without the "CAPABILITY" string at the front.  And with no square brackets.
*/
void CImapCapability::ParseCapabilityDataL(const TDesC8& aCapabilityData)
	{
	// From RFC3501 section 9
	//
	// capability-data = "CAPABILITY" *(SP capability) SP "IMAP4rev1" *(SP capability)
	// capability = ("AUTH=" auth-type) / atom
	//
	
	// "CAPABILITY" has beem found, so we're just looking for basic space-separated atoms.
	
	// First, reset iCapabilityInfo as it's going to be given some new information.
	iCapabilityInfo.Reset();
		
	RDesParts capabilityItems;
	CleanupClosePushL(capabilityItems);
		
	GetDelimitedPartsL(' ', aCapabilityData, capabilityItems);
	TInt countItems = capabilityItems.Count();
	for (TInt i=0; i<countItems; ++i)
		{
		TPtrC8 capability = capabilityItems[i];
		
		if (capability.CompareF(KImapTxtImapVersion) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EVersion4Rev1, ETrue);
			}
		else if (capability.CompareF(KImapTxtIdle) == 0)
			{			
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EIdle, ETrue);
			}
		else if (capability.CompareF(KImapTxtStartTls) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EStartTls, ETrue);
			}
		else if (capability.CompareF(KImapTxtLoginDisabled) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::ELoginDisabled, ETrue);
			}
		else if (capability.CompareF(KImapTxtAuthPlain) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EAuthPlain, ETrue);
			}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		else if (capability.CompareF(KImapTxtBinaryCap) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EBinaryCap, ETrue);
			}
		else if (capability.CompareF(KImapTxtAuthCRamMd5) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EAuthCramMd5, ETrue);
			}
		else if (capability.CompareF(KImapTxtAuthLogin) == 0)
			{
			iCapabilityInfo.SetFlag(CImapCapabilityInfo::EAuthLogin, ETrue);
			}
#endif
		}
	
	CleanupStack::PopAndDestroy(&capabilityItems);
	}
