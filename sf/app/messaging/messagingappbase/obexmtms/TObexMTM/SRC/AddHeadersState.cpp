// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: AddHeadersState.cpp $
// 
//

#include "AddHeadersState.h"

#include "obexharness.h"
#include "obextestutils.h"

#include <obexheaderlist.h>


CAddHeadersState::CAddHeadersState(const CObexHeader::THeaderType aHeaderType, const TUint8 aHeaderHI, const TDesC& aHeaderHV, CObexClientTest& aClientTest) :
iClientTest(aClientTest),
iHeaderType(aHeaderType),
iHeaderHI(aHeaderHI)
	{
	// Setup the header data buffer.
	iHeaderHV = HBufC::NewL(aHeaderHV.Length());
	iHeaderHV->Des().Copy(aHeaderHV);
	}
//

CAddHeadersState::~CAddHeadersState()
	{
	delete iHeaderHV;
	}
//

void CAddHeadersState::StartL(TRequestStatus& aStatus)
	{

	// Create a new obex header.
	CObexHeader* header = CObexHeader::NewL();
	CleanupStack::PushL(header);

	// Process the header data type depending on type.
	switch (iHeaderType)
		{
		case CObexHeader::EUnicode:
			{
			header->SetUnicodeL(iHeaderHI, iHeaderHV->Des());

			break;
			}

		case CObexHeader::EByteSeq:
			{
			// Convert the 16 bit string to 8 bit string.

			// Src
			TPtrC ptrHeaderHV(iHeaderHV->Des());

			// Dst
			HBufC8* byteSeqHV = HBufC8::NewMaxLC(iHeaderHV->Length());
			TPtr8 ptrByteSeqHV(byteSeqHV->Des()); 

			TInt	i = 0;
			while (i < iHeaderHV->Length())
				{
				ptrByteSeqHV[i] = (TUint8)ptrHeaderHV[i];
				i++;
				}
			header->SetByteSeqL(iHeaderHI, byteSeqHV->Des());

			CleanupStack::PopAndDestroy(byteSeqHV);
			break;
			}

		case CObexHeader::EByte:
			{
			TUint8 byteHV = 0;
			TLex lex(iHeaderHV->Des());

			lex.Inc(2); // Jump '0x'.
			User::LeaveIfError(lex.Val(byteHV, EHex));
			header->SetByte(iHeaderHI, byteHV);
			break;
			}

		case CObexHeader::EFourByte:
			{
			TUint32 fourByteHV = 0;
			TLex lex(iHeaderHV->Des());

			lex.Inc(2); // Jump '0x'.
			User::LeaveIfError(lex.Val(fourByteHV, EHex));
			header->SetFourByte(iHeaderHI, fourByteHV);
			break;
			}

		default:
			User::Leave(KErrUnknown);
		}

	// Add header to the current headers list.
	CObexHeaderList* currentHeaders = iClientTest.CurrentHeaders();
	User::LeaveIfError(currentHeaders->AddHeader(header));

	// Make a dupilcate header and add to the verify headers list.
	CObexHeader* dupHeader = CObexHeader::NewL();
	CleanupStack::PushL(dupHeader);
	
	dupHeader->Set(header);

	CObexTestHeaderList* verifyHeaders = iClientTest.VerifyHeaders();
	User::LeaveIfError(verifyHeaders->AddHeader(dupHeader));

	CleanupStack::Pop(dupHeader);
	CleanupStack::Pop(header);

	// Complete the state.
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}
