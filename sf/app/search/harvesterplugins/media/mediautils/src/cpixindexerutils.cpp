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
* Description:  Indexer utility create indexes for each drive
 *
*/


#include "cpixindexerutils.h"
#include <ccpixindexer.h>
#include "harvesterserverlogger.h"
#include <driveinfo.h> // TDriveInfo

// maximum length that the fully qualified msg Plugin base app class descriptor can be
// e.g. "@c:root media"
const TInt KMsgPluginBaseAppClassMaxLen = 64;

// local declarations and functions
namespace {
_LIT(KCPixSearchServerPrivateDirectory, "\\Private\\2001f6f7\\");
_LIT(KIndexingDBPath,"indexing\\indexdb");
_LIT(KPathIndexDbPath, CPIX_INDEVICE_INDEXDB);

_LIT(KAtSign, "@");
_LIT(KColon, ":");
};

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CCPixIndexerUtils* CCPixIndexerUtils::NewL(RSearchServerSession& aSession)
    {
    CCPixIndexerUtils* self = CCPixIndexerUtils::NewLC(aSession);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewLC()
// -----------------------------------------------------------------------------
//
CCPixIndexerUtils* CCPixIndexerUtils::NewLC(RSearchServerSession& aSession)
    {
    CCPixIndexerUtils* self = new (ELeave) CCPixIndexerUtils();
    CleanupStack::PushL(self);
    self->ConstructL(aSession);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::~CCPixIndexerUtils()
// -----------------------------------------------------------------------------
//
CCPixIndexerUtils::~CCPixIndexerUtils()
    {
    iFs.Close();
    for(TInt i=0;i<(EDriveZ+1);i++)
        {
        delete iIndexer[i];
        iIndexer[i] = NULL;
        }
    iMountedDrives.Reset();
    }
    

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CCPixIndexerUtils()
// -----------------------------------------------------------------------------
//
CCPixIndexerUtils::CCPixIndexerUtils()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::ConstructL(RSearchServerSession& aSession)
    {
    User::LeaveIfError(iFs.Connect());
    iSearchSession = aSession;
    for(TInt i=0;i<(EDriveZ+1);i++)
        {
        iIndexer[i] = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetDriveFromMediaId()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::GetDriveFromMediaId(TUint aUniqueID,TDriveNumber& aDrive)
    {
    aDrive = TDriveNumber(KErrNotSupported);//Initialize to not supported first
    TVolumeInfo volumeInfo;
    for(TInt i=0;i<=EDriveZ;i++)
       {
       TInt err= iFs.Volume(volumeInfo,i);
        if (err!=KErrNotReady) 
            // Volume() returns KErrNotReady if no volume present.
            // In this case, check next drive number
            {
            if(volumeInfo.iUniqueID == aUniqueID)
                {
                aDrive = TDriveNumber(i);//Is the drive
                break;
                }
            }
       }
    }

//Where Genreic is :root media and base app class is the return
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::FormBaseAppClass()
// -----------------------------------------------------------------------------
//
TInt CCPixIndexerUtils::FormBaseAppClass(TDriveNumber aMedia,const 
                                         TDesC& aBaseAppClassGeneric,
                                         TDes& aBaseAppClass)
    {
    TChar chr;
    const TInt ret = RFs::DriveToChar(aMedia, chr);
    if (KErrNone == ret)
        {
        aBaseAppClass.Copy(KAtSign);
        aBaseAppClass.Append(chr);
        aBaseAppClass.LowerCase();
        //Append generic class like :root media audio or :root media video
        aBaseAppClass.Append(aBaseAppClassGeneric);
        }
    return ret;    
    }
    
//Where path is trailer e.g. \\root\\media\\audio
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::MountAllAvailableDriveL()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::MountAllAvailableDriveL(const TDesC& aBaseAppClassGeneric,
                                                const TDesC& aPath)
    {
    CPIXLOGSTRING("START CCPixIndexerUtils::MountAllAvailableDriveL");
    TDriveList drivelist; 
    User::LeaveIfError(iFs.DriveList(drivelist));
    TInt driveNumber;
    TChar driveLetter;
    for (driveNumber=EDriveA; driveNumber<=EDriveZ;driveNumber++)
        {
        if (drivelist[driveNumber] && 
                IsDriveCanbeMountedL(TDriveNumber(driveNumber))) // if drive-list entry non-zero, drive is available
            {
            User::LeaveIfError(iFs.DriveToChar(driveNumber,driveLetter));
            // The following line prints the drive letter followed by the hex value 
            // of the integer indicating that drive's attributes 
            
            // Check if already exists
            if (iIndexer[driveNumber])
                return;
            
            // Form the baseappclass for this media
            TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
            FormBaseAppClass(TDriveNumber(driveNumber),aBaseAppClassGeneric,baseAppClass);

            // Define this volume
            HBufC* path = DatabasePathLC(TDriveNumber(driveNumber),aPath);
            User::LeaveIfError(iSearchSession.DefineVolume(baseAppClass, *path));
            CleanupStack::PopAndDestroy(path);
            
            // construct and open the database
            TRAPD(err,iIndexer[driveNumber] = CCPixIndexer::NewL(iSearchSession));
            CPIXLOGSTRING2("CCPixIndexer::NewL returned : %d", err );
            TRAP(err,iIndexer[driveNumber]->OpenDatabaseL(baseAppClass));
            if(err == KErrNone)
                {
                iMountedDrives.Append(TDriveNumber(driveNumber));
                CPIXLOGSTRING2("CCPixIndexerUtils::MountAllAvailableDriveL Drive %d Mounted",driveNumber);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::UnMountAllDrivesL()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::UnMountAllDrivesL(const TDesC& aBaseAppClassGeneric,TBool aUndefine )
    {
    CPIXLOGSTRING("START CCPixIndexerUtils::UnMountAllDrivesL");
    TInt driveNumber;
    for (driveNumber=EDriveA; driveNumber<=EDriveZ;driveNumber++)
       {
        // Check if already exists
        if (!iIndexer[driveNumber])
            continue;
        
        // Form the baseappclass for this media
        TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
        FormBaseAppClass(TDriveNumber(driveNumber), aBaseAppClassGeneric,baseAppClass);             
            
        // Delete the index object
        if (iIndexer[driveNumber])
            {
            delete iIndexer[driveNumber];
            iIndexer[driveNumber] = NULL;
            }
        
        // if the aActionType is EFFMmcDismount, then the
        // parameter aFilename is the baseAppClass of the Index database
        // to be dropped.
        if (aUndefine)
            iSearchSession.UnDefineVolume(baseAppClass);
       }
    iMountedDrives.Reset();//Reset all available
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::DatabasePathLC()
// -----------------------------------------------------------------------------
//
HBufC* CCPixIndexerUtils::DatabasePathLC(TDriveNumber aMedia,const TDesC& aPath)
    {
    CPIXLOGSTRING("START CCPixIndexerUtils::DatabasePathLC");
    // Allocate extra space for root path e.g. "C:\\Private\\2001f6f7\\"
    const TInt KRootPathMaxLength = 30;
    HBufC* indexDbPath = HBufC::NewLC(KRootPathMaxLength + KPathIndexDbPath().Length() + aPath.Length());
    TPtr indexDbPathPtr = indexDbPath->Des();

#if 1 // Data caging implementation
    iFs.CreatePrivatePath(aMedia);

    TChar chr;
    RFs::DriveToChar(aMedia, chr);
    indexDbPathPtr.Append(chr);
    indexDbPathPtr.Append(KColon);

    TFileName pathWithoutDrive;
    iFs.PrivatePath(pathWithoutDrive);
    indexDbPathPtr.Append(KCPixSearchServerPrivateDirectory);
#else // here is the way to calculate the path if data caging is not being used.
    TFileName rootPath;
    PathInfo::GetRootPath(rootPath, aMedia);
    indexDbPathPtr.Append(rootPath);
#endif 

    indexDbPathPtr.Append(KPathIndexDbPath);
    indexDbPathPtr.Append(aPath);

    return indexDbPath;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::MountDriveL()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::MountDriveL(TDriveNumber aMedia,
                                    const TDesC& aBaseAppClassGeneric,
                                    const TDesC& aPath)
    {
    CPIXLOGSTRING("START CCPixIndexerUtils::MountL");
    // Check if already exists
    if (iIndexer[aMedia])
        {
        CPIXLOGSTRING2("CCPixIndexerUtils::MountL Drive %d already Mounted",aMedia);
        return;
        }
    if(IsDriveCanbeMountedL(aMedia))
        {
        //clean the existing database if any
        RemoveUnmountedDatabaseL(aMedia,aPath);
        // Form the baseappclass for this media
        TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
        FormBaseAppClass(TDriveNumber(aMedia), aBaseAppClassGeneric,baseAppClass);
    
        // Define this volume
        HBufC* path = DatabasePathLC(TDriveNumber(aMedia),aPath);
        User::LeaveIfError(iSearchSession.DefineVolume(baseAppClass, *path));
        CleanupStack::PopAndDestroy(path);
        
        // construct and open the database
        TRAPD(err,iIndexer[aMedia] = CCPixIndexer::NewL(iSearchSession));
        CPIXLOGSTRING2("CCPixIndexer::NewL returned : %d", err );
        TRAP(err,iIndexer[aMedia]->OpenDatabaseL(baseAppClass));
        iMountedDrives.AppendL(TDriveNumber(aMedia));
        CPIXLOGSTRING2("CCPixIndexerUtils::MountDriveL Drive %d Mounted",aMedia);
        CPIXLOGSTRING("END CCPixIndexerUtils::MountL");
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::UnMountDriveL()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::UnMountDriveL(TDriveNumber aMedia,
                                      const TDesC& aBaseAppClassGeneric,
                                      TBool aUndefine )
    {
    CPIXLOGSTRING("START CCPixIndexerUtils::UnMount");
    // Check if already exists
    if (!iIndexer[aMedia])
        return;
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(aMedia, aBaseAppClassGeneric,baseAppClass);             
        
    // Remove from harvesting queue
    //iObserver->RemoveHarvestingQueue(this, baseAppClass);   
    
    // Delete the index object
    if (iIndexer[aMedia])
        {
        delete iIndexer[aMedia];
        iIndexer[aMedia] = NULL;
        }
    
    // if the aActionType is EFFMmcDismount, then the
    // parameter aFilename is the baseAppClass of the Index database
    // to be dropped.
    if (aUndefine)
        iSearchSession.UnDefineVolume(baseAppClass);
    //Remove from Mounted List
    RemoveUnmountedDrive(aMedia);
    CPIXLOGSTRING("END CCPixIndexerUtils::UnMount");
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::RemoveUnmountedDrive()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::RemoveUnmountedDrive(TDriveNumber aMedia)
    {
    for(TInt i=0;i<iMountedDrives.Count();i++)
        {
        TDriveNumber drive = iMountedDrives[i];
        if(drive == aMedia)
            {
            iMountedDrives.Remove(i);
            break;
            }
        }
    return;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::IsMediaRemovableL()
// -----------------------------------------------------------------------------
//
TBool CCPixIndexerUtils::IsMediaRemovableL(const TDriveNumber& aDrive)
    {
    TDriveInfo driveInfo; 
    User::LeaveIfError(iFs.Drive(driveInfo,aDrive));
    TBool mediaRemovable(EFalse);
    driveInfo.iDriveAtt & KDriveAttRemovable ? mediaRemovable = ETrue : mediaRemovable = EFalse;
    return mediaRemovable;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::IsDriveCanbeMountedL()
// -----------------------------------------------------------------------------
//
TBool CCPixIndexerUtils::IsDriveCanbeMountedL(TDriveNumber aDrive)
    {
    TBool driveCanBeMount(ETrue);
    TUint drvStatus( 0 );
    TInt err = DriveInfo::GetDriveStatus(iFs, TDriveNumber(aDrive), drvStatus);
    if ( err != KErrNone )
        {
        return EFalse;
        }
    // Harvest drive that are not ROM and Uservisible and Present
    if (!( drvStatus & DriveInfo::EDriveRom ) && // NOT ROM Drive
         ( drvStatus & DriveInfo::EDriveUserVisible ) &&
         ( drvStatus & DriveInfo::EDrivePresent ) )
        {
        driveCanBeMount = ETrue;
        }
    else
        {
        driveCanBeMount = EFalse;
        }
    return driveCanBeMount;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetIndexerFromDrive()
// -----------------------------------------------------------------------------
//
CCPixIndexer* CCPixIndexerUtils::GetIndexerFromDrive(TDriveNumber aMedia)
    {
    if(iIndexer[aMedia])
        {
        return iIndexer[aMedia];
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetIndexerFromMediaId()
// -----------------------------------------------------------------------------
//
CCPixIndexer* CCPixIndexerUtils::GetIndexerFromMediaId(TUint aUniqueID)
    {
    TDriveNumber drive;
    GetDriveFromMediaId(aUniqueID,drive);
    if(iFs.IsValidDrive(drive)) //If in case drive removed
        {
        return iIndexer[drive];
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::IsAlreadyAvilable()
// -----------------------------------------------------------------------------
//
TBool CCPixIndexerUtils::IsAlreadyAvilable(TDriveNumber aMedia)
    {
    TBool bAlready(EFalse);
    for(TInt i=0;i<iMountedDrives.Count();i++)
        {
        TDriveNumber drive = iMountedDrives[i];
        if(drive == aMedia)
            {
            bAlready = ETrue;
            break;
            }
        }
    return bAlready;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ResetAllL()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::ResetAllL()
    {
    for(TInt i=0;i<iMountedDrives.Count();i++)
        {
        TDriveNumber drive = iMountedDrives[i];
        if(iIndexer[drive])
            {
            iIndexer[drive]->ResetL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetMountedDriveList()
// -----------------------------------------------------------------------------
//
RArray<TDriveNumber>& CCPixIndexerUtils::CCPixIndexerUtils::GetMountedDriveList()
    {
    return iMountedDrives;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::RemoveUnmountedDatabase()
// -----------------------------------------------------------------------------
//
void CCPixIndexerUtils::RemoveUnmountedDatabaseL(TDriveNumber aMedia, const TDesC& aPath)
    {
    RFs aFs;
    User::LeaveIfError( aFs.Connect() );
    TChar drive;
    TInt err = aFs.DriveToChar((TDriveNumber)aMedia,drive);
    if ( err == KErrNone )
        {
        TBuf<KMaxFileName> folderpath;
        folderpath.Append(drive);
        folderpath.Append(KColon);
        folderpath.Append(KCPixSearchServerPrivateDirectory);
        folderpath.Append(KIndexingDBPath);
        folderpath.Append(aPath);
        CFileMan* FileMan = CFileMan::NewL(aFs);
        if ( FileMan )
            FileMan->Delete( folderpath );
        delete FileMan;
        }
    aFs.Close();
    }

//End of file
