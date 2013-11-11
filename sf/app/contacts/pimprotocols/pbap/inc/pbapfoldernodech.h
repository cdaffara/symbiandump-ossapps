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

#ifndef PBAPFOLDERNODECH_H
#define PBAPFOLDERNODECH_H

#include "pbapfoldernode.h"
#include "pbaprequest.h"
#include "pbapexporter.h"
#include "pbapchviewobserver.h"

//forward declarations
class CVirtualFolders;
class MVirtualFolderClient;
class CPbapAppHeader;
class CPbapChView;
class CPbapChExporter;

/**
 Base class for all of the "?ch" nodes
*/
class CFolderNodeCallHistory : public CFolderNode, public MPbapExporterCallback, public MPbapChViewObserver
	{
public:
	enum THistoryType
		{
		EIncoming,
		EOutgoing,
		EMissed,
		ECombined
		};

protected:
	CFolderNodeCallHistory(MVirtualFolderClient& aClient, const TDesC& aFolderName, THistoryType aHistoryType);
	void ConstructL();
	~CFolderNodeCallHistory();

	// from MPbapExporterCallback
	virtual void HandleExportComplete(TInt aError);

	// from CFolderBase
	virtual void CancelGet();
	virtual void GetComplete();
		
	// from CFolderNode
	virtual TInt DoGetItem(TInt aHandle);
	virtual TInt DoGetListing();
	virtual TInt DoGetCount();
	virtual TInt DoGetFolder();
	
	// from MPbapChViewObserver
	virtual void CallHistoryViewReady(TInt aError);
	virtual void CallHistoryChangeNotification(TBool aViewReady = ETrue);

private:
	enum TExportType
		{
		ENoExport,
		EExportItem,
		EExportListing,
		EExportCount,
		EExportFolder
		};
			
private:
	TInt DoExport(TExportType aExportType, TInt aHandle = 0);
	TInt StartExport(TExportType aExportType, TInt aHandle);

	void CancelPendingExport(TInt err);

private:
	CPbapChExporter* iAsyncExporter;
	
	THistoryType iHistoryType;
	
	CPbapChView* iFolderView;
	CPbapChView* iReadFolderView;
	
	TExportType iPendingExport;
	TInt iPendingHandle;
	
	TBool iReadyToExport;
	TBool iCallHistoryChanged;
	};


/**
 Incoming call history phonebook folder
*/
class CFolderNodeIch : public CFolderNodeCallHistory
	{
public:
	static CFolderNodeIch* NewL(MVirtualFolderClient& aClient);
	
private:
	CFolderNodeIch(MVirtualFolderClient& aClient);
	};


/**
 Outgoing call history phonebook folder
*/	
class CFolderNodeOch : public CFolderNodeCallHistory
	{
public:
	static CFolderNodeOch* NewL(MVirtualFolderClient& aClient);

private:
	CFolderNodeOch(MVirtualFolderClient& aClient);
	};


/**
 Missed call history phonebook folder
*/
class CFolderNodeMch : public CFolderNodeCallHistory
	{
public:
	static CFolderNodeMch* NewL(MVirtualFolderClient& aClient);
	
private:
	CFolderNodeMch(MVirtualFolderClient& aClient);
	};
	

/**
 Combined call history phonebook folder
*/	
class CFolderNodeCch : public CFolderNodeCallHistory
	{
public:
	static CFolderNodeCch* NewL(MVirtualFolderClient& aClient);

private:
	CFolderNodeCch(MVirtualFolderClient& aClient);
	};

#endif //PBAPFOLDERNODECH_H
