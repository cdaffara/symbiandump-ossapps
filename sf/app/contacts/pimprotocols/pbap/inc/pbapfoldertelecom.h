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

#ifndef PBAPFOLDERTELECOM_H
#define PBAPFOLDERTELECOM_H

#include "pbapfolderbase.h"

//forward declarations
class MVirtualFolderClient;
class CPbapAppHeader;


class CFolderTelecom : public CFolderBase
	{
public:
	static CFolderTelecom* NewL(MVirtualFolderClient& aClient);

private:
	CFolderTelecom(MVirtualFolderClient& aClient);

private: //from CFolderBase
	virtual TInt Get(const TDesC& aObjectName, CPbapAppHeader* aGetData);	
	virtual void CancelGet();
	virtual void GetComplete();
	};

#endif //PBAPFOLDERTELECOM_H
