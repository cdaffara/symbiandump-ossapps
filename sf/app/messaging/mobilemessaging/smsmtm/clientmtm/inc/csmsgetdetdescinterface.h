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

#ifndef __CSMSGETDETDESCINTERFACE_H__
#define __CSMSGETDETDESCINTERFACE_H__

#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <smut.h>

// Forward class references
class CSmsMessage;

/**
ECOM interface definition for the routines used to create the details and
description values for an SMS.

Note: Default parameters are included to provide an equivalent interface to that
previously provided by TSmsUtilities.
Implementers of substitute plugins should be aware that the value of the default 
arguments specified in each of the functions, ie aMaxLength = KSmsDetailsLength, 
cannot be overridden by specifying a new value in the plugin implementation, as
the APIs are always called via a base-class pointer.

@publishedPartner
@released
*/
class CSmsGetDetDescInterface : public CBase
	{
public:
	static CSmsGetDetDescInterface* NewL();
	virtual ~CSmsGetDetDescInterface();

	virtual TInt GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength = KSmsDetailsLength) = 0;
	virtual TInt GetDetails(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength = KSmsDetailsLength) = 0;
	virtual TInt GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength = KSmsDescriptionLength) = 0;

protected:
	inline CSmsGetDetDescInterface();

private:
	TUid iDtor_ID_Key;
	};

#include <csmsgetdetdescinterface.inl>

#endif // __CSMSGETDETDESCINTERFACE_H__
