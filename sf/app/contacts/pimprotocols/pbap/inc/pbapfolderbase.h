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

#ifndef PBAPFOLDERBASE_H
#define PBAPFOLDERBASE_H

#include <e32base.h>
#include "pbapfolderclient.h"

//forward declarations
class CPbapAppHeader;


class CFolderBase : public CBase
	{
public:	
	const TPtrC FolderName() const;
	
	// implementations of Get must take ownership of aGetData even if they return an error
	virtual TInt Get(const TDesC& aObjectName, CPbapAppHeader* aGetData)=0;	
	virtual void CancelGet() =0;
	virtual void GetComplete() =0;
	
protected:
	inline CFolderBase(MVirtualFolderClient& aClient, const TPtrC aFolderName);
	
protected:
	MVirtualFolderClient& iClient;
	
private:
	TPtrC iFolderName;
	};


//
// inline functions
//
inline CFolderBase::CFolderBase(MVirtualFolderClient& aClient, const TPtrC aFolderName)
: iClient(aClient)
	{
	iFolderName.Set(aFolderName);
	}

inline const TPtrC CFolderBase::FolderName() const
	{
	return iFolderName;
	}
	
#endif //PBAPFOLDERBASE_H
