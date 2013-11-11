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

#include "calinstanceiterator.h"
#include "calsessionimpl.h"
#include "calinstanceviewimpl.h"

/** Instance iterator destructor

@publishedPartner
@released
*/
EXPORT_C CCalInstanceIterator::~CCalInstanceIterator()
	{
	const_cast<CCalInstanceViewImpl&>(iInstanceViewImpl).DecrementSessionReferenceCount();
	}

/** Instance iterator constructor

@internalComponent
*/
CCalInstanceIterator::CCalInstanceIterator(const CCalInstanceViewImpl& aInstanceViewImpl)
	:iInstanceViewImpl(aInstanceViewImpl),
	iSessionImpl(aInstanceViewImpl.CalSessionImpl())
	{//iSessionImpl is not needed after multiple calendar support. However, we have to provide CCalSessionImpl for BC reason
	const_cast<CCalInstanceViewImpl&>(aInstanceViewImpl).IncrementsessionReferenceCount();
	}
