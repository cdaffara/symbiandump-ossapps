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

#include "agspermanentdata.h"

#include "agsfilemanager.h"
#include "calcommonimpl.h"


CAgnPermanentData* CAgnPermanentData::NewL(CAgnServer& aServer, CAgnServFileMgr& aFileMgr)
	{
	CAgnPermanentData* self = new (ELeave) CAgnPermanentData(aServer, aFileMgr);
	return self;
	}

CAgnPermanentData::CAgnPermanentData(CAgnServer& aServer, CAgnServFileMgr& aFileMgr)
 : iServer(aServer), iFileMgr(aFileMgr)
	{
	}

CAgnPermanentData::~CAgnPermanentData()
	{
	Close();
	}

void CAgnPermanentData::Close()
	{
	if (iFile)
		{
		iFileMgr.CloseAgenda(*iFile, ETrue);
		iFile = NULL;
		}
	}

/*
  Starts the creation of the pre-created data. In this case this means opening
  the default Agenda file and starting the index building process. Building
  the indices is asynchronously handled within the CAgnServFile object (iFile).
*/
void CAgnPermanentData::StartDataCreationL()
	{
	if (!iFile)
		{		
		CalCommon::TCalFileVersionSupport dummyStatus;
		iFile = &iFileMgr.OpenAgendaL(KDefaultSecureAgendaFileName(), iServer, dummyStatus);		
		iFile->DoStartBuildIndex();
		}
	}

/*
  Releases the file if it is being held open by this object.
  
@param Filename of Agenda file
*/
void CAgnPermanentData::ReleaseFileL(const TDesC& aFilename)
	{
	if (iFile && (aFilename.CompareF(iFile->FileName()) == 0))
		{
		Close();
		}
	}

/*
  Checks whether this object is the only one that is currently using
  aFilename.
  
@return ETrue if this is the only object referencing aFilename, EFalse otherwise.
*/
TBool CAgnPermanentData::IsOnlyClientOfFile(const TDesC& aFilename)
	{
	if (iFile)
		{
		if ((aFilename.CompareF(iFile->FileName()) == 0) && iFile->ReferenceCount() == 1)
			{
			return ETrue;
			}
		}
	return EFalse;
	}
	
