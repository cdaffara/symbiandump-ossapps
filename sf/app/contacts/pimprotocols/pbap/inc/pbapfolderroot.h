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

#ifndef PBAPFOLDERROOT_H
#define PBAPFOLDERROOT_H

#include "pbapfolderbase.h"

//forward declarations
class MVirtualFolderClient;
class CPbapAppHeader;


class CFolderRoot : public CFolderBase
	{
public:
	static CFolderRoot* NewL(MVirtualFolderClient& aClient);
private:
	CFolderRoot(MVirtualFolderClient& aClient);

private: //from CFolderBase
	virtual TInt Get(const TDesC& aObjectName, CPbapAppHeader* aGetData);	
	virtual void CancelGet();
	virtual void GetComplete();
	};

#endif //PBAPFOLDERROOT_H
