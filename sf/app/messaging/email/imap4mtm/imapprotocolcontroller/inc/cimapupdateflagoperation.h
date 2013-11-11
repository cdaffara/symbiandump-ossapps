/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __CIMAPUPDATEFLAGOPERATION_H__
#define __CIMAPUPDATEFLAGOPERATION_H__

#include "cimapcompoundbase.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"




/**
Sends updated Read/unread mail status to server

@internalTechnology
@released
*/
 NONSHARABLE_CLASS (CImapUpdateFlagOperation) : public CImapCompoundBase
	{
public:
	~CImapUpdateFlagOperation();
	
	static CImapUpdateFlagOperation* NewL(CImapSyncManager& aSyncManager,
						 			  CMsvServerEntry& aServerEntry,
						 			  CImapSettings& aImapSettings
						 			  );
	static CImapUpdateFlagOperation* NewL(CImapSyncManager& aSyncManager,
	                                      CMsvServerEntry& aServerEntry,
	                                      CImapSettings& aImapSettings,
	                                      const CMsvEntrySelection& aSourceSel,
	                                      TBool aFlagChanged
	                                      );
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	
////////////	virtual void StartOperation(TRequestStatus& aStatus);//
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapUpdateFlagOperation(CImapSyncManager& aSyncManager,
						 CMsvServerEntry& aServerEntry,
						 CImapSettings& aImapSettings
						 );
	void ConstructL();
	void ConstructL(const CMsvEntrySelection& aSourceSel,TBool aFlagChanged);

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();
						 
private:

	RArray<TUint> iMessageUids;
	
	RArrayMessageFlagInfo iFlagInfoResponseArray;
	
	CMsvEntrySelection* iSourceSel;
	TBool iMarkFlag;
	TBool iEntrySelection;
	TInt iProgressMsgsToDo;
	TInt iProgressMsgsDone;

	};

#endif	// __CIMAPUPDATEFLAGOPERATION_H__
