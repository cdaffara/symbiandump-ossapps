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

#ifndef __CIMAPFETCHSINGLEBODYSTRUCTURE_H__
#define __CIMAPFETCHSINGLEBODYSTRUCTURE_H__

#include "cimapfetchbodystructurebase.h"

class CImapFetchResponse;
class MOutputStream;

/**
Performs an IMAP FETCH command, requesting bodystructure and header information for
a single message.  The requested inforamtion is returned via the CImapFetchResponse 
object that is passed into the constructor.
Most of the processing for this command happens in the base class.
@internalTechnology
@prototype
*/
class CImapFetchSingleBodyStructure : public CImapFetchBodyStructureBase
	{
public:
	static CImapFetchSingleBodyStructure* NewL(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, TInt aMsgUid, const TDesC8& aHeaderFields, CImapFetchResponse& aFetchResponse);
	~CImapFetchSingleBodyStructure();

private:
	CImapFetchSingleBodyStructure(CImapFolderInfo* aSelectedFolderInfo, TInt aLogId, const TDesC8& aHeaderFields, CImapFetchResponse& aFetchResponse);
	void ConstructL(TInt aMsgUid);
	
private:
	HBufC8* iUidBuf;
	};

#endif // __CIMAPFETCHSINGLEBODYSTRUCTURE_H__
