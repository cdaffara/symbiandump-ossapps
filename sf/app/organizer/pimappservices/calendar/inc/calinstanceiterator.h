// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALINSTANCEITERATOR_H__
#define __CALINSTANCEITERATOR_H__

#include <e32base.h>

class CCalSessionImpl;
class CCalInstance;
class CAgnEntry;
class TAgnInstanceId;
class TCalTime;
class CCalInstanceViewImpl;

/** A calendar instance iterator

This class defines an interface for iterating through calendar instances
and can be created by calling the appropriate methods on CCalInstanceView.  
Please see the coments on those APIs for the details of which instances
will be iterated though.

@publishedPartner
@released
*/
class CCalInstanceIterator : public CBase
	{
public:
	IMPORT_C ~CCalInstanceIterator();
	
	/** Fetches the next instance in the iterator.

	@return The next instance in the iterator or NULL if there are no more instances.
	This API returns ownership of the instance.
	
	@leave KErrNotFound If there is a problem creating the instance because the entry cannot be found.
	
	@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
	Otherwise any of the system-wide error codes.

	@publishedPartner
	@released
	@capability ReadUserData
	*/
	virtual CCalInstance* NextL() = 0;
	
	/** Fetches the previous instance in the iterator.

	@return The previous instance in the iterator or NULL if there are no more instances.
	This API returns ownership of the instance.
	
	@leave KErrNotFound If there is a problem creating the instance because the entry cannot be found.
	
	@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
	Otherwise any of the system-wide error codes.

	@publishedPartner
	@released
	@capability ReadUserData
	*/
	virtual CCalInstance* PreviousL() = 0;
	
	/** Checks to see if there are any more results in the iterator to be returned by calling NextL.
	
	@return ETrue if there are more results, EFalse if not.

	@publishedPartner
	@released
	@capability None
	*/
	virtual TBool HasMore() const = 0;
	
	/** Returns the total number of instances the iterator will iterate through, including the starting instance if one was provided.

	Note that it is best to avoid using this API if possible because it will be slow
	to calculate the count, if the iterator contains a lot of instances. 
	
	@return The number of instances found or, if there was a problem calculating the count,
	a system-wide error code.
	
	@publishedPartner
	@released
	@capability None
	*/
	virtual TInt Count() const = 0;
	
protected:
	CCalInstanceIterator(const CCalInstanceViewImpl& aInstanceViewImpl);

protected:
	const CCalInstanceViewImpl& iInstanceViewImpl;
	CCalSessionImpl& iSessionImpl;
	};

#endif // __CALINSTANCEITERATOR_H__
