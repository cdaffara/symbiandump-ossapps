// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <calcontent.h>

#include "agmcontent.h"


/**
Constructor.
@return Pointer to a newly created content object
*/
EXPORT_C CCalContent* CCalContent::NewL()
	{
	CCalContent* self = new (ELeave) CCalContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/**
Destructor.
*/
EXPORT_C CCalContent::~CCalContent()
	{
	delete iImpl;
	}


/**
Takes ownership of the data or link to data.

A NULL pointer may be passed  to this function as an argument if the
caller does not want to specify a  parameter.  Any data or its MIME type
already owned by this class will be deleted first.

@param aSmallContent The data or link (URI).
@param aContentMimeType	The MIME type of the data or link (for example, text/image or text/http).
@param aContentDisposition Whether the content is data or a link to data.
@see CCalContent::TDisposition
*/
EXPORT_C void CCalContent::SetContentL(TDesC8* aSmallContent, TDesC8* aContentMimeType, CCalContent::TDisposition aContentDisposition)
	{
	iImpl->SetContentL(aSmallContent, aContentMimeType, aContentDisposition);
	}


/**
Gets the data set by CCalContent::SetContentL().

@return A reference to the data or link.  If no data or link has been set, a NULL
descriptor is returned.
*/
EXPORT_C const TDesC8& CCalContent::Content() const
	{
	return iImpl->Content();
	}


/**
Gets the MIME type set by CCalContent::SetContentL().

@return A reference to the MIME type.  If no MIME type has been set, a NULL
descriptor is returned.
*/
EXPORT_C const TDesC8& CCalContent::MimeType() const
	{
	return iImpl->MimeType();
	}


/**
Gets the disposition set by CCalContent::SetContentL().

@return The disposition of the data (whether the data is inline or a link).
*/
EXPORT_C CCalContent::TDisposition CCalContent::Disposition() const
	{
	return iImpl->Type();
	}


CCalContent::CCalContent()
	{
	}


void CCalContent::ConstructL()
	{
	iImpl = new (ELeave) CAgnContent;
	}


CAgnContent* CCalContent::Impl() const
	{
	return iImpl;
	}


void CCalContent::SetImpl(CAgnContent* aImpl)
	{
	if (iImpl != NULL)
		{
		delete iImpl;
		}
	iImpl = aImpl;
	}
