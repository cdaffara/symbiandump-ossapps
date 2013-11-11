/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Controller implementation
*
*/


// INCLUDE FILES

#include <pathinfo.h>
#include <stringresourcereader.h>
#include <bautils.h>
#include <driveinfo.h>
#include <sconftp.rsg>
#include <centralrepository.h>
#include <sysutildomaincrkeys.h>

#include "debug.h" 
#include "sconftp.h"
#include "sconfshandler.h"
#include "sconinboxhandler.h"
#include "sconconsts.h"

const TInt KPackageSize = 65536;

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CreateCSConFTPL()
// Entry to CSConFTP
// -----------------------------------------------------------------------------
//
EXPORT_C CSConFTP* CreateCSConFTPL()
    {
    TRACE_FUNC;
    return CSConFTP::NewL();
    }

// -----------------------------------------------------------------------------
// CSConFTP::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConFTP* CSConFTP::NewL()
    {
    TRACE_FUNC_ENTRY;
    CSConFTP* self = new (ELeave) CSConFTP();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CSConFTP::~CSConFTP()
// Destructor
// -----------------------------------------------------------------------------
//
CSConFTP::~CSConFTP()
    {
    TRACE_FUNC_ENTRY;
    delete iSConInboxHandler;
    iSConInboxHandler = NULL;
    delete iSConFsHandler;
    iSConFsHandler = NULL;
    delete iBuffer;
    iBuffer = NULL;

    iFs.Delete( iTempFileName );
    iFs.Close();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTP::GetFolderObjectL( CObexBufObject*& aObjectList )
// Gets object that contains folder listing from inbox or file system
// -----------------------------------------------------------------------------
//
TInt CSConFTP::GetFolderObjectL( CObexBufObject*& aObjectList )
    {
    TRACE_FUNC_ENTRY;
    
    TInt ret( KErrNone );
    TInt offset = 0;
    TChar driveLetter;
    TBuf8<KSConDriveNameLength> tmpBuf8;
    
    iBuffer->Reset();
    
    //default folder list - lists all available drives, inbox
    if( iPathName.Length() == 0 )
        {
        TInt devDriveCount = 0;
        TInt mmcDriveCount = 0;
        
        iBuffer->ResizeL( KSConXmlDocBegin().Length() );
        iBuffer->Write( offset, KSConXmlDocBegin );
        offset += KSConXmlDocBegin().Length();
        
        //Write all drivers to folderlisting object
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
                LOGGER_WRITE_1( "DriveInfo for drive: %d", i);

                if( !(driveStatus & DriveInfo::EDrivePresent )
                    || driveStatus & DriveInfo::EDriveCorrupt
                    || (driveStatus & DriveInfo::EDriveRemote) )
                    {
                    LOGGER_WRITE_1( "skip drive %d", i);
                    continue;
                    }
                
                User::LeaveIfError( iFs.DriveToChar( i, driveLetter ) );
                
                //Letter to uppercase form.
                driveLetter.UpperCase();
                
                //Folder begin
                iBuffer->ExpandL( offset, 
                KSConXmlFolderNameBegin().Length() );
                iBuffer->Write( offset, KSConXmlFolderNameBegin );
                offset += KSConXmlFolderNameBegin().Length();
                
                iBuffer->ExpandL( offset, KSConDriveCName().Length() );
                
                tmpBuf8.Delete( 0, tmpBuf8.Length() );
                tmpBuf8.Append( driveLetter );
                tmpBuf8.Append( KDriveDelimiter );
                
                iBuffer->Write( offset, tmpBuf8 );
                offset += KSConDriveNameLength;
                
                //permission
                iBuffer->ExpandL( offset, 
                KSConXmlUserAttributes().Length() );
                iBuffer->Write( offset, KSConXmlUserAttributes );
                offset += KSConXmlUserAttributes().Length();
                if( driveStatus & DriveInfo::EDriveInternal && devDriveCount==0 )
                    {
                    //permission always R for first internal drive
                    iBuffer->ExpandL( offset, 
                    KSConXmlUserEntryReadOnly().Length() );
                    iBuffer->Write( offset, KSConXmlUserEntryReadOnly );
                    offset += KSConXmlUserEntryReadOnly().Length();
                    }
                else
                    {
                    //permission always RW
                    iBuffer->ExpandL( offset, 
                    KSConXmlUserEntryDrive().Length() );
                    iBuffer->Write( offset, KSConXmlUserEntryDrive );
                    offset += KSConXmlUserEntryDrive().Length();
                    }
                
                //memory type
                iBuffer->ExpandL( offset, 
                KSConXmlMemoryType().Length() );
                iBuffer->Write( offset, KSConXmlMemoryType );
                offset += KSConXmlMemoryType().Length();
                if( driveStatus & DriveInfo::EDriveInternal )
                    {
                    LOGGER_WRITE( "DriveInfo::EDriveInternal" );
                    // DEV type
                    devDriveCount++;
                    if( devDriveCount > 1 )
                        {
                        // DEV2
                        TBuf8<KSConMemTypeMaxLength> memType;
                        memType.Copy( KSConMemoryTypeDev );
                        memType.AppendNum(devDriveCount);
                        iBuffer->ExpandL( offset, memType.Length() );
                        iBuffer->Write( offset, memType );
                        offset += memType.Length();
                        }
                    else
                        {
                        // DEV
                        iBuffer->ExpandL( offset, 
                        KSConMemoryTypeDev().Length() );
                        iBuffer->Write( offset, KSConMemoryTypeDev );
                        offset += KSConMemoryTypeDev().Length();
                        }
                    }
                if( driveStatus & DriveInfo::EDriveRemovable )
                    {
                    LOGGER_WRITE( "DriveInfo::EDriveRemovable" );
                    // MMC type
                    mmcDriveCount++;
                    if( mmcDriveCount > 1 )
                        {
                        // MMC2
                        TBuf8<KSConMemTypeMaxLength> memType;
                        memType.Copy( KSConMemoryTypeMmc );
                        memType.AppendNum(mmcDriveCount);
                        iBuffer->ExpandL( offset, memType.Length() );
                        iBuffer->Write( offset, memType );
                        offset += memType.Length();
                        }
                    else
                        {
                        // MMC
                        iBuffer->ExpandL( offset, 
                        KSConMemoryTypeMmc().Length() );
                        iBuffer->Write( offset, KSConMemoryTypeMmc );
                        offset += KSConMemoryTypeMmc().Length();
                        }
                    }
                
                //memory label
                iBuffer->ExpandL( offset, 
                KSConXmlMemoryLabel().Length() );
                iBuffer->Write( offset, KSConXmlMemoryLabel );
                offset += KSConXmlMemoryLabel().Length();
                
                TInt err( KErrNotFound );
                TBuf8<KMaxFileName> volumeName;
                if( driveStatus & DriveInfo::EDriveRemovable || devDriveCount == 1 )
                    {
                    // get Volume name for C-drive(Phone) and E-drive(MMC)
                    err = GetVolumeNameL( i, volumeName );
                    }
                else
                    {
                    // Get localized name for DEV2 (Mass memory)
                    //Read memory string and convert it
                    TFileName file( KSConResourceName );
                    
                    BaflUtils::NearestLanguageFile( iFs, file );
                    
                    CStringResourceReader* reader = CStringResourceReader::NewL( file );
                    
                    CnvUtfConverter::ConvertFromUnicodeToUtf8( volumeName, 
                        reader->ReadResourceString( R_SECON_VALUE_MASS_STORAGE ) );
                    
                    delete reader;
                    err = KErrNone;
                    }
                
                if ( err == KErrNone )
                    {
                    iBuffer->ExpandL( offset, 
                    volumeName.Length() );
                    iBuffer->Write( offset, volumeName );
                    offset += volumeName.Length();
                    }
                
                //Write the end of filelist to folderlisting object
                iBuffer->ExpandL( offset, KSConXmlFileEnd().Length() );
                iBuffer->Write( offset, KSConXmlFileEnd );
                offset += KSConXmlFileEnd().Length();
                }
            }
        //Write the end of folderlist to folderlisting object
        iBuffer->ExpandL( offset, KSConXmlFolderListEnd().Length() );
        iBuffer->Write( offset, KSConXmlFolderListEnd );
        iBuffer->Compress();
        }
    else
        {
        if( IsCurrentVolumeOK() )
            {
            ret = iSConFsHandler->ParseFolderListL( iBuffer, iPathName, iCurrentDriveTypeNumber );
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    
    //Put data to aObjectList
    delete aObjectList;
    aObjectList = NULL;
    
    aObjectList = CObexBufObject::NewL( iBuffer );
    aObjectList->SetTypeL( KSConFolderListType );
    aObjectList->SetLengthL( iBuffer->Size() );
    
    LOGGER_WRITE_1( "CSConFTP::GetFolderObjectL( CObexBufObject*& aObjectList ) : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::GetFileObjectL( CObexFileObject*& aFileObject )
// Gets object that contains a file from file system
// -----------------------------------------------------------------------------
//
TInt CSConFTP::GetFileObjectL( CObexFileObject*& aFileObject )
    {
    TRACE_FUNC_ENTRY;
    
    TInt ret( KErrNone );
    
    if( aFileObject->Name().Length() == 0 )
        {
        return KErrBadName;
        }
    
    // check is current drive ok
    if( iPathName.Length() > 0 )
        {
        TPath path( KSConAllowedPath );
        path.Append( KPathDelimiter );
        if( iPathName.CompareF( path ) != 0 )
            {
            // was normal path (not KSConAllowedPath)
            if( !IsCurrentVolumeOK() )
                {
                return KErrNotFound;
                }
            }
        }
    
    HBufC* nameBuf = HBufC::NewLC( KMaxFileName );
    TPtr namePtr = nameBuf->Des();
    namePtr.Append( aFileObject->Name() );
    
    if( namePtr.LocateReverse( KPathDelimiter ) == namePtr.Length()-1 )
        {
        namePtr.Delete( namePtr.Length()-1, 1 );
        }
    
    ret = iSConFsHandler->GetFileObjectL( aFileObject, iPathName, namePtr );
    
    CleanupStack::PopAndDestroy( nameBuf );
    
    LOGGER_WRITE_1( "CSConFTP::GetFileObjectL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::PutFileObjectInitL( CObexBufObject*& aObject, 
//                                  CBufFlat*& aBuffer )
// Initializes the receiving from the client.
// -----------------------------------------------------------------------------
//
TInt CSConFTP::PutFileObjectInitL( CObexBufObject*& aObject, 
                                    CBufFlat*& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    RFile file;
    
    TBool lowMemory = IsCurrentDiskSpaceBelowCritical();
    
    //if Inbox then save temp file to default drive
    if( iPathName.Length() == 0 )
        {
        //When proprietary profile (PC Suite), doesn't allow to use Inbox
        if( iProfile == EProprietary )
            {
            LOGGER_WRITE( "CSConFTP::PutFileObjectInitL() : iProfile EProprietary" );
            ret = file.Temp( iFs, PathInfo::PhoneMemoryRootPath(), iTempFileName,
            EFileWrite | EFileShareExclusive );
            file.Close();

            if( ret == KErrNone )
                {
                CreateObexBufObjectL( aObject, aBuffer );
                }
            }
        else
            {
            LOGGER_WRITE( "CSConFTP::PutFileObjectInitL() : not EProprietary" );
            if( lowMemory )
                {
                // do not even try to save
                LOGGER_WRITE( "CSConFTP::PutFileObjectInitL() : disk full" );
                ret = KErrNoMemory;
                }
            else
                {
                ret = iSConInboxHandler->CreateInboxAttachmentL( aObject, aBuffer );
                }
            }
        
        return ret;
        }
    else
        {
        LOGGER_WRITE_1("iPathName: %S", &iPathName);
        // check is current drive ok 
        if( !IsCurrentVolumeOK() )
            {
            LOGGER_WRITE( "CSConFTP::PutFileObjectInitL() : drive not visible" );
            ret = KErrNotFound;
            }
        else
            {
            ret = file.Temp( iFs, iPathName, iTempFileName, EFileWrite | 
            EFileShareExclusive );
            }
        }
    
    file.Close();
    
    if( ret == KErrNone )
        {
        CreateObexBufObjectL( aObject, aBuffer );
        
        if( lowMemory )
            {
            // return error. it's up to pccon/ftp plugin howto handle it..
            LOGGER_WRITE( "CSConFTP::PutFileObjectInitL() : low memory" );
            ret = KErrNoMemory;
            }
        }
    
    LOGGER_WRITE_1( "CSConFTP::PutFileObjectInitL( CObexBufObject*& aObject, CBufFlat*& aBuffer ) : returned %d", ret );
    return ret;
    }

void CSConFTP::CreateObexBufObjectL( CObexBufObject*& aObject, CBufFlat*& aBuffer )
    {
    delete aObject;
    aObject = NULL;
    aObject = CObexBufObject::NewL( NULL );
    
    delete aBuffer;
    aBuffer = NULL;
    
    aBuffer = CBufFlat::NewL( KSConBufferSize );
    aBuffer->ResizeL( KSConBufferSize );
    
    TObexFilenameBackedBuffer bufferdetails( *aBuffer, iTempFileName, 
                    CObexBufObject::EDoubleBuffering );
    TRAPD( err, aObject->SetDataBufL( bufferdetails ));
    if ( err == KErrNoMemory )
        {
        LOGGER_WRITE( "KErrNoMemory, Using singe buffer strategy");
        // If fails, use singe buffer strategy to save RAM
        TObexFilenameBackedBuffer lowMemBufferdetails( *aBuffer, iTempFileName, 
                    CObexBufObject::ESingleBuffering );
        aObject->SetDataBufL( lowMemBufferdetails );
        }
    }

// -----------------------------------------------------------------------------
// CSConFTP::PutFileObjectFinalizeL( CObexBufObject*& aObject )
// Stores the relayed file object to inbox or file system.
// -----------------------------------------------------------------------------
//
TInt CSConFTP::PutFileObjectFinalizeL( CObexBufObject*& aObject )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    // if iPathName is not defined trying to save to inbox, no need to call IsCurrentVolumeOK()
    if( iPathName.Length() > 0 && !IsCurrentVolumeOK() )
        {
        // Memory card removed
        LOGGER_WRITE( "CSConFTP::PutFileObjectFinalizeL() : disk err, return KErrNotFound" );
        return KErrNotFound;
        }
    else if ( iPathName.Length() + aObject->Name().Length() > KMaxFileName )
        {
        LOGGER_WRITE( "CSConFTP::PutFileObjectFinalizeL() : Too long filename, return KErrBadName");
        return KErrBadName;
        }
        
    TBool lowMemory = IsCurrentDiskSpaceBelowCritical();
    if ( lowMemory )
        {
        LOGGER_WRITE( "CSConFTP::PutFileObjectFinalizeL() : Low memory, return KErrNoMemory");
        return KErrNoMemory;
        }
    
    HBufC* fullPathBuf = HBufC::NewLC( KMaxFileName );
    TPtr fullPathPtr = fullPathBuf->Des();
    fullPathPtr.Copy( iPathName );
    fullPathPtr.Append( aObject->Name() );
    
    // Save to inbox if path is not set.
    if( iPathName.Length() == 0 )
        {
        LOGGER_WRITE( "CSConFTP::PutFileObjectFinalizeL before SaveObjectToInbox" );
        ret = iSConInboxHandler->SaveObjToInboxL( aObject );
        LOGGER_WRITE_1( "CSConFTP::PutFileObjectFinalize SaveObjectToInbox returned %d", ret );
        }
    else if( iPathName.CompareF( K_C_ROOT ) == 0 )
        {
        // Saving to root of C:\ is not allowed
        ret = KErrAccessDenied;
        }
    //Save to file system
    else
        {
        TTime time = aObject->Time();
    
        // UTC offset   
        time -= User::UTCOffset();      
        
        delete aObject;
        aObject = NULL;
            
        ret = iSConFsHandler->SaveFileObjectL( fullPathPtr, time, 
        iTempFileName );
        }

    CleanupStack::PopAndDestroy( fullPathBuf );
    
    LOGGER_WRITE_1( "CSConFTP::PutFileObjectFinalizeL( CObexBufObject*& aObject ) : returned %d", ret );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::SetPathL( const TPtrC aPath, const TUint8 aFlags )
// Changes the current path. The path can point to inbox or file system
// -----------------------------------------------------------------------------
//
TInt CSConFTP::SetPathL( const TPtrC aPath, const TUint8 aFlags )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aPath: %S", &aPath );
    TInt ret( KErrNone );

    if( ( aPath.Length() > 0 && aFlags != KSConSetPathRoot )  
        || ( iPathName.Length() == KDriveLength  && aFlags == KSConSetPathBack) ) 
        {
        // check is current drive ok
        if( !IsCurrentVolumeOK() )
            {
            LOGGER_WRITE( "CSConFTP::SetPathL() : drive not found" );
            return KErrNotFound;
            }
        }
    
    ret = KErrNotFound;
    
    //path forward, deny access to forbidden directories and drives
    if( aPath.Length() > 0 
        && ( iPathName.Length() + aPath.Length() < KMaxFileName ) 
        && ( aFlags == KSConSetPathForward || aFlags == KSConSetPathDontCreate ) )
        {
        ret = SetPathForwardL( aPath, aFlags );
        }
    //path backward
    else if( aFlags == KSConSetPathBack )
        {
        ret = SetPathBackwardL( aPath );
        }
    //path root
    else if( aPath.Length() == 0 && aFlags == KSConSetPathRoot )
        {
        LOGGER_WRITE( "CSConFTP::SetPathL() : KSConSetPathRoot" );
        if( iPathName.Length() > 0 )
            {
            iPathName.Delete( 0, iPathName.Length() );
            }
        iCurrentDrive = KErrNotFound;
        ret = KErrNone;
        }
    
    if( ret == KErrNone )
        {
        // drive may have changed, update info
        UpdateDriveTypeInfoL();
        }
    
    LOGGER_WRITE_1( "CSConFTP::CSConFTP::SetPathL( const TPtrC aPath, const TUint8 aFlags ) : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::SetPathForwardL( const TPtrC aPath, const TUint8 aFlags )
// Set path forward
// -----------------------------------------------------------------------------
//
TInt CSConFTP::SetPathForwardL( const TPtrC aPath, const TUint8 aFlags )
    {
    TRACE_FUNC_ENTRY;
    TInt ret(KErrNone);
    HBufC* fullPath = HBufC::NewLC( KMaxFileName );
    TPtr fullPathPtr = fullPath->Des();
    
    if( iPathName.Length() == 0 )
        {
        fullPathPtr.Copy( aPath );   
        }
    else
        {
        fullPathPtr.Copy( iPathName );
        fullPathPtr.Append( aPath );
        }
    LOGGER_WRITE_1( "fullPathPtr: %S", &fullPathPtr );
    // now fullpath is new path
    // check validity
    if( fullPathPtr.Length() < 2 || fullPathPtr[1] != KDriveDelimiter )
        {
        LOGGER_WRITE( "CSConFTP::SetPathForwardL() : not valid name" );
        CleanupStack::PopAndDestroy( fullPath );
        return KErrNotFound;
        }
    
    if( fullPathPtr.CompareF( KSConAllowedPath ) == 0 )
        {
        // Always access to PC Suite configuration file on ROM
        LOGGER_WRITE( "CSConFTP::SetPathForwardL() : KSConAllowedPath" );
        iPathName.Copy( fullPathPtr );
        if( iPathName[iPathName.Length()-1] != KPathDelimiter )
            {
            iPathName.Append( KPathDelimiter );
            }
        
        CleanupStack::PopAndDestroy( fullPath );
        return KErrNone;
        }
    
    // check drive
    TInt driveNumber;
    TInt err = iFs.CharToDrive( fullPathPtr[0], driveNumber );
    if( err != KErrNone || !IsDriveVisible( driveNumber ) )
        {
        LOGGER_WRITE( "CSConFTP::SetPathForwardL() : drive err" );
        CleanupStack::PopAndDestroy( fullPath );
        return KErrNotFound;
        }
    
    // check folder and create it if needed
    if( fullPathPtr.Length() > 2 )
        {
        TBool isFolder = EFalse;
        err = BaflUtils::IsFolder( iFs, fullPathPtr, isFolder );
        if( err != KErrNone )
            {
            isFolder = EFalse;
            }
        LOGGER_WRITE_1( "CSConFTP::SetPathForwardL() IsFolder: %d", (TInt)isFolder );
        
        if( !isFolder )
            {
            //if "Don't create" is 1
            if( aFlags & 2 )
                {
                ret = KErrNotFound;
                }
            else
                {
                CreateFolderL( fullPathPtr );
                ret = KErrNone;
                }
            }
        else
            {
            ret = KErrNone;
            }
        }
    else
        {
        ret = KErrNone;
        }
    
    if( ret == KErrNone )
        {
        iPathName.Copy( fullPathPtr );
        
        if( iPathName[iPathName.Length()-1] != KPathDelimiter )
            {
            iPathName.Append( KPathDelimiter );
            }
        }
    
    CleanupStack::PopAndDestroy( fullPath );
    LOGGER_WRITE_1( "CSConFTP::SetPathForwardL() : end, ret: %d", ret );
    return ret;
    
    }

// -----------------------------------------------------------------------------
// CSConFTP::SetPathBackwardL( const TPtrC aPath )
// Set path backward
// -----------------------------------------------------------------------------
//
TInt CSConFTP::SetPathBackwardL( const TPtrC aPath )
    {
    TRACE_FUNC_ENTRY;
    TInt ret(KErrNotFound);
    if( iPathName.Length() == 0 )
        {
        // already in root, exit
        LOGGER_WRITE( "CSConFTP::SetPathBackwardL() : already in root" );
        LOGGER_WRITE_1( "CSConFTP::SetPathBackwardL() : end, ret: %d", ret );
        return ret;
        }
    TInt pos;
    TBool isFolder(EFalse);
    HBufC* fullPath = HBufC::NewLC( KMaxFileName );
    TPtr fullPathPtr = fullPath->Des();

    //no directory/folder name
    if( aPath.Length() == 0 )
        {
        LOGGER_WRITE( "CSConFTP::SetPathBackwardL() : no new path, goto parent" );
        iPathName.Delete( iPathName.Length()-1, 1 );
        pos = iPathName.LocateReverse( KPathDelimiter );
        iPathName.Delete( pos+1, iPathName.Length()-pos );
        ret = KErrNone;
        }
    //if folder name
    else if( aPath.Locate( KDriveDelimiter ) != 1 && iPathName.Length() > KDriveLength  )
        {
        LOGGER_WRITE( "CSConFTP::SetPathBackwardL() : goto parent and new path" );                
        fullPathPtr.Copy( iPathName );
        fullPathPtr.Delete( fullPathPtr.Length()-1, 1 );
        pos = fullPathPtr.LocateReverse( KPathDelimiter );
        fullPathPtr.Delete( pos+1, fullPathPtr.Length()-pos );
        fullPathPtr.Append( aPath );
        
        if( fullPathPtr.CompareF( KSConAllowedPath ) == 0 )
            {
            // Always access to PC Suite configuration file on ROM
            isFolder = ETrue;
            }
        else
            { 
            TInt driveNumber;
            TInt err = iFs.CharToDrive(fullPathPtr[0], driveNumber);
            if( err != KErrNone )
                {
                LOGGER_WRITE( "CSConFTP::SetPathForwardL() : drive err" );
                isFolder = EFalse;
                }
            else if( !IsDriveVisible(driveNumber) )
                {
                isFolder = EFalse;
                }
            else
                {
                // drive ok, check folder.
                err = BaflUtils::IsFolder( iFs, fullPathPtr, isFolder );
                if( err != KErrNone )
                    {
                    isFolder = EFalse;
                    }
                }
            }

        if( isFolder ) 
            {
            iPathName.Copy( fullPathPtr );

            if( aPath.LocateReverse( KPathDelimiter ) != aPath.Length()-1 )
                {
                iPathName.Append( KPathDelimiter );
                }

            ret = KErrNone;
            }
        else
            {
            ret = KErrNotFound;
            }
        
        }
    //drive
    else if( aPath.Locate( KDriveDelimiter ) == 1 )
        {
        fullPathPtr.Copy( aPath );
        if( fullPathPtr.LocateReverse( KPathDelimiter ) 
                    != fullPathPtr.Length()-1 )
            {
            fullPathPtr.Append( KPathDelimiter );
            }
        
        TFileName tempPath( KSConAllowedPath );
        tempPath.Append(KPathDelimiter);
        if( fullPathPtr.CompareF( tempPath ) == 0 )
            {
            // Always access to PC Suite configuration file on ROM
            isFolder = ETrue;
            }
        else
            {
            TInt driveNumber;
            TInt err = iFs.CharToDrive(fullPathPtr[0], driveNumber);
            if( err != KErrNone )
                {
                LOGGER_WRITE( "CSConFTP::SetPathForwardL() : drive err" );
                isFolder = EFalse;
                }
            else if( !IsDriveVisible(driveNumber) )
                {
                isFolder = EFalse;
                }
            else
                {
                // drive ok, check folder.
                err = BaflUtils::IsFolder( iFs, fullPathPtr, isFolder );
                if( err != KErrNone )
                    {
                    isFolder = EFalse;
                    }
                }
            }
            
        if( isFolder ) 
            {
            iPathName.Copy( fullPathPtr );
            
            if( aPath.LocateReverse( KPathDelimiter ) != aPath.Length()-1 )
                {
                iPathName.Append( KPathDelimiter );
                }
            
            ret = KErrNone;
            }
        else
            {
            ret = KErrNotFound;
            }               
        }
    CleanupStack::PopAndDestroy( fullPath );
    LOGGER_WRITE_1( "CSConFTP::SetPathBackwardL() : end, ret: %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::CreateFolderL( const TPtrC aFolderName )
// Creates a new folder to file system
// -----------------------------------------------------------------------------
//
TInt CSConFTP::CreateFolderL( const TPtrC aFolderName )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFolderName: %S", &aFolderName );
    TInt ret( KErrNone );
    if( aFolderName.Length() == 0 )
        {
        return KErrArgument;
        }
    
    HBufC*  pathBuf = HBufC::NewLC( KMaxFileName );
    TPtr pathBufPtr = pathBuf->Des();
    //absolute folder path
    if( aFolderName.Length() > 1 && aFolderName[1] == KDriveDelimiter )
        {
        if ( iPathName.Length()>0 )
            {
            LOGGER_WRITE( "CSConFTP::CreateFolderL() : KErrBadName" );
            CleanupStack::PopAndDestroy( pathBuf );
            return KErrBadName;
            }
        pathBufPtr.Copy( aFolderName );
        }
    else
        {
        pathBufPtr.Copy( iPathName );
        pathBufPtr.Append( aFolderName );
        }
    
    if( !iFs.IsValidName( pathBufPtr ) )
        {
        ret = KErrBadName;
        }
    else
        {
        // check drive
        TInt driveNumber;
        ret = iFs.CharToDrive(pathBufPtr[0], driveNumber);
        if( ret != KErrNone || !IsDriveVisible(driveNumber) )
            {
            LOGGER_WRITE( "CSConFTP::CreateFolderL() : drive err" );
            ret = KErrNotFound;
            }
        }

    if ( ret == KErrNone )
        {
        if( pathBufPtr.LocateReverse( KPathDelimiter ) != pathBufPtr.Length()-1 )
            {
            pathBufPtr.Append( KPathDelimiter );
            }
        ret = iSConFsHandler->CreateFolderL( pathBufPtr );
        }
    
    if( ret == KErrNone || ret == KErrAlreadyExists )
        {
        ret = SetPathL( aFolderName, KSConSetPathForward );
        }
    
    CleanupStack::PopAndDestroy( pathBuf );
    
    LOGGER_WRITE_1( "CSConFTP::CreateFolderL( const TPtrC aFolderName ) : returned %d", ret );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::DeleteObjectL( const TPtrC aObjectName )
// Deletes file/folder from inbox or file system
// -----------------------------------------------------------------------------
//
TInt CSConFTP::DeleteObjectL( const TPtrC aObjectName )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aObjectName: %S", &aObjectName );
    TInt ret( KErrNone );
    TFileName tmpTarget;
    HBufC* targetBuf = HBufC::NewLC( KMaxFileName );
    TPtr target = targetBuf->Des();
    
    //absolute folder path
    if( aObjectName.Length() > 1 && aObjectName[1] == KDriveDelimiter )
        {
        if ( iPathName.Length()>0 )
            {
            LOGGER_WRITE( "CSConFTP::DeleteObjectL() : KErrBadName" );
            CleanupStack::PopAndDestroy( targetBuf );
            return KErrBadName;
            }
        target.Copy( aObjectName );
        }
    else
        {
        target.Copy( iPathName );
        target.Append( aObjectName );
        }
    
    if( !iFs.IsValidName( target ) )
        {
        ret = KErrBadName;
        }
    else
        {
        TInt driveNumber;
        ret = iFs.CharToDrive(target[0], driveNumber);
        if( ret != KErrNone || !IsDriveVisible(driveNumber) )
            {
            LOGGER_WRITE( "CSConFTP::DeleteObjectL() : drive err" );
            ret = KErrNotFound;
            }
        }
    

    if ( ret == KErrNone )
        {
        ret = iSConFsHandler->DeleteObjectL( target );
        }
    
    CleanupStack::PopAndDestroy( targetBuf );
    
    LOGGER_WRITE_1( "CSConFTP::DeleteObjectL( const TPtrC aObjectName ) : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::GetPath( TDes& aPath )
// Gets the current path
// -----------------------------------------------------------------------------
//
TInt CSConFTP::GetPath( TDes& aPath )
    {
    TRACE_FUNC;
    aPath.Copy( iPathName );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSConFTP::AbortFileTransfer( CObexBufObject*& aObject )
// Abort file transfer
// -----------------------------------------------------------------------------
//
TInt CSConFTP::AbortFileTransfer( CObexBufObject*& aObject )
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNone );
    
    if( iPathName.Length() == 0 )
        {
        iSConInboxHandler->AbortInboxOperation( aObject );
        }
    
    //aObject has to be deleted, otherwise the temp file is locked
    delete aObject;
    aObject = NULL;
    
    err = iFs.Delete( iTempFileName );
    
    LOGGER_WRITE_1( "CSConFTP::AbortFileTransfer( CObexBufObject*& aObject ) : iFs.Delete() err: %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// CSConFTP::SetProfile( TInt aProfile )
// Set used transfer profile
// -----------------------------------------------------------------------------
//
void CSConFTP::SetProfile( TSConProfile aProfile )
    {
    TRACE_FUNC;
    iProfile = aProfile;
    }

// -----------------------------------------------------------------------------
// CSConFTP::SetBackupStarted( TBool aValue )
// Set backup status
// -----------------------------------------------------------------------------
//
void CSConFTP::SetBackupStarted( TBool aValue )
    {
    TRACE_FUNC;
    iBackupStarted = aValue;
    }

// -----------------------------------------------------------------------------
// CSConFTP::SetUsedMedia( TSConUsedMedia aMedia )
// Set the used media information
// -----------------------------------------------------------------------------
//  
void CSConFTP::SetUsedMedia( TSConUsedMedia aMedia )
    {
    TRACE_FUNC;
    iMedia = aMedia;
    }

// -----------------------------------------------------------------------------
// CSConFTP::IsCurrentDiskSpaceBelowCritical( TUint32 aFilesize )
// Check is current disk space below critical level.
// -----------------------------------------------------------------------------
//
TBool CSConFTP::IsCurrentDiskSpaceBelowCritical( TUint32 aFilesize )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFilesize: %d", aFilesize );
    TInt drive;
    TInt err;
    
    if( iCurrentDrive != KErrNotFound )
        {
        drive = iCurrentDrive;
        }
    else
        {
        LOGGER_WRITE( "CSConFTP::IsCurrentDiskSpaceBelowCritical() : drive not specified, use default drive" );
        err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive );
        if( err )
            {
            LOGGER_WRITE_1( "CSConFTP::IsCurrentDiskSpaceBelowCriticaL() : DriveInfo::GetDefaultDrive err %d", err );
            return ETrue; //exit
            }
        }
    
    // check disk space
    LOGGER_WRITE_1( "drive: %d", drive );
    TVolumeInfo volumeInfo;
    err = iFs.Volume(volumeInfo, drive);
    if( err != KErrNone )
        {
        LOGGER_WRITE_1( "CSConFTP::IsCurrentDiskSpaceBelowCritical() : iFs.Volume err %d", err );
        LOGGER_WRITE( "CSConFTP::IsCurrentDiskSpaceBelowCritical() : End" );
        return ETrue; //exit
        }
    LOGGER_WRITE_1( "volumeInfo.iFree: %Ld", volumeInfo.iFree );
    TBool diskLevelCritical( EFalse );
    if ( volumeInfo.iFree - aFilesize <= iCriticalDiskLevel )
        {
        // Can not write the data, there's not enough free space on disk.
        diskLevelCritical = ETrue;
        }

    LOGGER_WRITE_1( "CSConFTP::IsCurrentDiskSpaceBelowCritical() : ret %d",
     (TInt)diskLevelCritical );
    return diskLevelCritical;
    }

// -----------------------------------------------------------------------------
// CSConFTP::ReadWBXMLDataL( CBufFlat*& aBuffer )
// Read received ConML protocol packet
// -----------------------------------------------------------------------------
//  
TInt CSConFTP::ReadWBXMLDataL( CBufFlat*& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    RFile file;
    TInt fileSize;
    
    ret = file.Open( iFs, iTempFileName, EFileRead|EFileShareAny );
    CleanupClosePushL( file );
    if( ret == KErrNone )
        {
        file.Size( fileSize );
        
        delete aBuffer;
        aBuffer = NULL;
            
        HBufC8* wbxmlDataBuf = HBufC8::NewLC( fileSize );
        TPtr8 wbxmlDataPtr = wbxmlDataBuf->Des();
        
        file.Read( wbxmlDataPtr );
        
        aBuffer = CBufFlat::NewL( KSConBufSize );
        aBuffer->ExpandL( 0, fileSize );
        aBuffer->Write( 0, wbxmlDataPtr );
        
        CleanupStack::PopAndDestroy( wbxmlDataBuf );
        }
    CleanupStack::PopAndDestroy( &file );
    LOGGER_WRITE_1( "CSConFTP::ReadWBXMLData( CBufFlat*& aBuffer ) : WBXML packet size: %d", fileSize );
        
    LOGGER_WRITE_1( "CSConFTP::ReadWBXMLData( CBufFlat*& aBuffer ) : returned %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConFTP::DeleteTempFile()
// Delete OBEX stack's temp file
// -----------------------------------------------------------------------------
//  
void CSConFTP::DeleteTempFile()
    {
    TRACE_FUNC_ENTRY;
    iFs.Delete( iTempFileName );
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConFTP::MoveFile()
// Move file/folder
// -----------------------------------------------------------------------------
//
TInt CSConFTP::MoveFile(const TDesC& aSource, const TDesC& aTarget)
    {
    TRACE_FUNC_ENTRY;
    TInt err;
    TFileName tmpSource;
    TFileName tmpTarget;
    err = GetAbsolutePath( aSource, tmpSource );
    if( err == KErrNone )
        {
        err = GetAbsolutePath( aTarget, tmpTarget );
        }
        
    if( err == KErrNone )
        {
        TInt sourceDrive;
        TInt targetDrive;
        
        err = iFs.CharToDrive( tmpSource[0], sourceDrive );
        TInt err2 = iFs.CharToDrive( tmpTarget[0], targetDrive );
        
        if( err != KErrNone || err2 != KErrNone
            || !IsDriveVisible( sourceDrive )
            || !IsDriveVisible( targetDrive ) )
            {
            // drive not visible to user
            err = KErrNotFound;
            }
        }
        
    if( err == KErrNone )
        {
        TRAP( err,
        iSConFsHandler->DoCopyOrMoveFileL( tmpSource, tmpTarget, EFalse ) );
        }
    
    LOGGER_WRITE_1( "CSConFTP::MoveFile() : end, err: %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// CSConFTP::CopyFile()
// Copy file/folder
// -----------------------------------------------------------------------------
//
TInt CSConFTP::CopyFile(const TDesC& aSource, const TDesC& aTarget)
    {
    TRACE_FUNC_ENTRY;
    TInt err;
    TFileName tmpSource;
    TFileName tmpTarget;
    err = GetAbsolutePath( aSource, tmpSource );
    if (err == KErrNone)
        {
        err = GetAbsolutePath( aTarget, tmpTarget );
        }
    
    if( err == KErrNone )
        {
        TInt sourceDrive;
        TInt targetDrive;
        
        err = iFs.CharToDrive( tmpSource[0], sourceDrive );
        TInt err2 = iFs.CharToDrive( tmpTarget[0], targetDrive );
        
        if( err != KErrNone || err2 != KErrNone
            || !IsDriveVisible( sourceDrive )
            || !IsDriveVisible( targetDrive ) )
            {
            // drive not visible to user
            err = KErrNotFound;
            }
        }
        
    if (err == KErrNone)
        {
        TRAP( err,
        iSConFsHandler->DoCopyOrMoveFileL( tmpSource, tmpTarget, ETrue ) );
        }
    
    LOGGER_WRITE_1( "CSConFTP::CopyFile() : end, ret: %d", err );
    return err;
    }
    
// -----------------------------------------------------------------------------
// CSConFTP::SetReadOnly()
// Set read-only permissions to file or folder
// -----------------------------------------------------------------------------
//
TInt CSConFTP::SetReadOnly(const TDesC& aTarget, const TBool aReadOnly)
    {
    TRACE_FUNC_ENTRY;
    TInt err;
    TFileName tmpTarget;
    err = GetAbsolutePath( aTarget, tmpTarget );
    if( err != KErrNone )
        {
        return err;
        }
    else
        {
        TInt targetDrive;
        err = iFs.CharToDrive( tmpTarget[0], targetDrive );
        
        if( err != KErrNone
            || !IsDriveVisible( targetDrive ) )
            {
            // drive not visible to user
            return KErrNotFound;
            }
        }
    
    TBool isFolder(EFalse);
    err = BaflUtils::IsFolder( iFs, tmpTarget, isFolder );
    if ( err == KErrNone && isFolder )
        {
        tmpTarget.Append(KPathDelimiter);
        }
    if ( !iSConFsHandler->IsFolderVisible( tmpTarget ) )
        {
        // folder is not visible to user
        err = KErrAccessDenied;
        }
    else
        {
        if( aReadOnly )
            {
            err = iFs.SetAtt( tmpTarget, KEntryAttReadOnly, KEntryAttArchive );
            }
        else
            {
            err = iFs.SetAtt( tmpTarget, KEntryAttNormal, KEntryAttReadOnly );
            }
        }
        
    LOGGER_WRITE_1( "CSConFTP::SetReadOnly() : ret: %d", err );
    return err;
    }
    
// -----------------------------------------------------------------------------
// CSConFTP::SetHidden()
// Set hidden permissions to file or folder
// -----------------------------------------------------------------------------
//
TInt CSConFTP::SetHidden( const TDesC& /*aTarget*/, const TBool /*aHidden*/ )
    {
	// This is currently not supported.
    LOGGER_WRITE("CSConFTP::SetHidden return KErrNotSupported");
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CSConFTP::GetAbsolutePath()
// Get absolute path from relative file/folder
// -----------------------------------------------------------------------------
//
TInt CSConFTP::GetAbsolutePath( const TDesC& aFolderName, TDes &aFullPath )
    {
    if (aFolderName.Length() == 0)
        {
        return KErrBadName;
        }
    //absolute folder path
    if( aFolderName.Length() > 1 && aFolderName[1] == KDriveDelimiter )
        {
        aFullPath.Copy( aFolderName );
        }
    // relative to the root folder
    else if( aFolderName[0] == KPathDelimiter )
        {
        if( iPathName.Length() < 2 )
            {
            return KErrBadName;
            }
        aFullPath.Copy( iPathName.Left(2) );
        aFullPath.Append( aFolderName );
        }
    // relative to the current folder
    else
        {
        aFullPath.Copy( iPathName );
        if( aFullPath.LocateReverse( KPathDelimiter ) != aFullPath.Length()-1 )
            {
            aFullPath.Append( KPathDelimiter );
            }
        aFullPath.Append( aFolderName );
        // no need to check internal root, because iPathName is real target.
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSConFTP::CSConFTP()
// Default constructor
// -----------------------------------------------------------------------------
//  
CSConFTP::CSConFTP() : iProfile( EStandard ), iCurrentDrive( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// CSConFTP::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//  
void CSConFTP::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    iBuffer = CBufFlat::NewL( KSConBufSize );
    User::LeaveIfError( iFs.Connect() );
    iSConFsHandler = CSConFsHandler::NewL( iFs );
    iSConInboxHandler = CSConInboxHandler::NewL();
    
    CRepository* repository = CRepository::NewLC( KCRUidDiskLevel );
    User::LeaveIfError( repository->Get( KDiskCriticalThreshold, iCriticalDiskLevel ) );
    CleanupStack::PopAndDestroy( repository );
    // inlcude package size
    iCriticalDiskLevel += KPackageSize;
    LOGGER_WRITE_1( "criticalLevel: %d", iCriticalDiskLevel );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFTP::GetVolumeNameL()
// Get volume name
// -----------------------------------------------------------------------------
//  
TInt CSConFTP::GetVolumeNameL(const TInt aDriveNumber, TDes8& aVolumeName)
    {
    TRACE_FUNC_ENTRY;
    TVolumeInfo volumeInfo;
    TInt ret = iFs.Volume( volumeInfo, aDriveNumber );
    
    if ( ret == KErrNone)
        {
        CnvUtfConverter::ConvertFromUnicodeToUtf8( aVolumeName,
             volumeInfo.iName );
        //Replace special characters
        for( TInt i = 0; i < aVolumeName.Length(); i++ )
            {
            switch( aVolumeName[i] )
                {
                case '&':
                    aVolumeName.Delete( i, 1 );
                    aVolumeName.Insert( i, KReplace1 );
                    break;
                case '<':
                    aVolumeName.Delete( i, 1 );
                    aVolumeName.Insert( i, KReplace2 );
                    break;
                case '>':
                    aVolumeName.Delete( i, 1 );
                    aVolumeName.Insert( i, KReplace3 );
                    break;
                case '"':
                    aVolumeName.Delete( i, 1 );
                    aVolumeName.Insert( i, KReplace4 );
                    break;
                case '\'':
                    aVolumeName.Delete( i, 1 );
                    aVolumeName.Insert( i, KReplace5 );
                    break;
                default:
                    break;
                }
            }
        
        //No name
        if( aVolumeName.Length() == 0 )
            {
            LOGGER_WRITE( "Volume has no name, use default localized name" );
            //Read memory string and convert it
            TFileName file( KSConResourceName );
            
            BaflUtils::NearestLanguageFile( iFs, file );
            
            CStringResourceReader* reader = CStringResourceReader::NewL( file );
            CleanupStack::PushL( reader );
            TUint driveStatus;
            User::LeaveIfError( DriveInfo::GetDriveStatus( iFs, aDriveNumber, driveStatus ) );
            if( driveStatus & DriveInfo::EDriveRemovable )
                {
                // read default MMC name
                CnvUtfConverter::ConvertFromUnicodeToUtf8( aVolumeName, 
                reader->ReadResourceString( R_SECON_VALUE_MMC ) );
                }
            else
                {
                // read default DEV name
                CnvUtfConverter::ConvertFromUnicodeToUtf8( aVolumeName, 
                reader->ReadResourceString( R_SECON_VALUE_DEVICE ) );
                }
            CleanupStack::PopAndDestroy( reader );
            }
        }
    LOGGER_WRITE_1("GetVolumeNameL returned: %d", ret);
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFTP::IsDriveVisible()
// Check is drive visible for user
// -----------------------------------------------------------------------------
//  
TBool CSConFTP::IsDriveVisible( const TInt aDriveNumber )
    {
    TUint driveStatus;
    TInt err = DriveInfo::GetDriveStatus( iFs, aDriveNumber, driveStatus);
    if( err )
        {
        LOGGER_WRITE_1( "CSConFTP::IsDriveVisible() : DriveInfo::GetDriveStatus err: %d", err );
        return EFalse;
        }
    if( driveStatus & DriveInfo::EDriveRemote )
        {
        LOGGER_WRITE( "CSConFTP::IsDriveVisible() : remote drive" );
        return EFalse;
        }
    if( !(driveStatus & DriveInfo::EDriveUserVisible) )
        {
        LOGGER_WRITE( "CSConFTP::IsDriveVisible() : not visible" );
        return EFalse;
        }
    if( !(driveStatus & DriveInfo::EDrivePresent ) )
        {
        LOGGER_WRITE( "CSConFTP::IsDriveVisible() : not present" );
        return EFalse;
        }
    if( driveStatus & DriveInfo::EDriveCorrupt )
        {
        LOGGER_WRITE( "CSConFTP::IsDriveVisible() : corrupted" );
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConFTP::UpdateDriveTypeInfoL()
// Test is current volume still ok.
// -----------------------------------------------------------------------------
//  
TBool CSConFTP::IsCurrentVolumeOK()
    {
    if( iCurrentDrive == KErrNotFound )
        {
        LOGGER_WRITE( "CSConFTP::IsCurrentVolumeOK() : not set" );
        return EFalse;
        }
    
    TUint driveStatus;
    TInt err = DriveInfo::GetDriveStatus( iFs, iCurrentDrive, driveStatus);
    if( err )
        {
        LOGGER_WRITE_1( "CSConFTP::IsCurrentVolumeOK() : DriveInfo::GetDriveStatus err: %d", err );
        return EFalse;
        }
    if( !(driveStatus & DriveInfo::EDrivePresent ))
        {
        LOGGER_WRITE( "CSConFTP::IsCurrentVolumeOK() : not present" );
        return EFalse;
        }
    if( driveStatus & DriveInfo::EDriveCorrupt )
        {
        LOGGER_WRITE( "CSConFTP::IsCurrentVolumeOK() : corrupted" );
        return EFalse;
        }   
    else
        {
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CSConFTP::UpdateDriveTypeInfoL()
// Updates drive information (iCurrentDrive and iCurrentDriveTypeNumber)
// -----------------------------------------------------------------------------
//  
void CSConFTP::UpdateDriveTypeInfoL()
    {
    TRACE_FUNC_ENTRY;
    TInt driveNumber;
    iCurrentDriveTypeNumber = 0;
    
    if( iPathName.Length() == 0 )
        {
        LOGGER_WRITE( "CSConFTP::UpdateDriveTypeInfoL() : drive not specified" );
        iCurrentDrive = KErrNotFound;
        return;
        }
    
    User::LeaveIfError( iFs.CharToDrive(iPathName[0], driveNumber) );
    iCurrentDrive = driveNumber;
    
    TUint driveStatus;
    User::LeaveIfError( DriveInfo::GetDriveStatus( iFs, iCurrentDrive, driveStatus ) );
    // if true, search internal drives, else search removable drives
    TBool searchInternalDrives = (driveStatus & DriveInfo::EDriveInternal);
    
    TInt driveCount;
    TDriveList driveList;
    
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount ) );
    
    for( TInt i = EDriveA; i <= iCurrentDrive; i++ )
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
                    iCurrentDriveTypeNumber++;
                    }
                }
            else if( driveStatus & DriveInfo::EDriveRemovable )
                {
                if( !searchInternalDrives )
                    {
                    iCurrentDriveTypeNumber++;
                    }
                }
            }
        }
    TRACE_FUNC_EXIT;
    }
    
// End of file

