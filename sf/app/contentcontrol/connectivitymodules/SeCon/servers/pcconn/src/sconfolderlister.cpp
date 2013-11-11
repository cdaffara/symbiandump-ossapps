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
* Description:  CSconFolderLister implementation
*
*/


#include "sconfolderlister.h"

#include <driveinfo.h>
#include <centralrepository.h>
#include <sysutildomaincrkeys.h>
#include <CDirectoryLocalizer.h>
#include <stringresourcereader.h>
#include <sconftp.rsg>
#include <bautils.h>

#include "debug.h"

const TUint16 KFormatVersion( 1 );

// Obex package size
const TInt KPackageSize = 65536;

const TInt KMaxLevelsToSearch = 80;

_LIT( KSConResourceName, "z:\\Resource\\sconftp.rsc" );

CSconFolderEntry* CSconFolderEntry::NewLC()
    {
    CSconFolderEntry* self = new (ELeave) CSconFolderEntry();
    CleanupStack::PushL( self );
    return self;
    }
CSconFolderEntry* CSconFolderEntry::NewLC( const TEntry& aEntry )
    {
    CSconFolderEntry* self = new (ELeave) CSconFolderEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    return self;
    }
CSconFolderEntry::CSconFolderEntry()
    {
    }
CSconFolderEntry::~CSconFolderEntry()
    {
    delete iName;
    delete iLabel;
    }


void CSconFolderEntry::ConstructL( const TEntry& aEntry )
    {
    iAtt = aEntry.iAtt;
    iModified = aEntry.iModified.Int64();
    iName = aEntry.iName.AllocL();
    }

CSconFolderLister::CSconFolderLister( RFs& aFs ) : iFs(aFs)
    {
    // No implementation required
    }

CSconFolderLister::~CSconFolderLister()
    {
    TRACE_FUNC;
    delete iLocalizer;
    }

CSconFolderLister* CSconFolderLister::NewL( RFs& aFs )
    {
    TRACE_FUNC_ENTRY;
    CSconFolderLister* self = new (ELeave) CSconFolderLister( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

void CSconFolderLister::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    iLocalizer = CDirectoryLocalizer::NewL();
    
    // set free memory up to critical level for all drives
    CRepository* repository = CRepository::NewLC( KCRUidDiskLevel );
    TInt iCriticalLevel(0);
    User::LeaveIfError( repository->Get( KDiskCriticalThreshold, iCriticalLevel ) );
    CleanupStack::PopAndDestroy( repository );
    iCriticalLevel += KPackageSize; // add obex package size to critical level
    TRACE_FUNC_EXIT;
    }


void CSconFolderLister::GenerateFolderListL( RBufWriteStream& aStream, const TDesC& aStartPath, 
        const TInt aLevelsToSearch )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aStartPath: %S", &aStartPath );
    LOGGER_WRITE_1("aLevelsToSearch: %d", aLevelsToSearch );
    TInt levelsToSearch = aLevelsToSearch;
    if ( levelsToSearch > KMaxLevelsToSearch || levelsToSearch == KErrNotFound )
        levelsToSearch = KMaxLevelsToSearch;
    iFolders = 0;
    iFiles = 0;
    const TInt KDriveNameLength( 3 );
    
    if ( aStartPath.CompareF( _L("\\") ) == 0 )
        {
        LOGGER_WRITE("List all drives");
        aStream.WriteUint16L( KFormatVersion );
        ListAllDrivesL( aStream, levelsToSearch );
        }
    else if ( aStartPath.Length() >= KDriveNameLength )
        {
        if ( aStartPath.Length() > KDriveNameLength )
            {
            // check root path permissions, is listing allowed
            TPtrC path = aStartPath.Mid(KDriveNameLength);
            LOGGER_WRITE_1("pathFromRoot: %S", &path);
            TInt pathEndMark = path.Locate(TChar('\\'));
            if ( pathEndMark == KErrNotFound )
                {
                User::Leave( KErrArgument );
                }
            //"private\" locate = 7"
            TPtrC pathName = aStartPath.Left( KDriveNameLength+pathEndMark );
            LOGGER_WRITE_1("pathName: %S", &path);
            TEntry pathEntry;
            User::LeaveIfError( iFs.Entry(pathName, pathEntry) );
            
            TBool printable = IsDirectoryPrintable( aStartPath.Left(3), pathEntry );
            if ( !printable )
                {
                // access denied
                LOGGER_WRITE("Access denied!");
                User::Leave( KErrAccessDenied );
                }
            User::LeaveIfError( iFs.Entry(aStartPath, pathEntry) );
            if ( pathEntry.IsSystem() )
                {
                // access denied
                LOGGER_WRITE("System folder, Access denied!");
                User::Leave( KErrAccessDenied );
                }
            }
        
        
        aStream.WriteUint16L( KFormatVersion );
        
        TInt drive;
        User::LeaveIfError( iFs.CharToDrive( aStartPath[0], drive) );
        ListDriveAndPathL( aStream, drive, aStartPath, levelsToSearch);

        }
    else
        {
        LOGGER_WRITE("aStartPath KErrArgument");
        User::Leave( KErrArgument );
        }
    
    
    LOGGER_WRITE_1("iFolders: %d", iFolders);
    LOGGER_WRITE_1("iFiles: %d", iFiles);
    TRACE_FUNC_EXIT;
    }

