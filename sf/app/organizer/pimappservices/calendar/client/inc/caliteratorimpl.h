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

#ifndef __CALITERATORIMPL_H__
#define __CALITERATORIMPL_H__

#include <e32base.h>

class CCalSessionImpl;
class RAgendaServ;


NONSHARABLE_CLASS(CCalIteratorImpl) : public CBase
/** 
An iterator for iterating though all the entries in the calendar store.

This may be used by a synchronisation application to find entries that
have been added\deleted between two synchronisations. 

@internalComponent
*/
    {
public:
	CCalIteratorImpl(CCalSessionImpl& aSessionImpl);
	~CCalIteratorImpl();
	
	const TDesC8& FirstL();
	const TDesC8& NextL();
	
private:
	void FindUidL();

private:
	CCalSessionImpl&	iCalSessionImpl;//Not owned
	RAgendaServ&		iServer;
	HBufC8*				iGuid;
	TBool				iFileEntry;
	};
	
#endif // __CALITER_H__
