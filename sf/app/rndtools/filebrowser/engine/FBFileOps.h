/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FILEBROWSER_FILEOPS_H
#define FILEBROWSER_FILEOPS_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <connect/sbeclient.h>

// FORWARD DECLARATIONS
class TFileEntry;
class CFBFileOpClient;
class CFileMan;
class CEngine;
class CSBEClient;


class CFileBrowserFileOps : public CBase,
                            public MFileManObserver 
	{
private:
    enum TRecursiveOperation
    	{
    	EFileOpInvalid = -1,
    	EFileOpCopy,
    	EFileOpMove,
    	EFileOpRename,
    	EFileOpAttribs,
    	EFileOpDelete,
    	EFileOpMkDirAll,
    	EFileOpCreateEmptyFile,
    	EFileOpEraseMBR,
    	EFileOpPartitionDrive
    	};
public:
	static CFileBrowserFileOps* NewL(CEngine* aEngine);
	~CFileBrowserFileOps();

public: // From MFileManObserver
    
    TControl NotifyFileManStarted();
    TControl NotifyFileManOperation();
    TControl NotifyFileManEnded();
    
private:
	CFileBrowserFileOps(CEngine* aEngine);
	void ConstructL();

public:
    TInt ActivateSecureBackUpViaFileOp();
    TInt DeActivateSecureBackUpViaFileOp();
    TInt ActivateSecureBackUp(conn::TBURPartType aPartType=conn::EBURBackupFull, conn::TBackupIncType aBackupIncType=conn::EBackupBase);
    TInt DeActivateSecureBackUp();
    TInt Copy(const TFileEntry& aSourceEntry, const TDesC& aTargetFullName, TUint aSwitch=CFileMan::EOverWrite, TBool aDeleteSource=EFalse); 
    TInt Rename(const TFileEntry& aSourceEntry, const TDesC& aNew, TUint aSwitch=CFileMan::EOverWrite); 
    TInt Attribs(const TFileEntry& aSourceEntry, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch=0); 
    TInt Delete(const TFileEntry& aSourceEntry, TUint aSwitch=0); 
    TInt MkDirAll(const TDesC& aPath, TInt aSetAtts=0, TBool aQuickOperation=EFalse); 
    TInt CreateEmptyFile(const TDesC& aName); 
    TInt DriveSnapShot(TChar aSourceDriveLetter, TChar aTargetDriveLetter); 
    TInt EraseMBR(TUint aDriveNumber); 
    TInt PartitionDrive(TUint aDriveNumber, TUint aNumberOfPartitions);
    void CancelOp();

private:
    TInt DoFindEntries(const TDesC& aFileName, const TDesC& aPath);
    TInt DoFindEntriesRecursiveL(const TDesC& aFileName, const TDesC& aPath);    

    TInt FileOpCopy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch); 
    TInt DoFileOpCopy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch); 

    TInt FileOpRename(const TDesC& aName, const TDesC& aNew, TUint aSwitch); 
    TInt DoFileOpRename(const TDesC& aName, const TDesC& aNew, TUint aSwitch); 

    TInt FileOpAttribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch); 
    TInt DoFileOpAttribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch); 

    TInt FileOpDeleteFile(const TDesC& aName, TUint aSwitch); 
    TInt DoFileOpDeleteFile(const TDesC& aName, TUint aSwitch); 

    TInt FileOpRmDir(const TDesC& aDirName, TUint aSwitch); 
    TInt DoFileOpRmDir(const TDesC& aDirName, TUint aSwitch); 

    TInt FileOpMkDirAll(const TDesC& aPath, TInt aSetAtts=0); 
    TInt DoFileOpMkDirAll(const TDesC& aPath); 

    TInt FileOpCreateEmptyFile(const TDesC& aName); 
    TInt DoFileOpCreateEmptyFile(const TDesC& aName); 

public:
    inline TBool SecureBackUpActive() { return iSecureBackUpActive; }
    inline TBool FileCommandActivatedSecureBackup() { return iFileCommandActivatedSecureBackup; }

private:
    TInt                            iRecursiveState;
    CEngine*                        iEngine;
    CFBFileOpClient*                iFileOpClient;
    CFileMan*                       iFileMan;
    RFs                             iFs;
    TInt                            iOperationError;
    conn::CSBEClient*               iSBEClient;
    TBool                           iSecureBackUpActive;
    TBool                           iFileCommandActivatedSecureBackup;

    TFileName   iBuf1;
    TFileName   iBuf2;
    TUint       iUint1;
    TUint       iUint2;
    TUint       iUint3;
    TTime       iTime1;
    MFileManObserver::TControl iFileManObserverResult;
    };

#endif

// End of File

