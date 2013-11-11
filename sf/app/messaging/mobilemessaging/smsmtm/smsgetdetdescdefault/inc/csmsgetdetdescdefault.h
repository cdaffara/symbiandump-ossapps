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

#ifndef __CSMSGETDETDESCDEFAULT_H__
#define __CSMSGETDETDESCDEFAULT_H__

#include <csmsgetdetdescinterface.h>

/**
Default ECOM interface implementation for the routines used to create 
the details and description values for an SMS.
@internalTechnology
@released
*/
class CSmsGetDetDescDefault : public CSmsGetDetDescInterface
	{
public:
	static CSmsGetDetDescDefault* NewL();
	virtual ~CSmsGetDetDescDefault();

	virtual TInt GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength);
	virtual TInt GetDetails(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength);
	virtual TInt GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength);

private:
	CSmsGetDetDescDefault();
	};

#endif // __CSMSGETDETDESCDEFAULT_H__