void CSconFolderLister::ListAllDrivesL( RBufWriteStream& aStream, const TInt aLevelsToSearch )
    {
    TRACE_FUNC_ENTRY;
    TInt devDriveCount = 0;
    TInt mmcDriveCount = 0;
    TChar driveLetter;
    TBuf8<1> driveBuf;
    RArray<TSconDriveInfo>  sconDriveInfo(5);
    
    // Write all drives to folderlisting object
    TDriveList driveList;
    // Get all drives that are visible to the user.
    TInt driveCount;
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount ) );
    
    for( TInt i = EDriveA; i < KMaxDrives; i++ )
        {
        if( driveList[i] )
            {
            TUint driveStatus;
            User::LeaveIfError( DriveInfo::GetDriveStatus( iFs, i, driveStatus ) );
            //LOGGER_WRITE_1( "DriveInfo for drive: %d", i);

            if( !(driveStatus & DriveInfo::EDrivePresent )
                || driveStatus & DriveInfo::EDriveCorrupt
                || (driveStatus & DriveInfo::EDriveRemote) )
                {
                //LOGGER_WRITE_1( "skip drive %d", i);
                continue;
                }
            
            User::LeaveIfError( iFs.DriveToChar( i, driveLetter ) );
            //Letter to uppercase form.
            driveLetter.UpperCase();
            
            TSconDriveInfo info;
            info.iDriveStatus = driveStatus;
            info.iDriveLetter.Zero();
            info.iDriveLetter.Append( driveLetter );
            info.iDriveLetter.Append( _L(":") );
            
            
            info.iDriveAttr = KEntryAttNormal; // ReadWrite
            
            if( driveStatus & DriveInfo::EDriveInternal && devDriveCount==0 )
                {
                //permission always R for first internal drive
                info.iDriveAttr = KEntryAttReadOnly;
                }
            
            //memory type
            if( driveStatus & DriveInfo::EDriveInternal )
                {
                LOGGER_WRITE( "DriveInfo::EDriveInternal" );
                devDriveCount++;
                info.iDriveType = DriveInfo::EDriveInternal; // =1
                info.iDriveTypeCount = devDriveCount;
                
                }
            else if( driveStatus & DriveInfo::EDriveRemovable )
                {
                LOGGER_WRITE( "DriveInfo::EDriveRemovable" );
                mmcDriveCount++;
                info.iDriveType = DriveInfo::EDriveRemovable;; // =2
                info.iDriveTypeCount = mmcDriveCount;
                }
            
            TVolumeInfo volumeInfo;
            User::LeaveIfError( iFs.Volume( volumeInfo, i) );
            
            info.iVolumeName = volumeInfo.iName;
            if ( volumeInfo.iFree > iCriticalLevel )
                {
                volumeInfo.iFree = volumeInfo.iFree - iCriticalLevel;
                }       
            else
                {
                volumeInfo.iFree = 0;
                }
            
            info.iFree = volumeInfo.iFree;
            info.iSize = volumeInfo.iSize;
            
            
            sconDriveInfo.AppendL( info );
            
            }
        }
    
    LOGGER_WRITE_1("Drives: %d", sconDriveInfo.Count());
    aStream.WriteUint8L( sconDriveInfo.Count() );
    for ( TInt i=0; i < sconDriveInfo.Count(); i++ )
        {
        
        TBuf<3> path(KNullDesC);
        path.Copy(sconDriveInfo[i].iDriveLetter);
        path.Append(_L("\\"));
        
        ExportDriveL( aStream, sconDriveInfo[i], path, aLevelsToSearch);
        }
    sconDriveInfo.Close();
    TRACE_FUNC_EXIT;
    }

