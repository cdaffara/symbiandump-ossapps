// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: SendAsStates.cpp $
// $Author: Stevep $
// $Revision: 2 $
// $Date: 27/03/02 10:44 $
// 
//

#include "sendasstates.h"

// System includes
#include <sendas.h>             // CSendAs
#include <mtmdef.h>             // Message part constant bitmasks for a TMsvPartList value. 


// User includes
#include "obexharness.h"        // CObexClientTest
#include "obextestutils.h"      // CObexTestUtils

_LIT(KObexSendAsPanicLiteral, "Obex SendAs States");               // literal for panic

//
// CObexSendAsCreateState
//

CObexSendAsCreateState::CObexSendAsCreateState(CObexClientTest& aClientTest) :
 iClientTest(aClientTest)    
    {
    }

void CObexSendAsCreateState::StartL(TRequestStatus& aStatus)
    {
    if(!iClientTest.SendAs())
        iClientTest.CreateSendAsL();

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CObexSendAsAddRecipientState
//

CObexSendAsAddRecipientState::CObexSendAsAddRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iRecipient(aRecipient)    
    {
    }

void CObexSendAsAddRecipientState::StartL(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));

    iClientTest.SendAs()->AddRecipientL(iRecipient);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }

//
// CObexSendAsAddBTRecipientState
//

TUint8 CObexSendAsAddBTRecipientState::GetByteFromUnicodeHexTextChar(const TUint16 *input)
	{
	TUint8 result = (TUint8)*input;

	if(result >='0' && result <='9')
		result = (TUint8)(result - '0');
	else if(result >='a' && result <='f')
		result = (TUint8)(result - 'a' + 10);
	else if(result >='A' && result <='F')
		result = (TUint8)(result - 'A' + 10);
	else
		result = 0;

	return result;
	}

CObexSendAsAddBTRecipientState::CObexSendAsAddBTRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest) :
 iClientTest(aClientTest)
    {
		// convert the incoming string from 12 unicode chars to 3 by
		// converting 4 chars into 1 thus:

		// first 4 input chars:		'0' '0' '0' '2'
		// output char				0x0002

	// Convert first addresse from 16bit descriptor to 8bit descriptor containing 48bit BT device address
	TPtrC16 ptr16 = aRecipient;
	int ii;

	for(ii=0;ii<12;ii+=4)
		{
		// do 4 characters to get one out. We're going to force the compiler to do the character endianness.
		TUint8 byte1 = GetByteFromUnicodeHexTextChar(&ptr16[ii]);
		TUint8 byte2 = GetByteFromUnicodeHexTextChar(&ptr16[ii+1]);
		TUint8 byte3 = GetByteFromUnicodeHexTextChar(&ptr16[ii+2]);
		TUint8 byte4 = GetByteFromUnicodeHexTextChar(&ptr16[ii+3]);

		byte1 = (TUint8) ( ((byte1 & 0x0F) << 4) | (byte2 & 0x0f) );
		byte3 = (TUint8) ( ((byte3 & 0x0F) << 4) | (byte4 & 0x0f) );

		// make sure this is put together in network endianness
		TUint16 result = (TUint16)( (TUint16)(byte3 << 8) | byte1 );

		iRecipient.Append( result );
		}

    }

void CObexSendAsAddBTRecipientState::StartL(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));

    iClientTest.SendAs()->AddRecipientL(iRecipient);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }

//
// CObexSendAsSetSubjectState
//

CObexSendAsSetSubjectState::CObexSendAsSetSubjectState(const TDesC& aSubject, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iSubject(aSubject)
    {
    }

void CObexSendAsSetSubjectState::StartL(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));

    iClientTest.SendAs()->SetSubjectL(iSubject);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CObexSendAsAddAttachmentState
//

CObexSendAsAddAttachmentState::CObexSendAsAddAttachmentState(const TDesC& aFileName, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iFileName(aFileName)
    {
    }

void CObexSendAsAddAttachmentState::StartL(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));
    
    TMsvId id;
    TFileName filename;
    
    iClientTest.SendAs()->CreateAttachmentL(id, filename);

    CFileMan* fileMan = CFileMan::NewL(iClientTest.ObexTestUtils().iMsvSession->FileSession());
    CleanupStack::PushL(fileMan);                                               // PUSH
    User::LeaveIfError(fileMan->Copy(iFileName, filename));
    CleanupStack::PopAndDestroy(); // fileMan                                   // POP(1)

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CObexSendAsAbandonState
//

CObexSendAsAbandonState::CObexSendAsAbandonState(CObexClientTest& aClientTest) :
 iClientTest(aClientTest)    
    {
    }

void CObexSendAsAbandonState::StartL(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));

    iClientTest.SendAs()->AbandonMessage();
    
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CObexSendAsSaveState
//

CObexSendAsSaveState::CObexSendAsSaveState(CObexClientTest& aClientTest) :
 iClientTest(aClientTest)    
    {
    }

void CObexSendAsSaveState::StartL(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));

    iClientTest.SendAs()->SaveMessageL();   //no real benefit in using the async version of this function
    
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CObexSendAsValidateState
//

CObexSendAsValidateState::CObexSendAsValidateState(CObexClientTest& aClientTest) :
 iClientTest(aClientTest)    
    {
    }

void CObexSendAsValidateState::StartL(TRequestStatus& aStatus)
    {
//CBaseMtm::ValidateMessage currently only validates the recipient part of the message but, hey, that may change.
    __ASSERT_ALWAYS( iClientTest.SendAs(), User::Panic(KObexSendAsPanicLiteral, KErrNotReady));

    TMsvPartList partlist = iClientTest.SendAs()->ValidateMessage();

    if(partlist != 0)
        {
		_LIT(KOpeningMessage, "\nThe following parts of the SendAs message are invalid:");
        iClientTest.LogCommentL(KOpeningMessage);
        
        if(partlist & KMsvMessagePartAttachments)
			{
			_LIT(KAttachments, "KMsvMessagePartAttachments.");
            iClientTest.LogCommentL(KAttachments);
			}

        if(partlist & KMsvMessagePartBody)
  			{
			_LIT(KBody, "KMsvMessagePartBody.");
			iClientTest.LogCommentL(KBody);
			}

        if(partlist & KMsvMessagePartDate)
   			{
			_LIT(KDate, "KMsvMessagePartDate.");
			iClientTest.LogCommentL(KDate);
			}

        if(partlist & KMsvMessagePartDescription)
   			{
			_LIT(KDescription, "KMsvMessagePartDescription.");
            iClientTest.LogCommentL(KDescription);
			}

        if(partlist & KMsvMessagePartOriginator)
   			{
			_LIT(KOriginator, "KMsvMessagePartOriginator.");
            iClientTest.LogCommentL(KOriginator);
			}

        if(partlist & KMsvMessagePartRecipient)
   			{
			_LIT(KRecipient, "KMsvMessagePartRecipient.");
            iClientTest.LogCommentL(KRecipient);
			}

        }
    else
		{
		_LIT(KMessageValid, "\nThe SendAs message is valid.");
        iClientTest.LogCommentL(KMessageValid);
		}
    
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }

