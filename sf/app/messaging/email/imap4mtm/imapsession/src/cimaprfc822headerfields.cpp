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
// cimaprfc822headerfields.cpp
//

#include "cimaprfc822headerfields.h"
#include "cimapsessionconsts.h"

class CDesC8Array;

#include <imcvutil.h>
#include <miuthdr.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#endif

CImapRfc822HeaderFields* CImapRfc822HeaderFields::NewL()
	{
	CImapRfc822HeaderFields* self = new (ELeave) CImapRfc822HeaderFields();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImapRfc822HeaderFields::CImapRfc822HeaderFields()
	{
	}

void CImapRfc822HeaderFields::ConstructL()
	{
	CImapHeaderFields::ConstructL(EImapCountFields);
	}

CImapRfc822HeaderFields::~CImapRfc822HeaderFields()
	{
	}

/**
Returns the value of the field identified by the supplied field index.
If the field has not been populated, a null descriptor is returned.
@param aFieldId index of the field value to return.
@return The value of the filed or a null descriptor.
*/
EXPORT_C const TDesC8& CImapRfc822HeaderFields::FieldValue(THeaderFields aFieldId)
	{
	return CImapHeaderFields::FieldValue(aFieldId);
	}

/**
Returns whether the field identified by the supplied field index has been populated.
This indicates whether the field value was supplied by the server.
@param aFieldId index of the field value to return.
@return Whether field has been populated.
*/
EXPORT_C TBool CImapRfc822HeaderFields::FieldExists(THeaderFields aFieldId)
	{
	return CImapHeaderFields::FieldExists(aFieldId);
	}

/**
Returns the numerical priority for an email based on the various priority fields that can
be received in the header fields.
If no priority is found, then the returned value will be set to medium priority.
@return Extracted numerical priority. If no priority is found, then medium priority is returned.
*/
EXPORT_C TMsvPriority CImapRfc822HeaderFields::PriorityL()
	{
	CImcvUtils* imcvUtils = CImcvUtils::NewLC();
	TMsvPriority priority(EMsvMediumPriority);

	if (FieldExists(EImapPriority))
		{
		priority = imcvUtils->EvaluatePriorityText(FieldValue(EImapPriority));
		}
	else if (FieldExists(EImapXPriority))
		{
		priority = imcvUtils->EvaluatePriorityText(FieldValue(EImapXPriority));
		}
	else if (FieldExists(EImapXMSMailPriority))
		{
		priority = imcvUtils->EvaluatePriorityText(FieldValue(EImapXMSMailPriority));
		}
	else if (FieldExists(EImapImportance))
		{
		priority = imcvUtils->EvaluatePriorityText(FieldValue(EImapImportance));
		}

	CleanupStack::PopAndDestroy(imcvUtils);

	return priority;
	}

/**
Returns a date for an email. If the 'date' header field exists, then this is used. If not,
then a check is made to see if a 'received' header field exists containing a date.
@return Date value. If no date found, then the date value will be set to 0.
*/
EXPORT_C TTime CImapRfc822HeaderFields::Date()
	{
	TTime date(0);
	TImRfc822DateField dateField;

	if (FieldExists(EImapDate))
		{
		dateField.ParseDateField(FieldValue(EImapDate), date);
		}
	else if (FieldExists(EImapReceived))
		{
		TPtrC8 ptr(FieldValue(EImapReceived));

		_LIT8(KSemiColonSpace, "; ");
		TInt semicolonPos = ptr.Find(KSemiColonSpace);

		if (semicolonPos >= 0)
			{
			ptr.Set(ptr.Right(ptr.Length() - semicolonPos - 2));

			if (ptr.Length() > 0)
				{
				dateField.ParseDateField(ptr, date);
				}
			}
		}

	return date;
	}

/**
Returns the return receipt value from the header fields based on the various 'return receipt'
header fields that can be received.
@return Return receipt field value. Null descriptor if no 'return receipt' fields exist.
*/
EXPORT_C const TDesC8& CImapRfc822HeaderFields::ReturnReceiptField()
	{
	if (FieldExists(EImapReturnReceiptTo))
		{
		return FieldValue(EImapReturnReceiptTo);
		}

	if (FieldExists(EImapXReturnReceiptTo))
		{
		return FieldValue(EImapXReturnReceiptTo);
		}

	if (FieldExists(EImapDispositionNotificationTo))
		{
		return FieldValue(EImapDispositionNotificationTo);
		}

	return KNullDesC8();
	}


TInt CImapRfc822HeaderFields::Match(const TDesC8& aFieldName, TBool& /*aNeedToStripSpaces*/)
	{
	THeaderFields fieldId;

	if (aFieldName.CompareF(KImapTxtFrom) == 0)
		{
		fieldId = EImapFrom;
		}
	else if (aFieldName.CompareF(KImapTxtReplyTo) == 0)
		{
		fieldId = EImapReplyTo;
		}
	else if (aFieldName.CompareF(KImapTxtTo) == 0)
		{
		fieldId = EImapTo;
		}
	else if (aFieldName.CompareF(KImapTxtCc) == 0)
		{
		fieldId = EImapCc;
		}
	else if (aFieldName.CompareF(KImapTxtBcc) == 0)
		{
		fieldId = EImapBcc;
		}
	else if (aFieldName.CompareF(KImapTxtSubject) == 0)
		{
		fieldId = EImapSubject;
		}
	else if (aFieldName.CompareF(KImapTxtReceived) == 0)
		{
		fieldId = EImapReceived;
		}
	else if (aFieldName.CompareF(KImapTxtDate) == 0)
		{
		fieldId = EImapDate;
		}
	else if (aFieldName.CompareF(KImapTxtMessageId) == 0)
		{
		fieldId = EImapMessageId;
		}
	else if (aFieldName.CompareF(KImapTxtXMailer) == 0)
		{
		fieldId = EImapXMailer;
		}		
	else if (aFieldName.CompareF(KImapTxtPriority) == 0)
		{
		fieldId = EImapPriority;
		}
	else if (aFieldName.CompareF(KImapTxtXPriority) == 0)
		{
		fieldId = EImapXPriority;
		}
	else if (aFieldName.CompareF(KImapTxtXMSMailPriority) == 0)
		{
		fieldId = EImapXMSMailPriority;
		}
	else if (aFieldName.CompareF(KImapTxtPrecedence) == 0)
		{
		fieldId = EImapPrecedence;
		}
	else if (aFieldName.CompareF(KImapTxtImportance) == 0)
		{
		fieldId = EImapImportance;
		}
	else if (aFieldName.CompareF(KImapTxtReturnReceiptTo) == 0)
		{
		fieldId = EImapReturnReceiptTo;
		}
	else if (aFieldName.CompareF(KImapTxtXReturnReceiptTo) == 0)
		{
		fieldId = EImapXReturnReceiptTo;
		}
	else if (aFieldName.CompareF(KImapTxtDispositionNotificationTo) == 0)
		{
		fieldId = EImapDispositionNotificationTo;
		}
	else if (aFieldName.CompareF(KImapTxtDispositionNotificationOptions) == 0)
		{
		fieldId = EImapDispositionNotificationOptions;
		}
	else
		{
		return KErrNotFound;
		}

	return fieldId;
	}
