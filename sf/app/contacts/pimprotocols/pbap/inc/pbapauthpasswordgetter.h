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

#ifndef PBAPAUTHPASSWORDGETTER_H
#define PBAPAUTHPASSWORDGETTER_H

#include <e32base.h>
#include <btextnotifiers.h>
#include <btextnotifierspartner.h>

// forward declarations
class CPbapServer;

class CPbapAuthPasswordGetter : public CActive
	{
public:
	static CPbapAuthPasswordGetter* NewL(CPbapServer& aParent);
	~CPbapAuthPasswordGetter();

	void GetPassword(const TDesC& aRealm, const TBTDevAddr& aAddress);

private:
	CPbapAuthPasswordGetter(CPbapServer& aParent);
	void ConstructL();

	// from CActive
	void RunL();
	void DoCancel();

private:
	CPbapServer& iParent;
	RNotifier iNotifier;
	TPbapAuthNotifierResponsePckg iResultsPckg;
	};

#endif // PBAPAUTHPASSWORDGETTER_H
