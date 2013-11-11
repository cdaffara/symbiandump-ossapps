// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client MTM utilities for POP3, SMTP and IMAP4 protocols
// 
//

#include <mtmdef.h>		//KUidMtmQueryxxx & TMsvPartList flags
#include <mtclreg.h>
#include "MIUTSET.H"
#include "MIUT_ERR.H"
#include "IMCMMAIN.H"
#include "IMCMUTIL.H"
#include "SMTPCMDS.H"

/**
Allocates and constructs a new CImClientMTMUtils object.
@return New CImClientMTMUtils object
*/
EXPORT_C CImClientMTMUtils* CImClientMTMUtils::NewL()
	{
	CImClientMTMUtils* self = new (ELeave) CImClientMTMUtils;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Destructor.
*/
EXPORT_C CImClientMTMUtils::~CImClientMTMUtils()
	{
	delete iFindText;
	}

void CImClientMTMUtils::ConstructL()
	{
	iFindText = CMsvFindText::NewL();
	}

/**
Searches a message for some specified text.

@param aTextToFind Text to find
@param aRichText Message body to search
@param aHeader Message header to search
@param aPartList Bitmask of the parts of the message to search for the text, and 
flags which modify the way the search works. See KMsvMessagePartBody etc.
in mtmdef.h for message part flags. Search flags are: KMsvFindCaseSensitive 
means the search is successful only if there is an exact case match for the 
text; KMsvFindWholeWord means the search is successful only if the matching 
text is not delimited by alphanumeric characters.

@param rReturnList On return, bitmask of the message parts in which the text was found
*/
EXPORT_C void CImClientMTMUtils::FindL(const TDesC& aTextToFind, CRichText& aRichText, CImHeader& aHeader, TMsvPartList aPartList, TMsvPartList& rReturnList)
	{
	if (aPartList & KMsvMessagePartBody)
		rReturnList = FindInBodyL(aTextToFind, aRichText, aPartList);
	// run for all others as FindInHeaderL() will check the TMsvPartList
	rReturnList |= FindInHeaderL(aTextToFind, aHeader, aPartList);
	}

/**
Searches a message body for some specified text.

@param aTextToFind Text to find
@param aRichText Message body to search
@param aPartList Bitmask of flags which modify the way the search works: KMsvFindCaseSensitive 
means the search is successful only if there is an exact case match for the 
text. KMsvFindWholeWord means the search is successful only if the matching 
text is not delimited by alphanumeric characters.
@return KMsvMessagePartBody if the text was found, KMsvMessagePartNone if not.
*/
EXPORT_C TMsvPartList CImClientMTMUtils::FindInBodyL(const TDesC& aTextToFind, CRichText& aRichText, TMsvPartList aPartList)
	{
	TMsvPartList foundList = KMsvMessagePartNone;
	if (iFindText->FindRichTextL(aTextToFind, aRichText, aPartList))
		foundList |= KMsvMessagePartBody;
	return foundList;
	}

/**
Searches a message header for some specified text.

@param aTextToFind Text to find
@param aHeader Message header to search
@param aPartList Bitmask of the parts of the message to search for the text, and 
flags which modify the way the search works. See KMsvMessagePartRecipient etc.
in mtmdef.h for message part flags. Search flags are: KMsvFindCaseSensitive 
means the search is successful only if there is an exact case match for the 
text; KMsvFindWholeWord means the search is successful only if the matching 
text is not delimited by alphanumeric characters. Wild cards can also be specified as part of the search.
The supported wild characters are * and ?
@return Bitmask of the message parts in which the text was found
*/
EXPORT_C TMsvPartList CImClientMTMUtils::FindInHeaderL(const TDesC& aTextToFind, CImHeader& aHeader, TMsvPartList aPartList)
	{
	TMsvPartList foundList = KMsvMessagePartNone;
	if (aPartList & KMsvMessagePartRecipient)
		{ 
		//search in To field, if not found then search in CC and if not found then search Bcc.
		if (FindInMessagePartRecipientL(aTextToFind, aPartList, aHeader.ToRecipients()))
			foundList |= KMsvMessagePartRecipient;
		else if (FindInMessagePartRecipientL(aTextToFind, aPartList, aHeader.CcRecipients()))
			foundList |= KMsvMessagePartRecipient;
		else 
			(FindInMessagePartRecipientL(aTextToFind, aPartList, aHeader.BccRecipients())) ? foundList|=KMsvMessagePartRecipient : foundList;
		}
	if (aPartList & KMsvMessagePartOriginator)
		iFindText->FindTextL(aTextToFind, aHeader.From(), aPartList) ? foundList|=KMsvMessagePartOriginator : foundList;
	
	if (aPartList & KMsvMessagePartDescription)
		iFindText->FindTextL(aTextToFind, aHeader.Subject(), aPartList) ? foundList|=KMsvMessagePartDescription : foundList;
	
	// Search based on new message parts that are introduced. The search only looks in the  specified message part
	// To search
	if (aPartList & KMsvMessagePartTo)
		{
		FindInMessagePartRecipientL(aTextToFind, aPartList, aHeader.ToRecipients()) ? foundList|=KMsvMessagePartTo : foundList;
		}
	// Cc search	
	else if (aPartList & KMsvMessagePartCc)
		{
		FindInMessagePartRecipientL(aTextToFind, aPartList, aHeader.CcRecipients()) ? foundList|=KMsvMessagePartCc : foundList;
		}
	// Bcc search
	else if (aPartList & KMsvMessagePartBcc)
		{
		FindInMessagePartRecipientL(aTextToFind, aPartList, aHeader.BccRecipients()) ? foundList|=KMsvMessagePartCc : foundList;
		}
	// From search	
	else if (aPartList & KMsvMessagePartFrom)
		{
		iFindText->FindTextL(aTextToFind, aHeader.From(), aPartList) ? foundList|=KMsvMessagePartFrom : foundList;
		}
	// Subject search	
	else if (aPartList & KMsvMessagePartSubject)
		{
		iFindText->FindTextL(aTextToFind, aHeader.Subject(), aPartList) ? foundList|=KMsvMessagePartSubject : foundList;
		}
	return foundList;
	}

TBool CImClientMTMUtils::FindInMessagePartRecipientL(const TDesC& aTextToFind, TMsvPartList aPartList, CDesCArray& aRecipients)
	{
	TInt count = aRecipients.Count();
	TBool found = EFalse;
	for (TInt i=0;i<count;i++)
		{			
		if (iFindText->FindTextL(aTextToFind, aRecipients[i], aPartList))
			{
			found = ETrue;
			break;			
			}
		}
	return found;
	}