void CSconFolderLister::ListDriveAndPathL( RBufWriteStream& aStream, TInt aDrive, const TDesC& aStartPath, const TInt aLevelsToSearch )
    {
    TRACE_FUNC_ENTRY;
    TVolumeInfo volumeInfo;
    User::LeaveIfError( iFs.Volume(volumeInfo, aDrive) );
    TInt driveCount(0);
    TUint driveStatus;
    User::LeaveIfError( DriveInfo::GetDriveStatus( iFs, aDrive, driveStatus ) );
    GetDriveTypeNumberL( aDrive, driveStatus, driveCount );
    
    
    TSconDriveInfo info;
    info.iDriveLetter.Copy( aStartPath.Left(2) );
    info.iVolumeName = volumeInfo.iName;
    
    info.iDriveAttr = KEntryAttNormal; // ReadWrite   
    if( driveStatus & DriveInfo::EDriveInternal && driveCount==0 )
        {
        //permission always R for first internal drive
        info.iDriveAttr = KEntryAttReadOnly;
        }
    info.iDriveType = 0;
    if( driveStatus & DriveInfo::EDriveInternal )
        {
        info.iDriveType = DriveInfo::EDriveInternal; // =1
        }
    else if( driveStatus & DriveInfo::EDriveRemovable )
        {
        info.iDriveType = DriveInfo::EDriveRemovable;; // =2
        }
    
    if ( volumeInfo.iFree > iCriticalLevel )
        {
        volumeInfo.iFree = volumeInfo.iFree - iCriticalLevel;
        }       
    else
        {
        volumeInfo.iFree = 0;
        }
    
    info.iFree = volumeInfo.iFree;
    info.iSize = volumeInfo.iSize;
    
    aStream.WriteUint8L( 1 );
    ExportDriveL( aStream, info, aStartPath, aLevelsToSearch);
    
    TRACE_FUNC_EXIT;
    }

void CSconFolderLister::ExportDriveL( RBufWriteStream& aStream, const TSconDriveInfo& aDriveInfo,
        const TDesC& aPathName, const TInt aLevelsToSearch )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("Drive: %S", &aDriveInfo.iDriveLetter);
    LOGGER_WRITE_1("aLevelsToSearch: %d", aLevelsToSearch );
    
    CSconFolderEntry* driveEntry = CSconFolderEntry::NewLC();
    driveEntry->iName = aDriveInfo.iDriveLetter.AllocL();
    
    if ( driveEntry->iLabel )
        {
        LOGGER_WRITE_1("Label: %S", &driveEntry->iLabel->Des());
        }
    driveEntry->iAtt = aDriveInfo.iDriveAttr;

    driveEntry->iDriveInfoEntryExists = ETrue;
    driveEntry->iDriveInfoEntry.iDriveType = aDriveInfo.iDriveType;
    driveEntry->iDriveInfoEntry.iDriveTypeCount = aDriveInfo.iDriveTypeCount;
             
    driveEntry->iDriveInfoEntry.iFree = aDriveInfo.iFree;
    driveEntry->iDriveInfoEntry.iSize = aDriveInfo.iSize;
    
    driveEntry->iLabel = aDriveInfo.iVolumeName.AllocL();
    GetLocalizedVolumeNameL( *driveEntry );
    
    ExportPathL( aStream, aPathName, *driveEntry, ETrue, aLevelsToSearch );
    CleanupStack::PopAndDestroy( driveEntry );
    TRACE_FUNC_EXIT;
    }

