// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CAgnPermanentData is a container to hold the handles/references 
// to data which is pre-created during server start-up when
// the Agenda server is started in non-transient mode.
// 
//

#ifndef __AGSPERMANENTDATA_H__
#define __AGSPERMANENTDATA_H__

#include <e32base.h>

class CAgnServer;
class CAgnServFile;
class CAgnServFileMgr;

NONSHARABLE_CLASS(CAgnPermanentData) : public CBase
	{
public:

	static CAgnPermanentData* NewL(CAgnServer& aServer, CAgnServFileMgr& aFileMgr);
	
	~CAgnPermanentData();

	void StartDataCreationL();
	TBool IsOnlyClientOfFile(const TDesC& aFilename);
	void ReleaseFileL(const TDesC& aFilename);
	
private:
	CAgnPermanentData(CAgnServer& aServer, CAgnServFileMgr& aFileMgr);
	void Close();
		
private:
	CAgnServer& iServer;
	CAgnServFileMgr& iFileMgr;
	CAgnServFile* iFile;
	};

#endif // __AGSPERMANENTDATA_H__
