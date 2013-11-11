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

#ifndef PBAPFOLDERNODE_H
#define PBAPFOLDERNODE_H

#include "pbapfolderbase.h"
#include "pbaprequest.h"
#include "pbapoperation.h"

//forward declarations
class MVirtualFolderClient;
class CPbapAppHeader;


/**
 Abstract base class representing all the phonebook folders - pb,ich,och,mch,cch
 a further derived class groups the commonality of the "?ch" nodes
*/
class CFolderNode : public CFolderBase
	{
public:
	~CFolderNode();
	
	// from CFolderBase
	virtual TInt Get(const TDesC& aObjectName, CPbapAppHeader* aGetData);
		
protected:
	CFolderNode(MVirtualFolderClient& aClient, const TDesC& aFolderName);
	
protected:	
	virtual TInt DoGetItem(TInt aHandle) =0;
	virtual TInt DoGetListing() =0;
	virtual TInt DoGetCount() =0;
	virtual TInt DoGetFolder() =0;
	
protected:
	CPbapAppHeader* iAppHeader;
	};

#endif //PBAPFOLDERNODE_H
