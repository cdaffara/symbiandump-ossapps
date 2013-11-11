// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSFILESEARCH_H__
#define __AGSFILESEARCH_H__ 

#include <badesca.h>

class RFs;
class CDir;

NONSHARABLE_CLASS(CAgnFileScanner) : public CBase
	{
public:
	static CDesCArray* ListAgendaFilesL(RFs& aFs, const TUidType aFileType,const TDesC& aSearchPath);

private:
	CAgnFileScanner(RFs& aFs, const TUidType aFileType, const TDesC& aSearchPath);
	~CAgnFileScanner();
	CDesCArray* ScanLD();
	void GenerateRootDirsL();
	void ScanForFilesL(CDesCArray& aArray);
	void ScanDirectoryL(const TDesC& aDirPath, CDesCArray& aArray);
	void AppendFileNamesL(const CDir& aFileSet, const TDesC& aDirPath, CDesCArray& aArray, const TUidType* aUid) const;
private:
	RFs& iFs;
	TUidType iFileType;
	CDesCArray* iDirStack;
	TFileName iPath;
	//When the is specified the search only happens in that path otherwise in whole drive.
	};


#endif