void CSconFolderLister::GetLocalizedVolumeNameL( CSconFolderEntry& aDriveEntry )
    {
    TRACE_FUNC_ENTRY;
    // Get localized names
    if ( aDriveEntry.iDriveInfoEntry.iDriveType == DriveInfo::EDriveInternal 
            && aDriveEntry.iDriveInfoEntry.iDriveTypeCount > 1 )
        {
        LOGGER_WRITE("Internal mass memory");
        // internal second memory (Mass memory)
        TFileName file( KSConResourceName );
        BaflUtils::NearestLanguageFile( iFs, file );
        
        CStringResourceReader* reader = CStringResourceReader::NewL( file );
        if ( aDriveEntry.iLabel )
            {
            delete aDriveEntry.iLabel;
            aDriveEntry.iLabel = NULL;
            }
        aDriveEntry.iLabel = reader->ReadResourceString( R_SECON_VALUE_MASS_STORAGE ).AllocL();
        delete reader;
        }
    else if ( aDriveEntry.iLabel && aDriveEntry.iLabel->Length()>0 )
        {
        LOGGER_WRITE("Use normal volume label");
        
        }
    else
        {
        TFileName file( KSConResourceName );
        BaflUtils::NearestLanguageFile( iFs, file );
        CStringResourceReader* reader = CStringResourceReader::NewL( file );
        if (aDriveEntry.iLabel)
            {
            delete aDriveEntry.iLabel;
            aDriveEntry.iLabel = NULL;
            }
        if ( aDriveEntry.iDriveInfoEntry.iDriveType == DriveInfo::EDriveRemovable )
            {
            LOGGER_WRITE("EDriveRemovable");
            // read default MMC name
            aDriveEntry.iLabel = reader->ReadResourceString( R_SECON_VALUE_MMC ).AllocL();
            }
        else
            {
            LOGGER_WRITE("EDriveInternal");
            // read default DEV name
            aDriveEntry.iLabel = reader->ReadResourceString( R_SECON_VALUE_DEVICE ).AllocL();
            }
        delete reader;
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSconFolderLister::GetDriveTypeNumberL()
// 
// -----------------------------------------------------------------------------
//  
void CSconFolderLister::GetDriveTypeNumberL( TInt aDrive, TUint aDriveStatus , TInt& aTypeNumber)
    {
    TRACE_FUNC_ENTRY;
    aTypeNumber = 0;
    
    //TUint driveStatus;
    //
    // if true, search internal drives, else search removable drives
    TBool searchInternalDrives = (aDriveStatus & DriveInfo::EDriveInternal);
    
    TInt driveCount;
    TDriveList driveList;
    
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount ) );
    
    for( TInt i = EDriveA; i <= aDrive; i++ )
        {
        if( driveList[i] )
            {
            TUint driveStatus;
            User::LeaveIfError( DriveInfo::GetDriveStatus( iFs, i, driveStatus ) );

            if( !(driveStatus & DriveInfo::EDrivePresent )
                || driveStatus & DriveInfo::EDriveCorrupt )
                {
                LOGGER_WRITE( "not present or corrupted" );
                continue;
                }
            
            if( driveStatus & DriveInfo::EDriveInternal )
                {
                if( searchInternalDrives )
                    {
                    aTypeNumber++;
                    }
                }
            else if( driveStatus & DriveInfo::EDriveRemovable )
                {
                if( !searchInternalDrives )
                    {
                    aTypeNumber++;
                    }
                }
            }
        }
    TRACE_FUNC_EXIT;
    }


