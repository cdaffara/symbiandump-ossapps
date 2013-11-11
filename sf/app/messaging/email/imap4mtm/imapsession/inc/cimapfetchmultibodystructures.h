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

#ifndef __CIMAPFETCHMULTIBODYSTRUCTURES_H__
#define __CIMAPFETCHMULTIBODYSTRUCTURES_H__

#include "cimapfetchbodystructurebase.h"

class MImapFetchStructureObserver;
class MOutputStream;

/**
Performs an IMAP FETCH command, requesting bodystructure and header information for
a multiple messages.  The requested inforamtion is returned via the MImapFetchStructureObserver 
callback interface that is passed into the constructor.  The observer's callback is
called once for each requested message, as soon as the data for each message is received.
Most of the processing for this command happens in the base class.
@internalTechnology
@prototype
*/
class CImapFetchMultiBodyStructures : public CImapFetchBodyStructureBase
	{
public:
	static CImapFetchMultiBodyStructures* NewL(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aSequenceSet, const TDesC8& aHeaderFields, MImapFetchStructureObserver& aObserver);
	~CImapFetchMultiBodyStructures();

private:
	CImapFetchMultiBodyStructures(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aSequenceSet, const TDesC8& aHeaderFields, MImapFetchStructureObserver& aObserver);
	void ConstructL();
	
	// From CImapCommand
	CImapCommand::TParseBlockResult DoParseBlockL(const TDesC8& aData);
	void WaitingForMoreDataL();

private:
	MImapFetchStructureObserver& iObserver;
	};

#endif // __CIMAPFETCHMULTIBODYSTRUCTURES_H__
