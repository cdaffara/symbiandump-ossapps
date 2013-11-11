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

#ifndef PBAPLOGENGVIEWOBSERVER_H
#define PBAPLOGENGVIEWOBSERVER_H

#include <e32def.h>

class MPbapLogEngViewObserver
	{
public:
	virtual void PbapLogEngViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount) = 0;
	virtual void PbapLogEngViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount) = 0;
	virtual void PbapLogEngViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount) = 0;

	virtual void PbapLogEngViewChangeEventL(TUid aChangeType, TInt aChangeParam1, TInt aChangeParam2, TInt aChangeParam3) = 0;
	};

#endif // PBAPLOGENGVIEWOBSERVER_H