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

#ifndef PBAPFOLDERSIM_H
#define PBAPFOLDERSIM_H

#ifdef __INCLUDE_SIM1_FOLDERS__

#include "pbapfolderbase.h"

//forward declarations
class CExternalPBAPProvider;
class MVirtualFolderClient;
 

class CFolderSIM1Base : public CFolderBase
	{
protected:
	CFolderSIM1Base(MVirtualFolderClient& aClient, const TDesC& aFolderName);
	CExternalPBAPProvider& ExternalProvider();
	void BaseConstructL();
	
	// from CFolderBase
	virtual TInt Get(const TDesC& aObjectName, CPbapAppHeader* aGetData);	
	virtual void CancelGet();
	
private:
	CExternalPBAPProvider* iSIM1Provider;
	};


class CFolderSIM1 : public CFolderSIM1Base
	{
public:
	static CFolderSIM1* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1(MVirtualFolderClient& aClient);
	};

	
class CFolderSIM1Telecom : public CFolderSIM1Base
	{
public:
	static CFolderSIM1Telecom* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1Telecom(MVirtualFolderClient& aClient);
	};


class CFolderSIM1NodePb : public CFolderSIM1Base
	{
public:
	static CFolderSIM1NodePb* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1NodePb(MVirtualFolderClient& aClient);
	};


class CFolderSIM1NodeIch : public CFolderSIM1Base
	{
public:
	static CFolderSIM1NodeIch* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1NodeIch(MVirtualFolderClient& aClient);
	};


class CFolderSIM1NodeOch : public CFolderSIM1Base
	{
public:
	static CFolderSIM1NodeOch* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1NodeOch(MVirtualFolderClient& aClient);
	};


class CFolderSIM1NodeMch : public CFolderSIM1Base
	{
public:
	static CFolderSIM1NodeMch* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1NodeMch(MVirtualFolderClient& aClient);
	};


class CFolderSIM1NodeCch : public CFolderSIM1Base
	{
public:
	static CFolderSIM1NodeCch* NewL(MVirtualFolderClient& aClient);
private:
	CFolderSIM1NodeCch(MVirtualFolderClient& aClient);
	};

#endif // __INCLUDE_SIM1_FOLDERS__

#endif // PBAPFOLDERSIM_H
