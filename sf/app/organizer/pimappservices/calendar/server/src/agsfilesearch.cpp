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

#include "agsfilesearch.h"
#include <f32file.h>
//
const TInt KNameArrayGranularity = 32;
//

CDesCArray* CAgnFileScanner::ListAgendaFilesL(RFs& aFs, const TUidType aFileType, const TDesC& aSearchPath)
	{
	CAgnFileScanner* scanner = new(ELeave) CAgnFileScanner(aFs, aFileType, aSearchPath);
	return scanner->ScanLD();
	}

CAgnFileScanner::CAgnFileScanner(RFs& aFs, const TUidType aFileType, const TDesC& aSearchPath)
: iFs(aFs), iFileType(aFileType), iPath(aSearchPath)
	{
	}

CAgnFileScanner::~CAgnFileScanner()
	{
	delete iDirStack;
	}

CDesCArray* CAgnFileScanner::ScanLD()
	{
	CleanupStack::PushL(this);

	iDirStack = new(ELeave) CDesCArrayFlat(KNameArrayGranularity);
	GenerateRootDirsL();

	CDesCArray* results = new(ELeave) CDesCArrayFlat(KNameArrayGranularity);
	CleanupStack::PushL(results);
	ScanForFilesL(*results);
	CleanupStack::Pop(results);
	CleanupStack::PopAndDestroy(this);

	return results;
	}

void CAgnFileScanner::GenerateRootDirsL()
	{
	ASSERT(iDirStack);
	// Generate list of root dirs.
	TBuf<3> root;
	TDriveList driveList;
	User::LeaveIfError(iFs.DriveList(driveList));
	TInt len = driveList.Length();
	for (TInt index=0;index<len;++index)
		{
		if(driveList[index]) // Check drive exists.
			{
			root.Copy(TDriveUnit(index).Name());
			root.Append(TChar(KPathDelimiter));
			iDirStack->AppendL(root);
			}
		}
	}

void CAgnFileScanner::ScanForFilesL(CDesCArray& aResults)
	{
	ASSERT(iDirStack);
	// Traverse the directory stack until all directories have been examined.
	// ScanDirectory may add new dirs to stack.
	TFileName filename;
	while(iDirStack->Count())
		{
		// Pop the dir.
		filename.Copy(iDirStack->MdcaPoint(0));
		iDirStack->Delete(0);
		if(iPath.Length()!=0)
			{
			filename.Delete(2,1);
			filename.Append(iPath);
			}
		else if(filename[filename.Length() - 1] != KPathDelimiter)
			{// Check it has a trailing '\' to identify it as a dir path.
			filename.Append(TChar(KPathDelimiter));
			}
		TRAPD(err,ScanDirectoryL(filename, aResults));//c:\private\1234\aaa c:\private\1234\bbb
		if(err==KErrNoMemory)
			{//continue to search in each drive except for out of memory
			User::Leave(err);
			}
		}
	}

void CAgnFileScanner::ScanDirectoryL(const TDesC& aDirPath, CDesCArray& aResults)
	{// Get a list of all files & subdirs in aDirPath (when the path is not specified).
	CDir* files = NULL;
	CDir* subdirs = NULL;

	User::LeaveIfError(iFs.GetDir(aDirPath, KEntryAttAllowUid, ESortByName, files, subdirs));

	TRAPD(err,AppendFileNamesL(*files, aDirPath, aResults, &iFileType));//the result when the file is in root
	delete files;
	CleanupStack::PushL(subdirs);
	
	User::LeaveIfError(err);
		// Add files matching iFileType to results.
	if(iPath.Length()==0)
	// Add all subdirs to dir stack.
		{
		AppendFileNamesL(*subdirs, aDirPath, *iDirStack, NULL);//the result where the file is in sub it is reused in ScanForFilesL which might not needed in secured data
		}
	CleanupStack::PopAndDestroy(subdirs);
	}

void CAgnFileScanner::AppendFileNamesL(const CDir& aFileSet, const TDesC& aDirPath, CDesCArray& aFileNames, const TUidType* aUid) const
	{
	TParse parse;
	TInt len = aFileSet.Count();
	for(TInt idx=0;idx<len;++idx)
		{
		const TEntry& entry = aFileSet[idx];
		User::LeaveIfError(parse.SetNoWild(entry.iName, &aDirPath, NULL));
		if(aUid)
			{
			if(entry.iType==(*aUid))
				{
				if(iPath.Length()>0)
					{//files are in server's private directory so the file should be in form drive:filename
					TFileName filename(parse.Drive());
					filename.Append(parse.NameAndExt());
					aFileNames.AppendL(filename);
					}
				else
					{
					aFileNames.AppendL(parse.FullName());
					}
				}
			}
		else
			{
			aFileNames.AppendL(parse.FullName());
			}
		}
	}


