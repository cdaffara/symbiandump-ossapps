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

// INCLUDE FILES
#include "FBFileOps.h"
#include "FBFileUtils.h"
#include "engine.h"
#include "FBTraces.h"

#ifndef FILEBROWSER_LITE
  #include "FBFileOpClient.h"
#endif  

#include <f32file.h>
#include <bautils.h>
#include <eikenv.h>
#include <babackup.h>

const TInt KSecureBackupStartDelay = 750000;
const TInt KSecureBackupLoopDelay = 100000;
const TInt KSecureBackupEndDelay = 200000;
const TInt KMaxFileLockAttempts = 3;


// ================= MEMBER FUNCTIONS =======================

CFileBrowserFileOps* CFileBrowserFileOps::NewL(CEngine* aEngine)
	{
	CFileBrowserFileOps* self = new(ELeave) CFileBrowserFileOps(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// --------------------------------------------------------------------------------------------

CFileBrowserFileOps::CFileBrowserFileOps(CEngine* aEngine) : iEngine(aEngine)
	{
	}

// --------------------------------------------------------------------------------------------

void CFileBrowserFileOps::ConstructL()
	{
	iRecursiveState = EFileOpInvalid;
	iSecureBackUpActive = EFalse;
	iFileCommandActivatedSecureBackup = EFalse;
	
    User::LeaveIfError(iFs.Connect());
    iFileMan = CFileMan::NewL( iFs, this );
	}

// --------------------------------------------------------------------------------------------

CFileBrowserFileOps::~CFileBrowserFileOps()
	{
	if (iSBEClient)
	    delete iSBEClient;
	
    #ifndef FILEBROWSER_LITE	
        if (iFileOpClient)
            delete iFileOpClient;
    #endif	

    delete iFileMan;
    iFs.Close();    
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::ActivateSecureBackUpViaFileOp()
    {
    iFileManObserverResult = MFileManObserver::EContinue;
    // if already activate by a file command, return ok
    if (iFileCommandActivatedSecureBackup)
        return KErrNone;
    else
        {
        // if secure backup is already active, disable it first, because it may in wrong state
        if (iSecureBackUpActive)
            DeActivateSecureBackUp();
        }
    
    // try to activate full secure backup
    TInt err = ActivateSecureBackUp(conn::EBURBackupFull, conn::EBackupBase);
    
    if (err == KErrNone)
        iFileCommandActivatedSecureBackup = ETrue;
    
    return err;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DeActivateSecureBackUpViaFileOp()
    {
    TInt err(KErrGeneral);
    iFileManObserverResult = MFileManObserver::EContinue;
    
    // if activate by a file command, try to reactivate it
    if (iFileCommandActivatedSecureBackup)
        {
        err = DeActivateSecureBackUp();
        
        // even if it fails, forget the state
        iFileCommandActivatedSecureBackup = EFalse;
        }

    return err;
    }    
    
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::ActivateSecureBackUp(conn::TBURPartType aPartType, conn::TBackupIncType aBackupIncType)
    {
    iFileManObserverResult = MFileManObserver::EContinue;
    // check for invalid parameters
    if (aPartType == conn::EBURNormal || aBackupIncType == conn::ENoBackup)
        User::Panic(_L("Inv.Usage.SE"), 532);
    
    TInt err(KErrNone);
    
    if (!iSBEClient)
        {
        TRAP(err, iSBEClient = conn::CSBEClient::NewL());
        if (err != KErrNone)
            return err;
        }
    
    TDriveList driveList;
    err = iFs.DriveList(driveList);
    
    if (err == KErrNone)
        {
        // make sure that the application has a system status to prevent getting shut down events
        iEngine->EikonEnv()->SetSystem(ETrue);
    
        // activating secure back up removes locks from files which respect this fuctionality                
        TRAP(err, iSBEClient->SetBURModeL(driveList, aPartType, aBackupIncType));
        
        if (err == KErrNone)
            {
            iSecureBackUpActive = ETrue;
            User::After(KSecureBackupStartDelay); // a short delay to wait to activate
            }
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DeActivateSecureBackUp()
    {
    TInt err(KErrNone);
    
    if (!iSBEClient)
        {
        TRAP(err, iSBEClient = conn::CSBEClient::NewL());
        if (err != KErrNone)
            return err;
        
        // make sure that the application has a system status
        iEngine->EikonEnv()->SetSystem(ETrue);
        }
        
    TDriveList driveList;
    err = iFs.DriveList(driveList);
    
    if (err == KErrNone)
        {
        // deactivate secure backup
        TRAP(err, iSBEClient->SetBURModeL(driveList, conn::EBURNormal, conn::ENoBackup));
        
        User::After(KSecureBackupEndDelay); // a short delay to wait to deactivate

        // system status not needed anymore
        iEngine->EikonEnv()->SetSystem(EFalse);
        }
    
    iSecureBackUpActive = EFalse;

    return err;
    }
        
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFindEntries(const TDesC& aFileName, const TDesC& aPath)
    {
    TFindFile fileFinder(iFs);
    CDir* dir;
    TInt err = fileFinder.FindWildByPath(aFileName, &aPath, dir);

    while (err == KErrNone && iFileManObserverResult != MFileManObserver::ECancel)
        {
        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];

            if (entry.iName.Length() && aPath.Length())
                {
                // parse the entry
                TParse parsedName;
                parsedName.Set(entry.iName, &fileFinder.File(), NULL);
                
                if (parsedName.Drive().Length() && aPath.Length() && parsedName.Drive()[0] == aPath[0])
                    {
                    // get full source path
                    TFileName fullSourcePath = parsedName.FullName();
                    if (entry.IsDir())
                        fullSourcePath.Append(_L("\\"));
                    
                    // call the file operation command
                    switch(iRecursiveState)
                        {
                        case EFileOpAttribs:
                            {
                            // the same attribs command can be given for both directories and files
                            FileOpAttribs(fullSourcePath, iUint1, iUint2, iTime1, iUint3);
                            }
                            break;

                        case EFileOpCopy:
                            {
                            // calculate length of new entries added to the original source path
                            TUint newEntriesLength = fullSourcePath.Length() - iBuf1.Length();
                            
                            // get pointer description to the rightmost data
                            TPtr16 newEntriesPtr = fullSourcePath.RightTPtr(newEntriesLength);
                            
                            // generate target path
                            TFileName fullTargetPath = iBuf2;
                            fullTargetPath.Append(newEntriesPtr);
                            
                            if (entry.IsDir())
                                {
                                // if it is a directory entry, just create it based on the entry's attributes
                                FileOpMkDirAll(fullTargetPath, entry.iAtt);
                                }
                            else
                                {
                                // ensure that root target folder exists
                                BaflUtils::EnsurePathExistsL(iFs, iBuf2);
                                // otherwise copy the file
                                FileOpCopy(fullSourcePath, fullTargetPath, iUint1);
                                }
                            }
                            break;

                        case EFileOpDelete:
                            {
                            if (entry.IsDir())
                                {
                                // for directories call rmdir    
                                FileOpRmDir(fullSourcePath, iUint1);
                                }
                            else
                                {
                                // for files call the normal file deletion operation    
                                FileOpDeleteFile(fullSourcePath, iUint1);
                                }
                            }
                            break;
                                                    
                        default:
                            User::Panic (_L("FileOpRecurs"), 775);
                            break;
                        }                        
                    }
                }
                if ( iFileManObserverResult == MFileManObserver::ECancel ) break;
            }

        delete dir;
        dir = NULL;
        if ( iFileManObserverResult != MFileManObserver::ECancel )
            {
            err = fileFinder.FindWild(dir);
            }
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFindEntriesRecursiveL(const TDesC& aFileName, const TDesC& aPath)
	{
    TInt err(KErrNone);

    // it is logical to scan upwards when deleting and changing attributes
    CDirScan::TScanDirection scanDirection = CDirScan::EScanUpTree;
    
    // when copying files, it is more logical to move downwards
    if (iRecursiveState == EFileOpCopy)
        scanDirection = CDirScan::EScanDownTree;
    
    
    CDirScan* scan = CDirScan::NewLC(iFs);
    scan->SetScanDataL(aPath, KEntryAttDir|KEntryAttMatchMask, ESortByName | EAscending | EDirsFirst, scanDirection);
    CDir* dir = NULL;

    for(;;)
        {
        TRAP(err, scan->NextL(dir));
        if (!dir  || (err != KErrNone))
            break;

        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];
            
            if (entry.IsDir())
                {
                TFileName path(scan->FullPath());
                
                if (path.Length())
                    {
                    path.Append(entry.iName);
                    path.Append(_L("\\"));
                    // test path.Left(iBuf2.Length()).Compare(iBuf2) - to prevent never ending recursive copy (in case of copy folder under itself)
                    if( !(iRecursiveState == EFileOpCopy && !path.Left(iBuf2.Length()).Compare(iBuf2)) )
                        {
                        DoFindEntries(aFileName, path);
                        }
                    }
                }
            if ( iFileManObserverResult == MFileManObserver::ECancel )
                {
                break;
                }
            }
        delete(dir);
        if ( iFileManObserverResult == MFileManObserver::ECancel )
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(scan);
    return err;
    }
    
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::Copy(const TFileEntry& aSourceEntry, const TDesC& aTargetFullName, TUint aSwitch, TBool aDeleteSource) 
    {
    iOperationError = KErrNone;
    iFileManObserverResult = MFileManObserver::EContinue;
    
    TFileName sourcePath = aSourceEntry.iPath;
    sourcePath.Append(aSourceEntry.iEntry.iName);
    
    TInt err(KErrNone);

    if (aSourceEntry.iEntry.IsDir() && (aSwitch & CFileMan::ERecurse))
        {
        // find all files recursively and run the operation for them
        iRecursiveState = EFileOpCopy;
        sourcePath.Append(_L("\\"));
        
        TFileName targetPath = aTargetFullName;
        targetPath.Append(_L("\\"));
        
        // remove the recursion flag because we will implement our own recursion
        TUint newSwitch(aSwitch);
        newSwitch &= ~CFileMan::ERecurse;
    
        iBuf1.Copy(sourcePath);
        iBuf2.Copy(targetPath);
        iUint1 = newSwitch;
        
        TRAP(err, DoFindEntries(_L("*"), sourcePath));              // entries under current directory entry
        if ( iFileManObserverResult != MFileManObserver::ECancel )
            {
            TRAP(err, DoFindEntriesRecursiveL(_L("*"), sourcePath));    // recursively under directories of current directory entry
            }
        // ensure that target folder exists in case of copying empty folder
        BaflUtils::EnsurePathExistsL(iFs, targetPath);
        // a path has a trailing backslash so it needs to be removed before the call
        err = FileOpAttribs(targetPath.Left(targetPath.Length()-1), aSourceEntry.iEntry.iAtt, 0, 0, 0);
        }

    else if (aSourceEntry.iEntry.IsDir())
        {
        TFileName targetPath = aTargetFullName;
        targetPath.Append(_L("\\"));
        
        // just create a directory based on the file attributes of the source directory
        err = FileOpMkDirAll(targetPath, aSourceEntry.iEntry.iAtt);
        }
        
    else
        {
        // remove a recursion flag if present (this should never happen, but some extra error checking)
        if (aSwitch & CFileMan::ERecurse)
            aSwitch &= ~CFileMan::ERecurse;    
            
        // do the operation for a file entry
        err = FileOpCopy(sourcePath, aTargetFullName, aSwitch);
        }
    
        
    // delete source if needed and copy succeeded without any errors (== move operation)
    if ( aDeleteSource && iOperationError == KErrNone &&
         iFileManObserverResult != MFileManObserver::ECancel )
        {
        err = Delete(aSourceEntry, aSwitch);    
        }
    
    if ( !iOperationError && iFileManObserverResult == MFileManObserver::ECancel )
        {
        iOperationError = KErrCancel;
        }
        
    return iOperationError; 
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::FileOpCopy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch) 
    {
    TInt err = DoFileOpCopy(aSourceFullName, aTargetFullName, aSwitch);

    // if locked, unlock the file and retry
    if (iEngine->Settings().iRemoveFileLocks && err == KErrInUse)
        {
        // try to remove the file lock by activating secure backup mode
        if (ActivateSecureBackUpViaFileOp() == KErrNone)
            {
            // try the operation several times
            for (TInt i=0; i<KMaxFileLockAttempts; i++)
                {
                err = DoFileOpCopy(aSourceFullName, aTargetFullName, aSwitch);
                
                if (err != KErrInUse)
                    break;
                else
                    User::After(KSecureBackupLoopDelay);
                }
            }
        }

    // if access denied, then try to remove the target path and try again
    if (iEngine->Settings().iIgnoreProtectionsAtts && err == KErrAccessDenied && BaflUtils::FileExists(iFs, aTargetFullName))
        {
        if (FileOpDeleteFile(aTargetFullName, 0) == KErrNone)
            {
            err = DoFileOpCopy(aSourceFullName, aTargetFullName, aSwitch);
            }
        }

    // if the file already exists, it is not an error    
    if (err == KErrAlreadyExists)
        err = KErrNone;
    
    
    // if copying from a ROM drive, remove the writing protection flag
    if (iEngine->Settings().iRemoveROMWriteProrection && err == KErrNone && aSourceFullName.Length() > 3 && (aSourceFullName[0]=='z' || aSourceFullName[0]=='Z'))
        {
        FileOpAttribs(aTargetFullName, 0, KEntryAttReadOnly, 0, 0);
        }
    
    
    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;

    LOGSTRING4("FileBrowser: FileOpCopy %S -> %S, err=%d", &aSourceFullName, &aTargetFullName, err);
        
    return err; 
    }
            
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFileOpCopy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch) 
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   
            
        return iFileOpClient->Copy(aSourceFullName, aTargetFullName, aSwitch);
        }
    else
        {
    #endif
        //CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
        //TInt result = iFileMan->Copy( aSourceFullName, aTargetFullName, aSwitch, waiter->iStatus );
        //waiter->StartAndWait();
        //if ( !result ) result = waiter->Result();
        //CleanupStack::PopAndDestroy( waiter );
        TInt result = iFileMan->Copy( aSourceFullName, aTargetFullName, aSwitch );
        return result;
    #ifndef FILEBROWSER_LITE
        }    
    #endif
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::Rename(const TFileEntry& aSourceEntry, const TDesC& aNew, TUint aSwitch)
    {
    iOperationError = KErrNone;
    iFileManObserverResult = MFileManObserver::EContinue;
    
    TFileName sourcePath = aSourceEntry.iPath;
    sourcePath.Append(aSourceEntry.iEntry.iName);
    
    if (aSourceEntry.iEntry.IsDir())
        {
        // do the operation for a directory entry
        FileOpRename(sourcePath, aNew, aSwitch);
        }
    else
        {
        // do the operation for a file
        FileOpRename(sourcePath, aNew, aSwitch);
        }

    return iOperationError; 
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::FileOpRename(const TDesC& aName, const TDesC& aNew, TUint aSwitch)
    {
    TBool setBackROFlag(EFalse);

    TInt err = DoFileOpRename(aName, aNew, aSwitch);

    // if locked, unlock the file and retry
    if (iEngine->Settings().iRemoveFileLocks && err == KErrInUse)
        {
        // try to remove the file lock by activating secure backup mode
        if (ActivateSecureBackUpViaFileOp() == KErrNone)
            {
            // try the operation several times
            for (TInt i=0; i<KMaxFileLockAttempts; i++)
                {
                err = DoFileOpRename(aName, aNew, aSwitch);
                
                if (err != KErrInUse)
                    break;
                else
                    User::After(KSecureBackupLoopDelay);
                }
            }
        }

    // if write protected, remove protection and retry
    else if (iEngine->Settings().iIgnoreProtectionsAtts && err == KErrAccessDenied)
        {
        // remove write protection and try again
        if (FileOpAttribs(aName, 0, KEntryAttReadOnly, 0, 0) == KErrNone)
            {
            err = DoFileOpRename(aName, aNew, aSwitch);
            
            setBackROFlag = ETrue;
            }
        }

    // if still access denied, then try to remove the target path and try again
    if (iEngine->Settings().iIgnoreProtectionsAtts && err == KErrAccessDenied && BaflUtils::FileExists(iFs, aNew))
        {
        if (FileOpDeleteFile(aNew, 0) == KErrNone)
            {
            err = DoFileOpRename(aName, aNew, aSwitch);
            }
        }

    // set back the read only flag
    if (setBackROFlag)
        FileOpAttribs(aName, KEntryAttReadOnly, 0, 0, 0);
            

    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;

    LOGSTRING3("FileBrowser: FileOpRename %S, err=%d", &aName, err);
        
    return err; 
    }
    
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFileOpRename(const TDesC& aName, const TDesC& aNew, TUint aSwitch)
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   

        return iFileOpClient->Rename(aName, aNew, aSwitch);
        }
    else
        {
    #endif
        return iFileMan->Rename(aName, aNew, aSwitch);
    #ifndef FILEBROWSER_LITE
        }    
    #endif
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::Attribs(const TFileEntry& aSourceEntry, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch) 
    {
    iOperationError = KErrNone;
    iFileManObserverResult = MFileManObserver::EContinue;
    
    TFileName sourcePath = aSourceEntry.iPath;
    sourcePath.Append(aSourceEntry.iEntry.iName);
    
    TInt err(KErrNone);

    if (aSourceEntry.iEntry.IsDir() && (aSwitch & CFileMan::ERecurse))
        {
        // do the operation for a current directory entry
        err = FileOpAttribs(sourcePath, aSetMask, aClearMask, aTime, 0);

        // find all files recursively and run the operation for them
        iRecursiveState = EFileOpAttribs;
        sourcePath.Append(_L("\\"));
    
        iBuf1.Copy(sourcePath);
        iUint1 = aSetMask;
        iUint2 = aClearMask;
        iTime1 = aTime;
        iUint3 = 0;
        
        TRAP(err, DoFindEntriesRecursiveL(_L("*"), sourcePath));    // recursively under directories of current directory entry
        TRAP(err, DoFindEntries(_L("*"), sourcePath));              // entries under current directory entry
        }

    else if (aSourceEntry.iEntry.IsDir())
        {
        //sourcePath.Append(_L("\\"));   // <-- do not apply!
    
        // do the operation for a directory entry
        err = FileOpAttribs(sourcePath, aSetMask, aClearMask, aTime, 0);
        }
            
    else
        {
        // do the operation for a file entry
        err = FileOpAttribs(sourcePath, aSetMask, aClearMask, aTime, 0);
        }

    return iOperationError;     
    }

// --------------------------------------------------------------------------------------------
    
TInt CFileBrowserFileOps::FileOpAttribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch) 
    {
    TInt err = DoFileOpAttribs(aName, aSetMask, aClearMask, aTime, aSwitch);

    // if locked, unlock the file and retry
    if (iEngine->Settings().iRemoveFileLocks && err == KErrInUse)
        {
        // try to remove the file lock by activating secure backup mode
        if (ActivateSecureBackUpViaFileOp() == KErrNone)
            {
            // try the operation several times
            for (TInt i=0; i<KMaxFileLockAttempts; i++)
                {
                err = DoFileOpAttribs(aName, aSetMask, aClearMask, aTime, aSwitch);
                
                if (err != KErrInUse)
                    break;
                else
                    User::After(KSecureBackupLoopDelay);
                }
            }
        }

    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;

    LOGSTRING3("FileBrowser: FileOpAttribs %S, err=%d", &aName, err);
        
    return err;    
    }
    
// --------------------------------------------------------------------------------------------
    
TInt CFileBrowserFileOps::DoFileOpAttribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch) 
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   

        return iFileOpClient->Attribs(aName, aSetMask, aClearMask, aTime, aSwitch);
        }
    else
        {
    #endif
        return iFileMan->Attribs(aName, aSetMask, aClearMask, aTime, aSwitch);
    #ifndef FILEBROWSER_LITE
        }    
    #endif
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::Delete(const TFileEntry& aSourceEntry, TUint aSwitch) 
    {
    iOperationError = KErrNone;
    iFileManObserverResult = MFileManObserver::EContinue;
    
    TFileName sourcePath = aSourceEntry.iPath;
    sourcePath.Append(aSourceEntry.iEntry.iName);
    
    TInt err(KErrNone);

    if (aSourceEntry.iEntry.IsDir() && (aSwitch & CFileMan::ERecurse))
        {
        // find all files recursively and run the operation for them
        iRecursiveState = EFileOpDelete;
        sourcePath.Append(_L("\\"));
    
        iBuf1.Copy(sourcePath);
        iUint1 = 0;
        
        TRAP(err, DoFindEntriesRecursiveL(_L("*"), sourcePath));    // recursively under directories of current directory entry
        if ( iFileManObserverResult != MFileManObserver::ECancel )
            {
            TRAP(err, DoFindEntries(_L("*"), sourcePath));              // entries under current directory entry
            }
        
        if ( iFileManObserverResult != MFileManObserver::ECancel )
            {
            // do the operation for a current directory entry as well
            err = FileOpRmDir(sourcePath, 0);
            }
        }

    else if (aSourceEntry.iEntry.IsDir())
        {
        sourcePath.Append(_L("\\"));
    
        // do the operation for a directory entry
        err = FileOpRmDir(sourcePath, 0);
        }

    else
        {
        // do the operation for a file entry
        err = FileOpDeleteFile(sourcePath, 0);
        }
    if ( !iOperationError && iFileManObserverResult == MFileManObserver::ECancel )
        {
        iOperationError = KErrCancel;
        }
    return iOperationError; 
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::FileOpDeleteFile(const TDesC& aName, TUint aSwitch) 
    {
    TInt err = DoFileOpDeleteFile(aName, aSwitch);

    // if locked, unlock the file and retry
    if (iEngine->Settings().iRemoveFileLocks && err == KErrInUse)
        {
        // try to remove the file lock by activating secure backup mode
        if (ActivateSecureBackUpViaFileOp() == KErrNone)
            {
            // try the operation several times
            for (TInt i=0; i<KMaxFileLockAttempts; i++)
                {
                err = DoFileOpDeleteFile(aName, aSwitch);
                
                if (err != KErrInUse)
                    break;
                else
                    User::After(KSecureBackupLoopDelay);
                }
            }
        }

    // if write protected or system file, remove protections and retry
    else if (iEngine->Settings().iIgnoreProtectionsAtts && (err == KErrAccessDenied || err == KErrNotFound))
        {
        // remove protections  and try again
        if (FileOpAttribs(aName, 0, KEntryAttReadOnly|KEntryAttSystem|KEntryAttHidden, 0, 0) == KErrNone)
            {
            err = DoFileOpDeleteFile(aName, aSwitch);
            }
        }

    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;

    LOGSTRING3("FileBrowser: FileOpDeleteFile %S, err=%d", &aName, err);
        
    return err; 
    }
        
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFileOpDeleteFile(const TDesC& aName, TUint aSwitch) 
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   

        return iFileOpClient->Delete(aName, aSwitch);
        }
    else
        {
    #endif
//        CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
//        TInt result = iFileMan->Delete( aName, aSwitch, waiter->iStatus );
//        waiter->StartAndWait();
//        if ( iFileManObserverResult == MFileManObserver::ECancel ) result = KErrCancel;
//        if ( !result ) result = waiter->Result();
//        CleanupStack::PopAndDestroy( waiter );
		TInt result = iFileMan->Delete( aName, aSwitch );
        return result;
    #ifndef FILEBROWSER_LITE
        }    
    #endif
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::FileOpRmDir(const TDesC& aName, TUint aSwitch) 
    {
    TInt err = DoFileOpRmDir(aName, aSwitch);

    // if write protected or system directory, remove protections and retry
    if (iEngine->Settings().iIgnoreProtectionsAtts && (err == KErrAccessDenied || err == KErrInUse))
        {
        // remove protections and try again
        if (FileOpAttribs(aName.Left(aName.Length()-1), 0, KEntryAttReadOnly|KEntryAttSystem|KEntryAttHidden, 0, 0) == KErrNone)
            {
            err = DoFileOpRmDir(aName, aSwitch);
            }
        }

    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;

    LOGSTRING3("FileBrowser: FileOpRmDir %S, err=%d", &aName, err);
        
    return err; 
    }
        
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFileOpRmDir(const TDesC& aDirName, TUint aSwitch)
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   

        return iFileOpClient->RmDir(aDirName, aSwitch);
        }
    else
        {
    #endif
        if ( aSwitch & CFileMan::ERecurse )
            {
//            CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
//            TInt result = iFileMan->RmDir( aDirName, waiter->iStatus );
//            waiter->StartAndWait();
//            if ( iFileManObserverResult == MFileManObserver::ECancel ) result = KErrCancel;
//            if ( !result ) result = waiter->Result();
//            CleanupStack::PopAndDestroy( waiter);
            TInt result = iFileMan->RmDir( aDirName );
            return result;
            }
        else
            return iFs.RmDir(aDirName);    
    #ifndef FILEBROWSER_LITE
        }    
    #endif
   }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::MkDirAll(const TDesC& aPath, TInt aSetAtts, TBool aQuickOperation) 
    {
    iFileManObserverResult = MFileManObserver::EContinue;
    if (aQuickOperation)
        return DoFileOpMkDirAll(aPath);
    else
        return FileOpMkDirAll(aPath, aSetAtts);
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::FileOpMkDirAll(const TDesC& aPath, TInt aSetAtts) 
    {
    TInt err = DoFileOpMkDirAll(aPath);

    // if the directory already exists, it is not an error    
    if (err == KErrAlreadyExists)
        err = KErrNone;
    
    
    // set attributes for directory just created
    if (aSetAtts > 0 && err == KErrNone && aPath.Length() > 3)
        {
        // a path has a trailing backslash so it needs to be removed before the call
        err = FileOpAttribs(aPath.Left(aPath.Length()-1), aSetAtts, 0, 0, 0);
        }
    

    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;
    
    LOGSTRING3("FileBrowser: FileOpMkDirAll %S, err=%d", &aPath, err);
    
    return err;
    }
        
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DoFileOpMkDirAll(const TDesC& aPath) 
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   

        return iFileOpClient->MkDirAll(aPath);
        }
    else
        {
    #endif
        return iFs.MkDirAll(aPath);
    #ifndef FILEBROWSER_LITE
        }    
    #endif
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::CreateEmptyFile(const TDesC& aName) 
    {
    return DoFileOpCreateEmptyFile(aName);
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::FileOpCreateEmptyFile(const TDesC& aName) 
    {
    TInt err = DoFileOpCreateEmptyFile(aName);

    // remember the "lowest" error
    if (err < iOperationError)
        iOperationError = err;
    
    LOGSTRING3("FileBrowser: FileOpCreateEmptyFile %S, err=%d", &aName, err);
    
    return err;
    }

// --------------------------------------------------------------------------------------------
    
TInt CFileBrowserFileOps::DoFileOpCreateEmptyFile(const TDesC& aName) 
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();   

        return iFileOpClient->CreateEmptyFile(aName);
        }
    else
        {
    #endif
        TInt err(KErrNone);
            
        RFile newFile;
        err = newFile.Create(iFs, aName, EFileShareExclusive);
        if (err == KErrNone)
            err = newFile.Flush(); 
        newFile.Close();
        
        return err; 
    #ifndef FILEBROWSER_LITE
        }    
    #endif
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::DriveSnapShot(TChar aSourceDriveLetter, TChar aTargetDriveLetter)
    {
    TInt err(KErrNone);
    iFileManObserverResult = MFileManObserver::EContinue;
    
    // remember old settings and force them to be true for this operation
    TBool currentRemoveFileLocksValue = iEngine->Settings().iRemoveFileLocks;
    TBool currentIgnoreProtectionAttsValue = iEngine->Settings().iIgnoreProtectionsAtts;
    TBool currentRemoveROMWriteProrection = iEngine->Settings().iRemoveROMWriteProrection;
    iEngine->Settings().iRemoveFileLocks = ETrue;
    iEngine->Settings().iIgnoreProtectionsAtts = ETrue;
    iEngine->Settings().iRemoveROMWriteProrection = ETrue;

    
    TFileName sourceDir;
    sourceDir.Append(aSourceDriveLetter);
    sourceDir.Append(_L(":"));

    _LIT(KTargetDir, "%c:\\SnapShot_%c_drive");
    TFileName targetDir;
    targetDir.Format(KTargetDir, TUint(aTargetDriveLetter), TUint(aSourceDriveLetter));            

    // remove any existing content, first get TEntry
    TEntry entry;
    err = iFs.Entry(targetDir, entry);

    // entry directory exists, delete it
    if (err == KErrNone && entry.IsDir())
        {
        TFileName targetRoot;
        targetRoot.Append(aTargetDriveLetter);
        targetRoot.Append(_L(":\\"));

        TFileEntry targetEntry;
        targetEntry.iPath = targetRoot;
        targetEntry.iEntry = entry;

        err = Delete(targetEntry, CFileMan::ERecurse);                
        }
        
    // do not care if removing succeeded or not, just continue with copying    
    TEntry fakeEntry;
    fakeEntry.iAtt |= KEntryAttDir;

    TFileEntry sourceEntry;
    sourceEntry.iPath = sourceDir;
    sourceEntry.iEntry = fakeEntry;

    err = Copy(sourceEntry, targetDir, CFileMan::ERecurse|CFileMan::EOverWrite);                


    // restore back settings
    iEngine->Settings().iRemoveFileLocks = currentRemoveFileLocksValue;
    iEngine->Settings().iIgnoreProtectionsAtts = currentIgnoreProtectionAttsValue;    
    iEngine->Settings().iRemoveROMWriteProrection = currentRemoveROMWriteProrection;    


    return iOperationError;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::EraseMBR(TUint aDriveNumber) 
    {
    #ifndef FILEBROWSER_LITE

        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();
        
        return iFileOpClient->EraseMBR(aDriveNumber);

    #else
        return KErrNotSupported;
    #endif
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileOps::PartitionDrive(TUint aDriveNumber, TUint aNumberOfPartitions) 
    {
    #ifndef FILEBROWSER_LITE

        if (!iFileOpClient)
            iFileOpClient = CFBFileOpClient::NewL();
        
        return iFileOpClient->PartitionDrive(aDriveNumber, aNumberOfPartitions);

    #else
        return KErrNotSupported
    #endif
    }

// --------------------------------------------------------------------------------------------

MFileManObserver::TControl CFileBrowserFileOps::NotifyFileManStarted()
    {
    return iFileManObserverResult;
    }

// --------------------------------------------------------------------------------------------

MFileManObserver::TControl CFileBrowserFileOps::NotifyFileManOperation()
    {
    return iFileManObserverResult;
    }
// --------------------------------------------------------------------------------------------

MFileManObserver::TControl CFileBrowserFileOps::NotifyFileManEnded()
    {
    return iFileManObserverResult;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileOps::CancelOp()
    {
#ifndef FILEBROWSER_LITE

    if ( iEngine->Settings().iBypassPlatformSecurity )
        {
        if ( !iFileOpClient )
            iFileOpClient = CFBFileOpClient::NewL();
        
        iFileOpClient->CancelOp();
        }

    // we need this information even when using iFileOpClient
    // to be able to not execute aggregate operations 
    iFileManObserverResult = MFileManObserver::ECancel;
    
#else
    iFileManObserverResult = MFileManObserver::ECancel;
#endif
    }

// End of File