void CSconFolderLister::ExportPathL( RBufWriteStream& aStream, const TDesC& aPathName,
        const CSconFolderEntry& aEntry, const TBool aLocalize, const TInt aLevelsToSearch )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aPathName: %S", &aPathName );
    LOGGER_WRITE_1("aLevelsToSearch: %d", aLevelsToSearch );
    
    LOGGER_WRITE_1("aEntry.iName: %S", &aEntry.iName->Des() );
    
    TBool localized(EFalse); 
    
    // foldername length + name
    aStream.WriteUint8L( aEntry.iName->Length() );
    aStream.WriteL( aEntry.iName->Des() );
    
    TUint att = aEntry.iAtt;
    
    if ( aEntry.iLabel && aEntry.iLabel->Length() > 0 )
        {
        aStream.WriteUint8L( aEntry.iLabel->Length() );
        aStream.WriteL( aEntry.iLabel->Des() );
        }
    else
        {
        if ( aLocalize )
            {
            // initialize Localizer
            iLocalizer->SetFullPath( aPathName );
            localized = iLocalizer->IsLocalized();
            }
        
        if( localized )
            {
            const TDesC& localizedName = iLocalizer->LocalizedName();
            aStream.WriteUint8L( localizedName.Length() );
            aStream.WriteL( localizedName );
            LOGGER_WRITE_1("LocalizedName: %S", &localizedName );
            }
        else if ( aPathName.CompareF(_L("C:\\Data\\")) == 0 )
            {
            TFileName file( KSConResourceName );
            
            BaflUtils::NearestLanguageFile( iFs, file );
               
            CStringResourceReader* reader = CStringResourceReader::NewL( file );
            CleanupStack::PushL( reader );
            
            const TDesC& localizedName = reader->ReadResourceString( R_SECON_DATA_FOLDER );
            aStream.WriteUint8L( localizedName.Length() );
            aStream.WriteL( localizedName );
            LOGGER_WRITE_1("LocalizedName: %S", &localizedName );
            
            CleanupStack::PopAndDestroy( reader );
            localized = ETrue;
            }
        else
            {
            aStream.WriteUint8L( 0 );
            }
        // attributes
        if ( localized )
            {
            // localized folder, set readonly flag
            att = att | KEntryAttReadOnly;
            }
        }
    
    if ( aPathName.Length() == 3 && aLocalize )
        {
        // enable subfolder localization, even when root cannot be localized
        localized = ETrue;
        }
    
    // DriveInfoEntry
    if ( aEntry.iDriveInfoEntryExists )
        {
        aStream.WriteUint8L( 1 );
        aStream.WriteUint8L( aEntry.iDriveInfoEntry.iDriveType );
        aStream.WriteUint8L( aEntry.iDriveInfoEntry.iDriveTypeCount );
        aStream << aEntry.iDriveInfoEntry.iFree;
        aStream << aEntry.iDriveInfoEntry.iSize;
        }
    else
        {
        aStream.WriteUint8L( 0 );
        }
    
    
    // modified time
    TDateTime time = aEntry.iModified.DateTime();
    aStream.WriteInt16L( time.Year() );
    aStream.WriteInt8L( time.Month()+1 ); // range 1...12
    aStream.WriteInt8L( time.Day()+1 );
    aStream.WriteInt8L( time.Hour() );
    aStream.WriteInt8L( time.Minute() );
    aStream.WriteInt8L( time.Second() );
    
    // write attribute
    aStream.WriteUint32L( att );
    
    
    if ( aLevelsToSearch == 0 )
        {
        // don't list directory content
        LOGGER_WRITE("Not listing directory content");
        aStream.WriteInt16L( 0 ); // files count
        aStream.WriteInt16L( 0 ); // directory count
        return;
        }
    
    CDir *fileList = 0;
    CDir *dirList = 0;
    
    // show normal and hidden files and folders
    TUint entryAttMask = KEntryAttNormal | KEntryAttHidden;
    
    // get folders and files
    TInt err = iFs.GetDir( aPathName, entryAttMask, ESortByName, 
            fileList, dirList );
    
    if ( err )
        {
        LOGGER_WRITE_1( "ParseFolderListL GetDir returned: %d", err );
        User::Leave( err );
        }
    
    CleanupStack::PushL( dirList );
    CleanupStack::PushL( fileList );
    
    
    LOGGER_WRITE_1("files: %d", fileList->Count());
    aStream.WriteUint16L( fileList->Count() );
    
    // Print files to folder listing object
    for( TInt i = 0; i < fileList->Count(); i++ )
        {
        const TEntry& fileEntry = ( *fileList )[i];
        iFiles++;
        LOGGER_WRITE_1("fileEntry.iName: %S", &fileEntry.iName);
        // filename length + name
        aStream.WriteUint8L( fileEntry.iName.Length() );
        aStream.WriteL( fileEntry.iName );
        
        // modified time
        TDateTime time = fileEntry.iModified.DateTime();
        aStream.WriteInt16L( time.Year() );
        aStream.WriteInt8L( time.Month()+1 ); // range 1...12
        aStream.WriteInt8L( time.Day()+1 );
        aStream.WriteInt8L( time.Hour() );
        aStream.WriteInt8L( time.Minute() );
        aStream.WriteInt8L( time.Second() );
        
        // attributes
        aStream.WriteUint32L( fileEntry.iAtt );
        
        // file size
        aStream.WriteUint32L( fileEntry.iSize );
        }
    CleanupStack::PopAndDestroy( fileList );
    
    
    // calculate ow many directories we are going to list
    TInt directoryCount(0);
    for( TInt i = 0; i < dirList->Count(); i++ )
        {
        if ( IsDirectoryPrintable(aPathName, ( *dirList )[i] ) )
            {
            directoryCount++;
            }
        }
    LOGGER_WRITE_1("directories: %d", directoryCount);
    aStream.WriteUint16L( directoryCount );
    
    HBufC* fullpath = HBufC::NewL( KMaxPath );
    TPtr fullpathPtr = fullpath->Des();
    CleanupStack::PushL( fullpath );
    
    // Print folders to folder listing object
    for( TInt i = 0; i < dirList->Count(); i++ )
        {
        const TEntry& folderEntry = ( *dirList )[i];
        if ( !IsDirectoryPrintable(aPathName, folderEntry) )
            {
            continue;
            }
        iFolders++;
        
        fullpathPtr.Copy( aPathName );
        fullpathPtr.Append( folderEntry.iName );
        fullpathPtr.Append( _L("\\") );
        CSconFolderEntry* subFolderEntry = CSconFolderEntry::NewLC( folderEntry );
        if ( aLevelsToSearch == KErrNotFound )
            {
            ExportPathL( aStream, fullpathPtr, *subFolderEntry, localized, aLevelsToSearch);
            }
        else
            {
            ExportPathL( aStream, fullpathPtr, *subFolderEntry, localized, aLevelsToSearch-1);
            }
        CleanupStack::PopAndDestroy( subFolderEntry );
        }
    CleanupStack::PopAndDestroy( fullpath );
    CleanupStack::PopAndDestroy( dirList );
    
    TRACE_FUNC_EXIT;
    }
