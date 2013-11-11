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

#include <ecom/implementationproxy.h>
#include "csmsgetdetdescdefault.h"

// Define the implementation table for Ecom
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10282FA6, CSmsGetDetDescDefault::NewL)
	};

/**
Returns the implementation table

@param aTableCount Returns the number of entries in the implementation table
@return Ptr to implementation table
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

/**
Static factory constructor

@return Ptr to newly created instance of the class
*/
CSmsGetDetDescDefault* CSmsGetDetDescDefault::NewL()
	{
	CSmsGetDetDescDefault* self = new (ELeave) CSmsGetDetDescDefault();	
	return self;
	}

/**
Class constructor
*/
CSmsGetDetDescDefault::CSmsGetDetDescDefault()
	{
	}

/**
Class destructor
*/
CSmsGetDetDescDefault::~CSmsGetDetDescDefault()
	{
	}

/**
Get the SMS message recipient/sender details.
This default version just makes a call to TSmsUtilities::GetDetails()

@param aFs A connected file server session.
@param aMessage The SMS message object
@param aDetails The output argument to contain the message details.
@param aMaxLength The maximum length of the supplied buffer in aDetails.

@return System wide error code
*/
TInt CSmsGetDetDescDefault::GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength)
	{
	return TSmsUtilities::GetDetails(aFs, aMessage, aDetails, aMaxLength);
	}

/**
Get the SMS message recipient/sender details.
This default version just makes a call to TSmsUtilities::GetDetails()

@param aFs A connected file server session.
@param aFromAddress The recipient/sender telephone number.
@param aDetails The output argument to contain the message details.
@param aMaxLength The maximum length of the supplied buffer in aDetails.

@return System wide error code
*/
TInt CSmsGetDetDescDefault::GetDetails(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength)
	{
	return TSmsUtilities::GetDetails(aFs, aFromAddress, aDetails, aMaxLength);
	}

/**
Get the SMS message description.
This default version just makes a call to TSmsUtilities::GetDescription()

@param aMessage The SMS message object
@param aDescription The output argument for the description.
@param aMaxLength The maximum length of the supplied buffer in aDescription.

@return System wide error code
*/
TInt CSmsGetDetDescDefault::GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength)
	{
	return TSmsUtilities::GetDescription(aMessage, aDescription, aMaxLength);
	}
