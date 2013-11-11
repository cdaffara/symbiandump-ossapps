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
#include <stringresourcereader.h>
#include <sconftp.rsg>
#include <pathinfo.h>
#include <connect/sbdefs.h> // to get backup/restore mode
#include <driveinfo.h>
#include <bautils.h>
#include <sysutil.h>
#include <CDirectoryLocalizer.h>

#include "sconfshandler.h"
#include "sconconsts.h"
#include "irmcconsts.h"
#include "cntparserclient.h"
#include "cntparserinfolog.h"
#include "debug.h"

// cntparser related constants
_LIT(KPhoneBookIrMCL2Request,"telecom/pb.vcf");
_LIT(KInfoLogIrMCL2Request,"telecom/pb/info.log");
_LIT(KInfoLogIrMCL2File,"IrMC\\info.log");
_LIT(KInfoLogPathAndFileName,"c:\\IrMC\\info.log");
_LIT(KPbPathAndFileName,"c:\\IrMC\\pb.vcf");

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSConFsHandler::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConFsHandler* CSConFsHandler::NewL( RFs& aFs )
    {
    TRACE_FUNC_ENTRY;
    CSConFsHandler* self = new (ELeave) CSConFsHandler( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::~CSConFsHandler()
// Destructor
// -----------------------------------------------------------------------------
//
CSConFsHandler::~CSConFsHandler()
    {
    TRACE_FUNC_ENTRY;
    delete iLocalizer;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::ParseFolderListL( CBufFlat* aBuffer, const TPtrC aPathName
//                                  , const TInt aMemNumber )
// Parses folder and file listing from specified directory
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::ParseFolderListL( CBufFlat* aBuffer, const TPtrC aPathName,
                                       const TInt aMemNumber )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "CSConFsHandler::ParseFolderListL aPathName =  %S", &aPathName );
    if( aPathName.Length() == 0 )
        {
        LOGGER_WRITE( "CSConFsHandler::ParseFolderListL( CBufFlat* aBuffer, const TPtrC aPathName ) : No path" );
        return KErrArgument;
        }
            
    if( aPathName.Length() > KDriveLength && !BaflUtils::FolderExists( iFs, aPathName ) )
        {
        LOGGER_WRITE( "CSConFsHandler::ParseFolderListL( CBufFlat* aBuffer, const TPtrC aPathName ) : Current folder doesn't exist any more" );
        return KErrNotFound;
        }
    
    TInt driveNumber;
    TInt err = iFs.CharToDrive(aPathName[0], driveNumber);
    if( err )
        {
        LOGGER_WRITE_1( "CSConFsHandler::ParseFolderListL : CharToDrive err: %d", err );
        return KErrNotFound;
        }
    TUint driveStatus;
    err = DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus);
    if( err )
        {
        LOGGER_WRITE_1( "CSConFsHandler::ParseFolderListL : DriveInfo::GetDriveStatus err: %d", err );
        return KErrNotFound;
        }
    
    // check mem type (DEV, DEV2, MMC, MMC2..)
    TBuf8<KSConMemTypeMaxLength> memType;
    if( driveStatus & DriveInfo::EDriveInternal )
        {
        memType.Copy( KSConMemoryTypeDev );
        if( aMemNumber > 1 )
            {
            memType.AppendNum(aMemNumber);
            }
        }
    else if( driveStatus & DriveInfo::EDriveRemovable )
        {
        memType.Copy( KSConMemoryTypeMmc );
        if( aMemNumber > 1 )
            {
            memType.AppendNum(aMemNumber);
            }
        }

    CDir *entryList = 0;
    CDir *dirList = 0;
    
    // show only normal files and folders
    TUint entryAttMask = KEntryAttNormal;
    
    
    // get folders and files
    err = iFs.GetDir( aPathName, entryAttMask, ESortByName, 
            entryList, dirList );
    
    if ( err )
        {
        LOGGER_WRITE_1( "ParseFolderListL GetDir returned: %d", err );
        return err;
        }
    
    CleanupStack::PushL( entryList );
    CleanupStack::PushL( dirList );
    
    TInt offset = 0;
    
    LOGGER_WRITE( "CSConFsHandler::ParseFolderList XML creation" );
    //Create folder XML
    aBuffer->ResizeL( KSConXmlDocBegin().Length() );
    aBuffer->Write( offset, KSConXmlDocBegin );
    offset += KSConXmlDocBegin().Length();

    aBuffer->ExpandL( offset, KSConXmlParentFolder().Length() );
    aBuffer->Write( offset, KSConXmlParentFolder );
    offset += KSConXmlParentFolder().Length();

    HBufC* fullNameBuf = HBufC::NewLC( KMaxPath );
    TPtr fullName = fullNameBuf->Des();
    
    // Print folders to folder listing object
    for( TInt i = 0; i < dirList->Count(); i++ )
        {
        const TEntry entry = ( *dirList )[i];
        
        fullName.Copy( aPathName );
        fullName.Append( entry.iName );
        fullName.Append( KPathDelimiter );
        
        // show path if not exluded
        TBool proceed = !IsExludedPath( fullName );
        if ( proceed )
            {
            PrintFolderL( aBuffer, offset, fullName, memType, entry );
            }
        }
    CleanupStack::PopAndDestroy( fullNameBuf );
    CleanupStack::PopAndDestroy( dirList );
    
    LOGGER_WRITE("Print files");
    // Print files to folder listing object
    for( TInt j = 0; j < entryList->Count(); j++ )
        {
        const TEntry entry = ( *entryList )[j];
        PrintFileL( aBuffer, offset, entry );
        }
    LOGGER_WRITE("files printed");
    //Write the end of XML
    aBuffer->ExpandL( offset, KSConXmlFolderListEnd().Length() );
    aBuffer->Write( offset, KSConXmlFolderListEnd );
    aBuffer->Compress();
    
    CleanupStack::PopAndDestroy( entryList );

    LOGGER_WRITE_1( "CSConFsHandler::ParseFolderListL() : returned %d", err );
    return err;
    }


// -----------------------------------------------------------------------------
// CSConFsHandler::PrintFolderL
// Prints folder entry to folder listing object
// -----------------------------------------------------------------------------
//
void CSConFsHandler::PrintFolderL( CBufFlat* aBuffer, TInt& aOffset, const TDesC& aFullPath,
                    const TDesC8& aMemType, const TEntry& aFolderEntry )
    {
    TRACE_FUNC_ENTRY;
    // initialize Localizer
    iLocalizer->SetFullPath( aFullPath );
    
    TBool dataPath( EFalse );
    if ( aFullPath.CompareF( PathInfo::PhoneMemoryRootPath() ) == 0 ) // datapath
        {
        // printable folder is C:\Data (Localized)
        dataPath = ETrue;
        }
    
    // print foldername
    aBuffer->ExpandL( aOffset, KSConXmlFolderNameBegin().Length() );
    aBuffer->Write( aOffset, KSConXmlFolderNameBegin );
    aOffset += KSConXmlFolderNameBegin().Length();
    
    HBufC8* folderBuf = HBufC8::NewLC( KMaxFileName );
    TPtr8 folderName = folderBuf->Des();
    User::LeaveIfError( 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( folderName, aFolderEntry.iName ) );
    
    //Replace special characters
    ReplaceSpecialChars( folderName );

    aBuffer->ExpandL( aOffset, folderName.Length() );
    aBuffer->Write( aOffset, folderName );
    aOffset += folderName.Length();
    
    CleanupStack::PopAndDestroy( folderBuf );
    
    // print modified time
    TBuf<KSConDateMaxLength> modifiedTime;
    TTime time = aFolderEntry.iModified;
    time.FormatL( modifiedTime, KSConXmlDate );
    
    TBuf8<KSConDateMaxLength> modifiedTime8;
    User::LeaveIfError( 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( modifiedTime8, modifiedTime ) );
    
    aBuffer->ExpandL( aOffset, KSConXmlModified().Length() );
    aBuffer->Write( aOffset, KSConXmlModified );
    aOffset += KSConXmlModified().Length();
    
    aBuffer->ExpandL( aOffset, modifiedTime8.Length() );
    aBuffer->Write( aOffset, modifiedTime8 );
    aOffset += modifiedTime8.Length();
    
    // print attributes
    aBuffer->ExpandL( aOffset, KSConXmlUserAttributes().Length() );
    aBuffer->Write( aOffset, KSConXmlUserAttributes );
    aOffset += KSConXmlUserAttributes().Length();
    
    // Add user-perm info
    if( aFolderEntry.IsReadOnly() )
        {
        aBuffer->ExpandL( aOffset, 
            KSConXmlUserEntryReadOnly().Length() );
        aBuffer->Write( aOffset, KSConXmlUserEntryReadOnly );
        aOffset += KSConXmlUserEntryReadOnly().Length();
        }
    else if( iLocalizer->IsLocalized() || dataPath ) // delete disabled
        {
        aBuffer->ExpandL( aOffset, 
            KSConXmlUserEntryDrive().Length() );
        aBuffer->Write( aOffset, KSConXmlUserEntryDrive );
        aOffset += KSConXmlUserEntryDrive().Length();
        }
    else
        {
        aBuffer->ExpandL( aOffset, 
            KSConXmlUserEntryArchive().Length() );
        aBuffer->Write( aOffset, KSConXmlUserEntryArchive );
        aOffset += KSConXmlUserEntryArchive().Length();
        }
    
    
    // print memory type
    aBuffer->ExpandL( aOffset, KSConXmlMemoryType().Length() );
    aBuffer->Write( aOffset, KSConXmlMemoryType );
    aOffset += KSConXmlMemoryType().Length();
    
    aBuffer->ExpandL( aOffset, aMemType.Length() );
    aBuffer->Write( aOffset, aMemType );
    aOffset += aMemType.Length();
    
    // print label if exists
    if( iLocalizer->IsLocalized() )
        {
        //get localized folder name
        HBufC8* labelBuf = 
            CnvUtfConverter::ConvertFromUnicodeToUtf8L( iLocalizer->LocalizedName() );
        CleanupStack::PushL( labelBuf );
        TPtrC8 label = labelBuf->Des();
        //memory label
        aBuffer->ExpandL( aOffset, 
            KSConXmlMemoryLabel().Length() );
        aBuffer->Write( aOffset, KSConXmlMemoryLabel );
        aOffset += KSConXmlMemoryLabel().Length();
        
        aBuffer->ExpandL( aOffset, label.Length() );
        aBuffer->Write( aOffset, label );
        aOffset += label.Length();
        
        CleanupStack::PopAndDestroy( labelBuf );
        }
    if ( dataPath ) // datapath
        {
        //memory label
        aBuffer->ExpandL( aOffset, 
            KSConXmlMemoryLabel().Length() );
        aBuffer->Write( aOffset, KSConXmlMemoryLabel );
        aOffset += KSConXmlMemoryLabel().Length();
        
        aBuffer->ExpandL( aOffset, iDataPathName.Length() );
        aBuffer->Write( aOffset, iDataPathName );
        aOffset += iDataPathName.Length();
        }
    // print ending
    aBuffer->ExpandL( aOffset, KSConXmlFileEnd().Length() );
    aBuffer->Write( aOffset, KSConXmlFileEnd );
    aOffset += KSConXmlFileEnd().Length();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::PrintFileL
// Prints file entry to folder listing object
// -----------------------------------------------------------------------------
//
void CSConFsHandler::PrintFileL( CBufFlat* aBuffer, TInt& aOffset, const TEntry& aFileEntry )
    {
    LOGGER_WRITE_1("CSConFsHandler::PrintFileL, filename: %S", &aFileEntry.iName );
    HBufC8* fileBuf = HBufC8::NewLC( KMaxFileName );
    TPtr8 fileName = fileBuf->Des();
        
    User::LeaveIfError( 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( fileName, aFileEntry.iName ) );

    //Replace special characters
    ReplaceSpecialChars( fileName );
        
    // print filename
    aBuffer->ExpandL( aOffset, KSConXmlFileNameBegin().Length() );
    aBuffer->Write( aOffset, KSConXmlFileNameBegin );
    aOffset += KSConXmlFileNameBegin().Length();

    aBuffer->ExpandL( aOffset, fileName.Length() );
    aBuffer->Write( aOffset, fileName );
    aOffset += fileName.Length();

    // print size
    aBuffer->ExpandL( aOffset, KSConXmlSize().Length() );
    aBuffer->Write( aOffset, KSConXmlSize );
    aOffset += KSConXmlSize().Length();

    fileName.Delete( 0, fileName.Length() ); // re-use fileName descriptor
    fileName.AppendNum( aFileEntry.iSize );
    aBuffer->ExpandL( aOffset, fileName.Length() );
    aBuffer->Write( aOffset, fileName );
    aOffset += fileName.Length();
    
    CleanupStack::PopAndDestroy( fileBuf );

    // print modified
    TBuf<KSConDateMaxLength> modifiedTime;
    TTime time = aFileEntry.iModified;
    time.FormatL( modifiedTime, KSConXmlDate );
    
    TBuf8<KSConDateMaxLength> modifiedTime8;
    User::LeaveIfError( 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( modifiedTime8, modifiedTime ) );
    
    
    aBuffer->ExpandL( aOffset, KSConXmlModified().Length() );
    aBuffer->Write( aOffset, KSConXmlModified );
    aOffset += KSConXmlModified().Length();
    
    aBuffer->ExpandL( aOffset, modifiedTime8.Length() );
    aBuffer->Write( aOffset, modifiedTime8 );
    aOffset += modifiedTime8.Length();

    // attributes
    aBuffer->ExpandL( aOffset, KSConXmlUserAttributes().Length() );
    aBuffer->Write( aOffset, KSConXmlUserAttributes );
    aOffset += KSConXmlUserAttributes().Length();

    if( aFileEntry.IsSystem() )
        {
        aBuffer->ExpandL( aOffset, 
            KSConXmlUserEntryReadOnly().Length() );
        aBuffer->Write( aOffset, KSConXmlUserEntryReadOnly );
        aOffset += KSConXmlUserEntryReadOnly().Length();
        }
    else if( aFileEntry.IsReadOnly() )
        {
        LOGGER_WRITE("was readonly" );
        aBuffer->ExpandL( aOffset, 
            KSConXmlUserEntryReadOnly().Length() );
        aBuffer->Write( aOffset, KSConXmlUserEntryReadOnly );
        aOffset += KSConXmlUserEntryReadOnly().Length();
        }
    else
        {
        aBuffer->ExpandL( aOffset, 
            KSConXmlUserEntryArchive().Length() );
        aBuffer->Write( aOffset, KSConXmlUserEntryArchive );
        aOffset += KSConXmlUserEntryArchive().Length();
        }
    

    //Word document
    if ( aFileEntry.iType.IsPresent( KSConWordUid ) ||
            aFileEntry.iName.Find( KSConEpocWordExt ) == aFileEntry.iName.Length() - 4 )
        {
        aBuffer->ExpandL( aOffset, KSConXmlType().Length() );
        aBuffer->Write( aOffset, KSConXmlType );
        aOffset += KSConXmlType().Length();
        
        aBuffer->ExpandL( aOffset, KSConXmlTypeWord().Length() );
        aBuffer->Write( aOffset, KSConXmlTypeWord );
        aOffset += KSConXmlTypeWord().Length();
        }
    //Sheet document
    else if ( aFileEntry.iType.IsPresent( KSConSheetUid ) ||
            aFileEntry.iName.Find( KSConEpocSheetExt ) == aFileEntry.iName.Length() - 4 )
        {
        aBuffer->ExpandL( aOffset, KSConXmlType().Length() );
        aBuffer->Write( aOffset, KSConXmlType );
        aOffset += KSConXmlType().Length();
        
        aBuffer->ExpandL( aOffset, KSConXmlTypeSheet().Length() );
        aBuffer->Write( aOffset, KSConXmlTypeSheet );
        aOffset += KSConXmlTypeSheet().Length();
        }

    aBuffer->ExpandL( aOffset, KSConXmlFileEnd().Length() );
    aBuffer->Write( aOffset, KSConXmlFileEnd );
    aOffset += KSConXmlFileEnd().Length();
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::GetFileObjectL( CObexFileObject*& aFileObject, 
// const TPtrC aPathName, const TPtrC aFileName )
// Gets a file object
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::GetFileObjectL( CObexFileObject*& aFileObject, 
                                const TPtrC aPathName, const TPtrC aFileName )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_2( "CSConFsHandler::GetFileObjectL aFileName =  %S aPathName = %S", &aFileName, &aPathName );

    TInt ret( KErrNone );
    
    if( ( aPathName.Length() == 0 || aFileName.Length() == 0 ) && 
    !( aFileName==KPhoneBookIrMCL2Request || 
    aFileName==KInfoLogIrMCL2Request ) )
        {
        return KErrArgument;
        }

    if( aPathName.Length() + aFileName.Length() > KMaxFileName )
        {
        return KErrAbort;
        }

    HBufC* pathBuf = HBufC::NewLC( KMaxFileName );
    TPtr pathBufPtr = pathBuf->Des();

    /*These modifiactions are mean to catch an IrMC L2 request. 
    If the request is made, then the requested phonebook file and
    info log -file are created. Path and filename is also changed 
    to point to these created files.*/
    if( aFileName == KPhoneBookIrMCL2Request || 
    aFileName == KInfoLogIrMCL2Request )
        {
        LOGGER_WRITE( "CSConFsHandler: IrMC L2 request!" );
        TInt err;
        TInt status;
        TInt contactsSaved = 0;
        RCntParserServer cntParserServer;

        err = cntParserServer.Connect();
        if( err != KErrNone )
            {
            LOGGER_WRITE( "CSConFsHandler: Cannot connect to the cntparserserver, ABORT!" );

            CleanupStack::PopAndDestroy( pathBuf );
            return KErrAbort;
            }

        LOGGER_WRITE( "CSConFsHandler: Time to call CreateIrMCL2PhoneBook" );
        TBool force = ( aFileName == KInfoLogIrMCL2Request );
        status=cntParserServer.CreateIrMCL2PhoneBook( KMaximumNumberOfContacts,
         force );
        LOGGER_WRITE_1( "CntParserServer.CreateIrMCL2PhoneBook returned %d", status );
         
        //Check how many vcards were saved
        contactsSaved = cntParserServer.ContactsSaved();    
        LOGGER_WRITE_1( "CntParserServer.ContactsSaved returned %d", contactsSaved );

        err = cntParserServer.Disconnect();
        LOGGER_WRITE_1( "CntParserServer.Disconnect() %d", err );
        if( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( pathBuf );
            return KErrAbort;
            }
        //save info.log, if pb.vcf has changed (Note: KPBNotUpdated is returned
        // if no changes made to pb.vcf)
        if( status == KErrNone )    
            {
            LOGGER_WRITE( "CSConFsHandler: Create info.log" );

            CCntParserInfoLog* infoLog=CCntParserInfoLog::NewL();
            CleanupStack::PushL( infoLog );
            //see KMaximumNumberOfContacts for more details
            infoLog->SetMaximumRecords( contactsSaved ); 
            infoLog->SetTotalRecords( contactsSaved );
            TFileName privatepath( K_C_ROOT );
            privatepath.Append( KInfoLogIrMCL2File );
            infoLog->SaveToFileL( privatepath );
            CleanupStack::PopAndDestroy( infoLog );
            }

        if( aFileName == KInfoLogIrMCL2Request )
            {
            //Changes filename and path to info.log
            pathBufPtr.Copy( KInfoLogPathAndFileName ); 
            }
        else
            {
            //Changes filename and path to pb.vcf
            pathBufPtr.Copy( KPbPathAndFileName );  
            }
        }       
    else
        {
        pathBufPtr.Copy( aPathName );
        pathBufPtr.Append( aFileName );         
        }
    
    LOGGER_WRITE( "CSConFsHandler::GetFileObjectL() : aFileObject->InitFromFileL( pathBufPtr )" );
    TRAP( ret, aFileObject->InitFromFileL( pathBufPtr ) );
    LOGGER_WRITE( "CSConFsHandler::GetFileObjectL() : aFileObject->InitFromFileL( pathBufPtr ) ok" );

    CleanupStack::PopAndDestroy( pathBuf );

    LOGGER_WRITE_1( "CSConFsHandler::GetFileObjectL( CObexFileObject*& aFileObject, const TPtrC aPathName, const TPtrC aFileName ) : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::SaveFileObjectL( const TPtrC aFullPathName, const TTime aTime,
// const TPtrC aTempFileName )
// Stores a file object
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::SaveFileObjectL( const TPtrC aFullPathName, 
                        const TTime aTime, const TPtrC aTempFileName )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_2( "CSConFsHandler::SaveFileObjectL aFullPathName = %S aTempFileName = %S", &aFullPathName, &aTempFileName );

    TInt ret( KErrNone );
    
    if( aFullPathName.Length() == 0 )
        {
        iFs.Delete( aTempFileName );
        return KErrPathNotFound;
        }

    ret = ValidateFileName( aFullPathName );
        
    if( ret == KErrNone )
        {
        TUint attr;
        TInt err = iFs.Att( BaflUtils::DriveAndPathFromFullName(aFullPathName), attr );
        if ( err == KErrNone && attr & KEntryAttReadOnly )
            {
            // folder is read only, permission denied
            iFs.Delete( aTempFileName );
            ret = KErrAccessDenied;
            }
        else if( !BaflUtils::FileExists( iFs, aFullPathName ) ) 
            {
            // file does not exist, rename.
            ret = iFs.Rename( aTempFileName, aFullPathName );

            if( ret == KErrNone )
                {
                ret = iFs.SetModified( aFullPathName, aTime );
                ret = KErrNone;
                }
            else
                {
                iFs.Delete( aTempFileName );
                }
            }
        else
            {
            LOGGER_WRITE( "CSConFsHandler::SaveFileObjectL() file exists. replace" );
            // file exists. Replace if not read-only file.
            TUint attr;
            ret = iFs.Att( aFullPathName, attr );
            // Check permissions
            if( ret == KErrNone && ( attr == KEntryAttNormal || 
            attr == KEntryAttArchive ) && !(attr & KEntryAttReadOnly) )
                {
                ret = iFs.Replace( aTempFileName, aFullPathName );
                LOGGER_WRITE_1( "CSConFsHandler::SaveFileObjectL() iFs.Replace ret %d", ret);
                //Set correct time stamp
                if( ret == KErrNone )
                    {
                    ret = iFs.SetModified( aFullPathName, aTime );
                    ret = KErrNone;
                    }
                else
                    {
                    iFs.Delete( aTempFileName );
                    }
                }
            else
                {
                LOGGER_WRITE( "CSConFsHandler::SaveFileObjectL() no permissions" );
                iFs.Delete( aTempFileName );
                ret = KErrAccessDenied;
                }
            }   
        }
    else
        {
        iFs.Delete( aTempFileName );
        }
    
    LOGGER_WRITE_1( "CSConFsHandler::SaveFileObjectL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::ValidateFileName( const TPtrC aName )
// Validates the file name
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::ValidateFileName( const TPtrC aName )
    {
    TInt ret( KErrNone );

    if( !iFs.IsValidName( aName ) )
        {
        ret = KErrBadName;
        }

    LOGGER_WRITE_1( "CSConFsHandler::ValidateFileName( const TPtrC aName ) : returned %d", ret  );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::CreateFolderL( const TPtrC aFolderPath )
// Creates a folder
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::CreateFolderL( const TPtrC aFolderPath )
    {
    TRACE_FUNC_ENTRY;
    
    TInt ret( KErrNone );
    
    if( aFolderPath.Length() == 0 )
        {
        return KErrArgument;
        }
    if( aFolderPath.Length() > KMaxPath )
        {
        return KErrAbort;
        }
    if( BaflUtils::PathExists( iFs, aFolderPath ) )
        {
        return KErrAlreadyExists;
        }
    // get restore mode
    TBool restoreInProgress( EFalse );
    RProperty backupProperty;
    TInt burState( 0 );
    TInt burErr = backupProperty.Get( KUidSystemCategory, conn::KUidBackupRestoreKey, burState ); 
    const conn::TBURPartType partType = 
        static_cast< conn::TBURPartType >( burState & conn::KBURPartTypeMask );
    if( burErr == KErrNone &&
     ( partType == conn::EBURRestorePartial || partType == conn::EBURRestoreFull ) )
        {
        // restore mode
        LOGGER_WRITE( "CSConFsHandler::CreateFolderL() restore mode" );
        restoreInProgress = ETrue;
        }
    
    // check permissions if not restore mode
    if ( !restoreInProgress )
        {
        if ( IsExludedPath( aFolderPath ) )
            {
            // user has no access to this folder
            LOGGER_WRITE( "CSConFsHandler::CreateFolderL() KErrAccessDenied" );
            return KErrAccessDenied;
            }
        
        // user can create whole folder path at once and we need to check that
        // user has permission to create folders. We need to find first folder that 
        // should be created, and see if parent folder has read-only permission.
        
        HBufC* fullPath = HBufC::NewLC( KMaxFileName );
        TPtr fullPathPtr = fullPath->Des();
    
        fullPathPtr.Copy(aFolderPath);
        TBool parentFound(EFalse);
        LOGGER_WRITE( "CSConFsHandler::CreateFolder() start loop" );
        TInt err(KErrNone);
        while (!parentFound && err==KErrNone)
            {
            err = GetParentFolder( fullPathPtr, fullPathPtr );
            if( err == KErrNone )
                {
                if (BaflUtils::PathExists( iFs, fullPathPtr ))
                    {
                    parentFound = ETrue;
                    TUint attr;
                    ret = iFs.Att( fullPathPtr, attr );
                    if ( ret == KErrNone &&  (attr & KEntryAttReadOnly || attr & KEntryAttSystem) )
                        {
                        // parent folder is read-only
                        LOGGER_WRITE( "CSConFsHandler::CreateFolderL() KErrAccessDenied" );
                        err = KErrAccessDenied;
                        }
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( fullPath );
        if ( err == KErrAccessDenied )
            {
            return KErrAccessDenied;
            }
        }
    
    ret = iFs.MkDirAll( aFolderPath );
    LOGGER_WRITE_1( "CSConFsHandler::CreateFolderL( const TPtrC aFolderPath ) : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::DeleteObjectL( const TPtrC aFullPath )
// Removes a folder or a file
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::DeleteObjectL( const TPtrC aFullPath )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aFullPath: %S", &aFullPath);
    TInt ret( KErrNone );
    
    if( aFullPath.Length() == 0 )
        {
        return KErrArgument;
        }
    
    if( aFullPath.Length() > KMaxFileName )
        {
        return KErrAbort;
        }
    
    TUint attr;
    // get source folder permissions.
    ret = iFs.Att( BaflUtils::DriveAndPathFromFullName(aFullPath), attr );
    if ( ret == KErrNone && (attr & KEntryAttReadOnly || attr & KEntryAttSystem ) )
        {
        return KErrAccessDenied;
        }
    
    
    // Check file/folder permissions
    ret = iFs.Att( aFullPath, attr );
    if ( ret == KErrNone && ( attr & KEntryAttReadOnly || attr & KEntryAttSystem ) )
        {
        return KErrAccessDenied;
        }
    
    TBool isFolder(EFalse);
    ret = BaflUtils::IsFolder( iFs, aFullPath, isFolder );
    
    if( isFolder && ret == KErrNone )
        {
        HBufC* fullPath = HBufC::NewLC( KMaxFileName );
        TPtr fullPathPtr = fullPath->Des();
        
        fullPathPtr.Copy( aFullPath );
        //check if there is a slash at the end
        if( fullPathPtr.LocateReverse( KPathDelimiter ) != fullPathPtr.Length()-1 )
            {
            fullPathPtr.Append( KPathDelimiter );
            }
        
        if ( !IsFolderVisible( fullPathPtr ) )
            {
            LOGGER_WRITE("folder was not visible");
            CleanupStack::PopAndDestroy( fullPath );
            return KErrNotFound;
            }
        else if ( !IsFolderDeletable( fullPathPtr ) )
            {
            LOGGER_WRITE("folder was not deletable");
            CleanupStack::PopAndDestroy( fullPath );
            return KErrAccessDenied;
            }
        
        ret = iFs.RmDir( fullPathPtr );
        LOGGER_WRITE_1("iFs.RmDir err: %d", ret);
        //delete non-empty folder
        if( ret != KErrNone )
            {
            CFileMan* fileMan = CFileMan::NewL( iFs );
            CleanupStack::PushL( fileMan );
            // if directory contains read-only files, those files are not deleted
            // and KErrAccessDenied is returned.
            ret = fileMan->RmDir( fullPathPtr );
            LOGGER_WRITE_1("fileMan->RmDir err: %d", ret);
            CleanupStack::PopAndDestroy( fileMan ); //fileman
            }
        if( ret == KErrNone )
            {
            // check is delete operation really succesfully completed
            if( BaflUtils::PathExists( iFs, fullPathPtr ) )
                {
                // Folder still exist, return error
                // KErrBadName mapped to precondition failed error.
                ret = KErrBadName;
                }
            }
        CleanupStack::PopAndDestroy( fullPath ); //fullPath
        }
    else if (ret == KErrNone)
        {
        // deleting file
        if ( IsExludedPath( aFullPath ) )
            {
            LOGGER_WRITE("Cannot delete from specified folder, return KErrAccessDenied");
            ret = KErrAccessDenied;
            }
        else
            {
            ret = iFs.Delete( aFullPath );
            }
        }
    
    
    
    LOGGER_WRITE_1( "CSConFsHandler::DeleteObjectL() : returned %d", ret );
    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSConFsHandler::CopyOrMoveFile()
// Copy or Move file/folder.
// -----------------------------------------------------------------------------
//
void CSConFsHandler::DoCopyOrMoveFileL( const TDesC& aSource, const TDesC& aTarget,
                                 const TBool aUseCopyCommand)
    {
    TRACE_FUNC_ENTRY;
    TInt lastError(KErrNone);
    TBool isFolder;
    
    if( !iFs.IsValidName( aSource ) || !iFs.IsValidName( aTarget ) )
        {
        LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Not valid name" );
        User::Leave( KErrBadName );
        }
    
    // will leave if source (file/folder) does not exists.
    User::LeaveIfError( BaflUtils::IsFolder(iFs, aSource, isFolder ) );
    
    // Check source file/folder permissions
    TUint attr;
    User::LeaveIfError( iFs.Att( aSource, attr ) );
    if ( !aUseCopyCommand && (attr & KEntryAttReadOnly || attr & KEntryAttSystem ) )
        {
        LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : no permissions, abort" );
        User::Leave(  KErrAccessDenied );
        }
    
    HBufC* sourceFolderBuf = HBufC::NewLC( KMaxFileName );
    TPtr sourceFolder = sourceFolderBuf->Des();
    HBufC* targetFolderBuf = HBufC::NewLC( KMaxFileName );
    TPtr targetFolder = targetFolderBuf->Des();
    
    sourceFolder.Copy( BaflUtils::DriveAndPathFromFullName(aSource) );
    targetFolder.Copy( BaflUtils::DriveAndPathFromFullName(aTarget) );
    
    
    // check disk space
    TInt driveNumber;
    
    User::LeaveIfError( iFs.CharToDrive(targetFolder[0], driveNumber) );
    
    TVolumeInfo volumeInfo;
    TInt err = iFs.Volume(volumeInfo, driveNumber);
    if( err != KErrNone )
        {
        LOGGER_WRITE_1( "CSConFsHandler::DoCopyOrMoveFileL : iFs.Volume err %d", err );
        User::Leave( KErrNotFound );
        }
    
    if( !isFolder )
        {
        // file handling routines
        if( !IsFolderVisible( aSource ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Current folder doesn't exist" );
            User::Leave( KErrNotFound );
            }
        else if( !aUseCopyCommand && IsFolderReadOnly( sourceFolder ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : folder read only, abort" );
            User::Leave( KErrAccessDenied );
            }
            
        // source file is ok, check target

        if( !IsFolderVisible( targetFolder ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Current target file/folder doesn't exist" );
            User::Leave( KErrNotFound );
            }
        else if( IsFolderReadOnly( targetFolder ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : folder read only, abort" );
            User::Leave( KErrAccessDenied );
            }
        else
            {
            TBool diskFull( EFalse );
            TEntry fileEntry;
            User::LeaveIfError( iFs.Entry( aSource, fileEntry ) );
            if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, fileEntry.iSize, driveNumber ) )
                {
                diskFull = ETrue;
                }
            
            // target ok, ready to copy or move
            CFileMan* fileMan=CFileMan::NewL(iFs);
            CleanupStack::PushL(fileMan);
            
            if( aUseCopyCommand )
                {
                if( diskFull )
                    {
                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : disk full" );
                    User::Leave( KErrDiskFull );
                    }
                LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Copy file" );
                User::LeaveIfError( fileMan->Copy( aSource, aTarget, 0 ) );
                }
            else
                {
                if( aSource.Find( aTarget.Left(2) ) == KErrNone)
                    {
                    // same drive
                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Rename file" );
                    User::LeaveIfError( fileMan->Rename( aSource, aTarget, 0 ) );
                    }
                else
                    {
                    // different drive
                    if( diskFull )
                        {
                        LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : disk full" );
                        User::Leave( KErrDiskFull );
                        }
                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : copy file" );
                    User::LeaveIfError( fileMan->Copy( aSource, aTarget, 0 ) );

                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : delete original" );
                    User::LeaveIfError( fileMan->Delete( aSource, 0 ) );
                    }
                }
            CleanupStack::PopAndDestroy( fileMan ); // fileMan
            }
        }
    else
        {
        // set trailing backslash after the final directory name
        HBufC* tmpSourceBuf = HBufC::NewLC( KMaxFileName );
        TPtr tmpSourcePtr = tmpSourceBuf->Des();
        HBufC* tmpTargetBuf = HBufC::NewLC( KMaxFileName );
        TPtr tmpTargetPtr = tmpTargetBuf->Des();
        
        tmpSourcePtr.Copy( aSource );
        if( tmpSourcePtr.LocateReverse( KPathDelimiter ) != tmpSourcePtr.Length()-1 )
            {
            tmpSourcePtr.Append( KPathDelimiter );
            }

        tmpTargetPtr.Copy( aTarget );
        if( tmpTargetPtr.LocateReverse( KPathDelimiter ) != tmpTargetPtr.Length()-1 )
            {
            tmpTargetPtr.Append( KPathDelimiter );
            }
        
        if( !IsFolderVisible( tmpSourcePtr ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Current folder doesn't exist" );
            User::Leave( KErrNotFound );
            }
        else if( !aUseCopyCommand && IsFolderReadOnly( sourceFolder ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : sourcefolder read only, abort" );
            User::Leave( KErrAccessDenied );
            }
        else if( !aUseCopyCommand && !IsFolderDeletable( tmpSourcePtr ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : source folder read only, abort" );
            User::Leave( KErrAccessDenied );
            }
        else if( !IsFolderVisible( targetFolder ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Current target base-folder doesn't exist" );
            User::Leave( KErrNotFound );
            }
        else if( IsFolderReadOnly( targetFolder) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : target folder read only, abort" );
            User::Leave( KErrAccessDenied );
            }
        else if( BaflUtils::FolderExists(iFs, tmpTargetPtr ) )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : target folder exist, abort" );
            User::Leave( KErrAlreadyExists );
            }
        else if ( tmpTargetPtr.Find( tmpSourcePtr ) == KErrNone )
            {
            LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : conflict, abort" );
            User::Leave( KErrInUse );
            }
        else
            {
            // is folder empty
            TBool emptyFolder( ETrue );
            CDir *entryList = 0;
            CDir *dirList = 0;

            User::LeaveIfError( iFs.GetDir( tmpSourcePtr, KEntryAttNormal, EDirsAnyOrder, 
            entryList, dirList ) );
            if( entryList->Count() > 0 || dirList->Count() > 0 )
                {
                emptyFolder = EFalse;
                }
                
            delete entryList;
            delete dirList;
            
            // get folder size
            TBool diskFull( EFalse );
            if( !emptyFolder )
                {
                TInt64 folderSize = GetFolderSizeL(tmpSourcePtr);
                LOGGER_WRITE_1( "CSConFsHandler::DoCopyOrMoveFileL : folderSize: %d", folderSize );
                if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, folderSize, driveNumber ) )
                    {
                    diskFull = ETrue;
                    }
                }
            
            
            CFileMan* fileMan = CFileMan::NewL(iFs);
            CleanupStack::PushL(fileMan);
    
            // ready to copy or move
            if( aUseCopyCommand )
                {
                LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Copy folder" );
                if( emptyFolder )
                    {
                    // folder is empty, can't copy folder. Create new empty folder.
                    User::LeaveIfError( iFs.MkDir( tmpTargetPtr ) );
                    }
                else
                    {
                    // folder not empty
                    if( diskFull )
                        {
                        LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : disk full" );
                        User::Leave( KErrDiskFull );
                        }
                    
                    User::LeaveIfError( 
                     fileMan->Copy( aSource, aTarget, CFileMan::ERecurse ) ); 
                    if( lastError == KErrNone )
                        {
                        lastError = fileMan->GetLastError();
                        }
                    }
                
                TUint attr;
                TInt err = iFs.Att( tmpSourcePtr, attr );
                if ( err == KErrNone && (attr & KEntryAttReadOnly ) )
                    {
                    //set read only attribute
                    err = iFs.SetAtt( tmpTargetPtr, KEntryAttReadOnly, KEntryAttArchive );
                    if (err != KErrNone)
                        {
                        // can't set attribute
                        LOGGER_WRITE_1( "CSConFsHandler::DoCopyOrMoveFileL : iFs.SetAtt err: %d",err );
                        }
                    }
                }
            else
                {
                if( aSource.FindF( aTarget.Left(2) ) == KErrNone)
                    {
                    // same drive
                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Rename folder" );
                    User::LeaveIfError( fileMan->Rename( aSource, aTarget, 0 ) );
                    }
                else
                    {
                    // different drive
                    if( diskFull )
                        {
                        LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : disk full" );
                        User::Leave( KErrDiskFull );
                        }
                    
                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : Copy folder" );
                    if( emptyFolder )
                        {
                        // folder is empty, can't copy empty folder. Create empty folder.
                        User::LeaveIfError( iFs.MkDir( tmpTargetPtr ) );
                        }
                    else
                        {
                        User::LeaveIfError( 
                         fileMan->Copy( aSource, aTarget, CFileMan::ERecurse ) );
                        }
                    // copy completed, delete original folder.
                    LOGGER_WRITE( "CSConFsHandler::DoCopyOrMoveFileL : delete original" );
                    User::LeaveIfError( fileMan->RmDir( tmpSourcePtr ) );
                    // check was delete operation succesfully completed
                    if( BaflUtils::PathExists( iFs, tmpSourcePtr ) )
                        {
                        // Folder still exist, return error
                        lastError = KErrAccessDenied;
                        }
                    }
                }
            CleanupStack::PopAndDestroy(fileMan); // fileMan
            }
        CleanupStack::PopAndDestroy(tmpTargetBuf); // tmpTargetBuf
        CleanupStack::PopAndDestroy(tmpSourceBuf); // tmpSourceBuf
        }
    
    CleanupStack::PopAndDestroy( targetFolderBuf );
    CleanupStack::PopAndDestroy( sourceFolderBuf );
    
    if( lastError != KErrNone )
        {
        LOGGER_WRITE_1( "CSConFTP::DoCopyOrMoveFileL() : lastError: %d", lastError );
        User::Leave( lastError );
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::IsFolderVisible()
// Check if folder can be showed
// -----------------------------------------------------------------------------
//
TBool CSConFsHandler::IsFolderVisible( const TDesC& aFullPath )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFullPath: %S", &aFullPath );
    TBool visible(EFalse);
    
    if( !BaflUtils::FolderExists( iFs, aFullPath ) )
        {
        LOGGER_WRITE( "CSConFsHandler::IsFolderVisible() notExist ret EFalse" );
        return EFalse;
        }
    
    // don't show if folder is exluded
    visible = !IsExludedPath( aFullPath );
    LOGGER_WRITE_1( "CSConFsHandler::IsFolderVisible() : end, ret %d", (TInt)visible );
    return visible;
    }
    
// -----------------------------------------------------------------------------
// CSConFsHandler::IsFolderReadOnly()
// Check if folder is read only (system folder)
// -----------------------------------------------------------------------------
//
TBool CSConFsHandler::IsFolderReadOnly( const TDesC& aFullPath ) const
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFullPath: %S", &aFullPath );
    TBool readOnly(EFalse);
    
    // C-drive root is read only, but there are some exceptions
    
    readOnly = IsExludedPath( aFullPath );
    
    if ( aFullPath.CompareF( K_C_ROOT ) == 0 )
        {
        // Root of C-drive is read-only
		LOGGER_WRITE("Root of C-Drive is read only");
        readOnly = ETrue;
        }
    
    // Check folder permissions
    TUint attr;
    TInt err = iFs.Att( aFullPath, attr );
    LOGGER_WRITE_1("iFs.Att err: %d", err);
    if ( err == KErrNone && (attr & KEntryAttReadOnly || attr & KEntryAttSystem ) )
        {
        LOGGER_WRITE("Folder is readonly");
        readOnly = ETrue;
        }
    
    LOGGER_WRITE_1( "CSConFsHandler::IsFolderReadOnly() : end, ret %d", (TInt)readOnly );
    return readOnly;
    }
    
// -----------------------------------------------------------------------------
// CSConFsHandler::IsFolderDeletable()
// Check if folder can be deleted. Localized folders cannot delete.
// In C-drive folders can be deleted only from under C:\\Data\\
// -----------------------------------------------------------------------------
//
TBool CSConFsHandler::IsFolderDeletable( const TDesC& aFullPath ) const
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFullPath: %S", &aFullPath );
    TBool deleteAllowed(ETrue);
    TPath PhoneMem( PathInfo::PhoneMemoryRootPath() ); // "C:\\Data\\"

    if ( aFullPath.CompareF( PhoneMem ) == 0 )
        {
        // C:\\Data\\
        LOGGER_WRITE( "CSConFsHandler::IsFolderDeletable() datapath, ret EFalse" );
        deleteAllowed = EFalse;
        }
    else if ( IsExludedPath( aFullPath ) )
        {
        deleteAllowed = EFalse;
        }
    
    iLocalizer->SetFullPath( aFullPath );
    if ( iLocalizer->IsLocalized() ) // delete disabled
        {
        // Cannot delete localized paths (default paths)
        deleteAllowed = EFalse;
        }
    
    LOGGER_WRITE_1( "CSConFsHandler::IsFolderDeletable() : end, ret %d", (TInt)deleteAllowed );
    return deleteAllowed;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::GetParentFolder( const TPtrC aFullPath, TDes& aParent)
// Get parent folder
// -----------------------------------------------------------------------------
//
TInt CSConFsHandler::GetParentFolder( const TDesC& aFullPath, TDes& aParent)
    {
    TRACE_FUNC_ENTRY;
    if (aFullPath.LocateReverse(KPathDelimiter) == aFullPath.Length()-1 )
        {
        // remove backlash
        aParent.Copy(aFullPath.Left(aFullPath.Length()-1));
        }
    else
        {
        aParent.Copy(aFullPath);
        }
    
    TInt pos = aParent.LocateReverse(KPathDelimiter);
    if (pos != KErrNotFound)
        {
        aParent.Copy(aParent.Left(pos+1));
        LOGGER_WRITE( "CSConFsHandler::GetParentFolder : end KErrNone" );
        return KErrNone;
        }
    else
        {
        LOGGER_WRITE_1( "CSConFsHandler::GetParentFolder : end err %d", pos);
        return pos;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConFsHandler::CSConFsHandler()
// Constructor
// -----------------------------------------------------------------------------
//
CSConFsHandler::CSConFsHandler( RFs& aFs ) : iFs(aFs)
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConFsHandler::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iLocalizer = CDirectoryLocalizer::NewL();
    
    GetDataPathNameL();
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConFsHandler::GetDataPathNameL()
// Updates the name of the data folder from a localized string
// -----------------------------------------------------------------------------
//
void CSConFsHandler::GetDataPathNameL()
    {
    TRACE_FUNC_ENTRY;
    //Read memory string and convert it
    TFileName file( KSConResourceName );
    
    BaflUtils::NearestLanguageFile( iFs, file );
       
    CStringResourceReader* reader = CStringResourceReader::NewL( file );
    CleanupStack::PushL( reader );
    
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iDataPathName, 
        reader->ReadResourceString( R_SECON_DATA_FOLDER ) );
    
    CleanupStack::PopAndDestroy( reader );
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConFsHandler::GetFolderSizeL()
// Get folder (and subfolders) size
// -----------------------------------------------------------------------------
//
TInt64 CSConFsHandler::GetFolderSizeL( const TDesC& aFullPath )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFullPath: %S", &aFullPath );
    TInt64 totalSize(0);
    CDir* dir( NULL );
    
    User::LeaveIfError( iFs.GetDir(aFullPath, KEntryAttMatchMask, ESortByName, dir) );
    CleanupStack::PushL(dir);
    for( TInt i=0; i<dir->Count(); ++i )
        {
        TEntry entry = (*dir)[i];
        if( !entry.IsDir() )
            {
            // file
            totalSize += entry.iSize;
            }
        else
            {
            // folder
            HBufC* subfolderBuf = HBufC::NewLC( KMaxPath );
            TPtr subfolderPtr = subfolderBuf->Des();
            
            subfolderPtr.Copy(aFullPath);
            subfolderPtr.Append(entry.iName);
            subfolderPtr.Append( KPathDelimiter );
            
            LOGGER_WRITE( "CSConFsHandler::GetFolderSizeL() : get subfolder size" );
            totalSize += GetFolderSizeL( subfolderPtr );
            CleanupStack::PopAndDestroy( subfolderBuf );
            }
        }
    CleanupStack::PopAndDestroy(dir);
    TRACE_FUNC_EXIT;
    return totalSize;
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::ReplaceSpecialChars()
// Replace special characters to xml compliant
// -----------------------------------------------------------------------------
//
void CSConFsHandler::ReplaceSpecialChars( TDes8& aDes )
    {
    for( TInt i = 0; i < aDes.Length(); i++ )
        {
        switch( aDes[i] )
            {
            case '&':
                aDes.Delete( i, 1 );
                aDes.Insert( i, KReplace1 );
                break;
            case '<':
                aDes.Delete( i, 1 );
                aDes.Insert( i, KReplace2 );
                break;
            case '>':
                aDes.Delete( i, 1 );
                aDes.Insert( i, KReplace3 );
                break;
            case '"':
                aDes.Delete( i, 1 );
                aDes.Insert( i, KReplace4 );
                break;
            case '\'':
                aDes.Delete( i, 1 );
                aDes.Insert( i, KReplace5 );
                break;
            default:
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSConFsHandler::IsExludedPath()
// Some folders are exluded from user access
// -----------------------------------------------------------------------------
//
TBool CSConFsHandler::IsExludedPath( const TDesC& aFullPath ) const
    {
    TInt exluded(ETrue);
    LOGGER_WRITE_1( "aFullPath: %S", &aFullPath );
    TPath mmcRoot( PathInfo::MemoryCardRootPath() );
    if ( aFullPath.Length() == KDriveLength )
        {
        // root paths not exluded
        exluded = EFalse;
        }
    else if ( aFullPath.FindF( K_C_ROOT ) == 0 )
        {
        if ( aFullPath.FindF( PathInfo::PhoneMemoryRootPath() ) == 0 )
            {
            // C:\\Data is visible, not exluded
            exluded = EFalse;
            }
#ifdef SCON_SHOW_LOGS_FOLDER
        else if ( aFullPath.FindF( KSConLogsPath ) == KDriveLength )
            {
            // C:\\Logs is visible, not exluded
            exluded = EFalse;
            }
#endif
        else {
            // other paths on C-drive are exluded
            LOGGER_WRITE_1("CSConFsHandler::IsExludedPath() Path '%S' is not visible to user", &aFullPath);
            exluded = ETrue;
            }
        }
    // hide undesired folders from E: root level
    else if( aFullPath.CompareF( mmcRoot ) == 0 )
        {
        if ( aFullPath.FindF( KSConSys ) == KDriveLength )
            {
            LOGGER_WRITE( "hiding mmcRoot KSConSys" );
            exluded = ETrue;
            }
        else if( aFullPath.FindF( KSConResource ) == KDriveLength )
            {
            LOGGER_WRITE( "hiding mmcRoot KSConResource" );
            exluded = ETrue;
            }
        else if( aFullPath.FindF( KSConPrivate ) == KDriveLength )
            {
            LOGGER_WRITE( "mmcRoot KSConPrivate" );
            exluded = ETrue;
            }
        else if( aFullPath.FindF( KSConSystem ) == KDriveLength )
            {
            LOGGER_WRITE( "hiding mmcRoot KSConSystem" );
            exluded = ETrue;
            }
        }
    // end of E: root level handling
    else
        {
        // don't exlude paths from other drives
        exluded = EFalse;
        }
    return exluded;
    }

// End of file