TBool CSconFolderLister::IsDirectoryPrintable( const TDesC& aParentPath, const TEntry& aFolderEntry )
    {
    if ( aFolderEntry.IsSystem() )
        {
        LOGGER_WRITE_1("folder '%S' was system folder", &aFolderEntry.iName);
        return EFalse;
        }
    if ( aParentPath.Length() == 3 )
        {
        if ( aParentPath.FindF(_L("C:\\")) == 0 && aFolderEntry.iName.CompareF(_L("data")) != 0 )
            {
            // C root not visible, exept Data folder
            LOGGER_WRITE_1("folder '%S' not visible to user in c-root", &aFolderEntry.iName);
            return EFalse;
            }
        
        if ( aFolderEntry.iName.CompareF(_L("private")) == 0 )
            {
            LOGGER_WRITE_1("folder '%S' not visible to user", &aFolderEntry.iName);
            return EFalse;
            }
        else if ( aFolderEntry.iName.CompareF(_L("sys")) == 0 )
            {
            LOGGER_WRITE_1("folder '%S' not visible to user", &aFolderEntry.iName);
            return EFalse;
            }
        else if ( aFolderEntry.iName.CompareF(_L("system")) == 0 )
            {
            LOGGER_WRITE_1("folder '%S' not visible to user", &aFolderEntry.iName);
            return EFalse;
            }
        else if ( aFolderEntry.iName.CompareF(_L("resource")) == 0 )
            {
            LOGGER_WRITE_1("folder '%S' not visible to user", &aFolderEntry.iName);
            return EFalse;
            }
        }
    return ETrue;
    }

