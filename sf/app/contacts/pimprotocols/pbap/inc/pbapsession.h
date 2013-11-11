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

#ifndef PBAPSESSION_H
#define PBAPSESSION_H

#include <e32base.h>

// forward declarations
class CPbapServer;

/**
 PBAP Server Session.
 */
class CPbapSession : public CSession2
	{
public:
	CPbapSession();
	~CPbapSession();
	
	CPbapServer& Server();
		
public:
	// from CSession2
	void CreateL();
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2 &aMessage, TInt aError);

private:
	void CompleteRequest(const RMessage2& aMessage, TInt aResult);
	};

#endif // PBAPSESSION_H
