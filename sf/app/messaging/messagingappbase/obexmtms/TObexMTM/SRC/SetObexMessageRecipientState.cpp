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
// $Workfile: CSetObexMessageRecipientState.cpp $
// $Author: ChristopherR $
// $Revision: 5 $
// $Date: 31/01/02 10:07 $
// 
//


#include "SetObexMessageRecipientStates.h"

//TODO:- check that all these headers are required

// System includes
#include <btmsgtypeuid.h>   // KUidMsgTypeBt
#include <btmtmcmds.h>      // TBtMtmAsyncCmds
#include <irmsgtypeuid.h>   // KUidMsgTypeIr
#include <irmtmcmds.h>      // TIrMtmAsyncCmds
#include <msvuids.h>        // KUidMsvMessageEntry

// User includes
#include "obexharness.h"
#include "obextestutils.h"
#include "sendasstates.h"
//
// CSetObexMessageBTRecipientState
//
CSetObexMessageBTRecipientState::CSetObexMessageBTRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest) :
 iClientTest(aClientTest)
    {
	TPtrC16 ptr16 = aRecipient;

	int ii;

	for(ii=0;ii<12;ii+=4)
		{
		// do 4 characters to get one out. We're going to force the compiler to do the character endianness.
		TUint8 byte1 = CObexSendAsAddBTRecipientState::GetByteFromUnicodeHexTextChar(&ptr16[ii]);
		TUint8 byte2 = CObexSendAsAddBTRecipientState::GetByteFromUnicodeHexTextChar(&ptr16[ii+1]);
		TUint8 byte3 = CObexSendAsAddBTRecipientState::GetByteFromUnicodeHexTextChar(&ptr16[ii+2]);
		TUint8 byte4 = CObexSendAsAddBTRecipientState::GetByteFromUnicodeHexTextChar(&ptr16[ii+3]);

		byte1 = (TUint8) ( ((byte1 & 0x0F) << 4) | (byte2 & 0x0f) );
		byte3 = (TUint8) ( ((byte3 & 0x0F) << 4) | (byte4 & 0x0f) );

		// make sure this is put together in network endianness
		TUint16 result = (TUint16)( (TUint16)(byte3 << 8) | byte1 );

		iRecipient.Append( result );
		}
    }

void CSetObexMessageBTRecipientState::StartL(TRequestStatus& aStatus)
    {
    // open our existing entry and set the addressee.
    CObexTestUtils& testUtils = iClientTest.ObexTestUtils();
    TUid mtm = testUtils.CurrentObexMtm();
    (mtm == KUidMsgTypeBt) ? testUtils.InstantiateBTClientMtmL() : testUtils.InstantiateIRClientMtmL();
    CBaseMtm& client = testUtils.GetCurrentObexClientMtm();
    
    client.SwitchCurrentEntryL(testUtils.Entry().Id());
    client.LoadMessageL();
	client.AddAddresseeL(iRecipient);
	client.SaveMessageL();

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CSetObexMessageRecipientState
//
CSetObexMessageRecipientState::CSetObexMessageRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iRecipient(aRecipient)
    {
    }

void CSetObexMessageRecipientState::StartL(TRequestStatus& aStatus)
    {
    // open our existing entry and set the addressee.
    CObexTestUtils& testUtils = iClientTest.ObexTestUtils();
    TUid mtm = testUtils.CurrentObexMtm();
    (mtm == KUidMsgTypeBt) ? testUtils.InstantiateBTClientMtmL() : testUtils.InstantiateIRClientMtmL();
    CBaseMtm& client = testUtils.GetCurrentObexClientMtm();
    
    client.SwitchCurrentEntryL(testUtils.Entry().Id());
    client.LoadMessageL();
	client.AddAddresseeL(iRecipient);
	client.SaveMessageL();

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }
