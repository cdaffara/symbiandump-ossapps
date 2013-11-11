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

#include "cimapheaderfieldsparser.h"
#include "cimapheaderfields.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "cimapcommand.h"

CImapHeaderFieldsParser::CImapHeaderFieldsParser(TInt aLogId) :
	iState(EWaitLine),
	iLogId(aLogId)
	{
	}

void CImapHeaderFieldsParser::Construct(CImapHeaderFields* aHeaderFields)
	{
	iHeaderFields = aHeaderFields;
	iHeaderFieldsOwned = ETrue;
	}

CImapHeaderFieldsParser::~CImapHeaderFieldsParser()
	{
	if (iHeaderFieldsOwned)
		{
		delete iHeaderFields;
		}
	}

TBool CImapHeaderFieldsParser::ProcessBlockL(const TDesC8& aData)
	{
	TBool wantMore = ETrue;
	switch (iState)
		{
		case EWaitLine:
			{
			if ((aData.Length() >= 3) && (aData[aData.Length() - 1] == '}'))
				{
				iState = EWaitLiteral;
				}
			else
				{
				// Header Fields ALWAYS delivered as a literal
				CImapCommand::CorruptDataL(iLogId);
				}
			}
			break;
		case EWaitLiteral:
			{
			ParseHeaderFieldsBlockL(aData);
			iState = EParseComplete;
			wantMore = EFalse;
			}
			break;
		case EParseComplete:
		default:
			{
			ASSERT(EFalse);
			wantMore = EFalse;
			}
			break;
		}
		
	return wantMore;
	}
	

TPtrC8 CImapHeaderFieldsParser::UnparsedData()
	{
	return iData;
	}

void CImapHeaderFieldsParser::ParseHeaderFieldsBlockL(const TDesC8& aData)
	{
	// Only expecting one header fields block.
	// Check that we own the header fields at this point.
	ASSERT(iHeaderFieldsOwned);

	iData.Set(aData);
	
	ParseFieldsL();
	
	// The block has been parsed so transfer ownership of iHeaderFields
	HeaderFieldsComplete(iHeaderFields);
	iHeaderFieldsOwned = EFalse;
	}

void CImapHeaderFieldsParser::ParseFieldsL()
	{
	RArray<TPtrC8> parts;
	CleanupClosePushL(parts);
	TInt length(0);

	// Parse one line at a time
	TInt posCrlf = iData.Find(KImapTxtCrlf);

	while (posCrlf >= 0)
		{
		// Fetch the line and consume it from iData
		TPtrC8 line = iData.Left(posCrlf);

		if (line.Length() > 0)
			{
			parts.AppendL(line);
			length += line.Length();
			}
		else
			{
			// Two instances of CRLF in a row. This signifies the end of the previous line.
			ParsePartsL(parts, length);
			}

		if (iData.Length() > posCrlf + KImapTxtCrlf.iTypeLength)
			{
			iData.Set(iData.Mid(posCrlf + KImapTxtCrlf.iTypeLength));

			if (iData[0] > ' ')
				{
				// Next line does not contain whitespace as its first character. This
				// signifies the end of the previous line.
				ParsePartsL(parts, length);
				}
			}
		else
			{
			iData.Set(KNullDesC8());
			}

		posCrlf = iData.Find(KImapTxtCrlf);
		}

	// Data block is now complete. Process any oustanding parts.
	ParsePartsL(parts, length);

	CleanupStack::PopAndDestroy(&parts);
	}

void CImapHeaderFieldsParser::ParsePartsL(RArray<TPtrC8>& aParts, TInt& aLength)
	{
	if (aParts.Count() > 0)
		{
		// Search for colon in the first part
		TInt posColon = aParts[0].Locate(':');

		// Ignore line if it has no colon (or if it begins with a colon)
		if (posColon > 0)
			{
			TPtrC8 fieldName(aParts[0].Left(posColon));

			// Move past the colon
			++posColon;

			// Check that there is some data still left to process after the colon.
			if (aLength > posColon)
				{
				// Update the first part to remove the portion before the colon
				if (aParts[0].Length() > posColon)
					{
					aParts[0].Set(aParts[0].Mid(posColon));
					}
				else
					{
					aParts[0].Set(KNullDesC8());
					}

				aLength -= posColon;

				// Strip all whitespace up to the start of the field data
				TInt part(0);
				for (; part < aParts.Count(); ++part)
					{
					while ((aParts[part].Length() > 0) && (aParts[part][0] <= ' '))
						{
						aParts[part].Set(aParts[part].Mid(1));
						aLength--;
						}

					// Either we have found a non whitespace character, or the current part
					// is empty. If we have found a non whitespace character, we can exit the
					// loop now.
					if (aParts[part].Length() > 0)
						{
						break;
						}
					}

				if (aLength > 0)
					{
					HBufC8* wholeLine = HBufC8::NewLC(aLength);

					for (part = 0; part < aParts.Count(); ++part)
						{
						wholeLine->Des().Append(aParts[part]);
						}

					// Pop whole line as ownership is about to be transferred
					CleanupStack::Pop(wholeLine);
					__LOG_FORMAT((iLogId, "Add header field - %S: %S", &fieldName, &(*wholeLine)));
					iHeaderFields->SetFieldL(fieldName, wholeLine);
					}
				}
			}

		aParts.Reset();
		aLength = 0;
		}
	}
