/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCapInfo implementation
*
*/


// INCLUDE FILES

#include <utf.h>
#include <driveinfo.h>
#include <pathinfo.h>    // for PathInfo::SoundsPath
#include <seconsdkcrkeys.h>
#include "sconsyncservice.h"
#include "capinfo.h"
#include "caplist.h"
#include "capparser.h"
#include "caputils.h"
#include "stringlist.h"
#include "sconversioninfo.h"
#include "debug.h"


_LIT8( KLineFeed, "\r\n" );

const TInt KDefaultArrayGranularity = 5;


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCapInfo::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CCapInfo* CCapInfo::NewL()
    {
    TRACE_FUNC_ENTRY;
    CCapInfo* self = new (ELeave) CCapInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE_FUNC_EXIT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// CCapInfo::~CCapInfo()
// Destructor
// -----------------------------------------------------------------------------
//
CCapInfo::~CCapInfo()
    {
    TRACE_FUNC_ENTRY;
    iFsSession.Close();
    delete iHeapBuf;
    delete iCapList;
    delete iSconInfo;
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CCapInfo::CapabilityDocumentL(CBufFlat* aBuf)
// Creates capability xml document end places it in aBuf
// -----------------------------------------------------------------------------
//
void CCapInfo::CapabilityDocumentL(CBufFlat* aBuf)
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNone );
    
    // set buffer where capability document is written
    SetCapabilityBuf( aBuf );
    
    WriteL( KXmlVersion );
    WriteL( KComment1 );
    WriteL( KDocType );
    WriteL( KRootBegin );
    
    
    //
    // general
    //
    TRAP( err, GeneralInfoL() );
    if ( err != KErrNone )
        {
        LOGGER_WRITE_1( "GeneralInfoL failed : %d", err );
        User::Leave( err ); 
        }
    
    
    //
    // services
    //
    // Search service files first from C-drive, and then from Z
    // Do not add same files twice.
    
    RArray<TFileName> excludeList( KDefaultArrayGranularity );
    CleanupClosePushL( excludeList );
    RArray<TFileName> filesAdded( KDefaultArrayGranularity );
    CleanupClosePushL( filesAdded );
    
    //read service files from C Import folder
    TRAP( err, ServiceInfoL( KCapabilityDirCImport, excludeList, filesAdded ) );     
    LOGGER_WRITE_1( "ServiceInfoL(KCapabilityDirCImport) returned : %d", err );
    for ( TInt i=0; i<filesAdded.Count(); i++ )
        {
        excludeList.Append( filesAdded[i] );
        }
    filesAdded.Reset();
    
    // read service files from C Capability folder
    TRAP( err, ServiceInfoL( KCapabilityDirC, excludeList, filesAdded ) ); 
    LOGGER_WRITE_1( "ServiceInfoL(KCapabilityDirC) returned : %d", err );
    for ( TInt i=0; i<filesAdded.Count(); i++ )
        {
        excludeList.Append( filesAdded[i] );
        }
    filesAdded.Reset();
    
    // read service files from Z Capability folder
    TRAP( err, ServiceInfoL( KCapabilityDirZ, excludeList, filesAdded ) );
    LOGGER_WRITE_1( "ServiceInfoL(KCapabilityDirZ) returned : %d", err );
    
    CleanupStack::PopAndDestroy( &filesAdded );
    CleanupStack::PopAndDestroy( &excludeList );
    
        
    // this service does not come from file
    TRAP( err, FolderServiceL() );
    LOGGER_WRITE_1( "FolderServiceL() returned : %d", err );
    
    TRAP( err, SyncSolutionsServiceL() );
    LOGGER_WRITE_1( "SyncSolutionsServiceL() returned: %d", err );

    WriteL( KRootEnd );
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CCapInfo::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CCapInfo::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iFsSession.Connect() );
    
    iHeapBuf = HBufC8::NewL( KMaxSize*2 );
    iCapList = CCapList::NewL();
    iIdStack.Reset();
    
    iSconInfo = CSconVersionInfo::NewL();
    TInt err(KErrNone);
    TRAP(err, iSconInfo->FetchInfoL( iFsSession ));
    LOGGER_WRITE_1("sconInfo->FetchInfoL() Leaved: %d", err);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::CCapInfo()
// Constuctor
// -----------------------------------------------------------------------------
//
CCapInfo::CCapInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCapInfo::CapList()
// Returns caplist
// -----------------------------------------------------------------------------
//
CCapList* CCapInfo::CapList()
    {
    TRACE_FUNC;
    __ASSERT_DEBUG(iCapList, CapUtil::Panic(KErrGeneral));
    return iCapList;
    }

// -----------------------------------------------------------------------------
// CCapInfo::SetCapabilityBuf( CBufFlat* aBuf )
// Set capability buffer
// -----------------------------------------------------------------------------
//
void CCapInfo::SetCapabilityBuf( CBufFlat* aBuf )
    {
    TRACE_FUNC;
    iCapabilityBuf=aBuf;
    }

// -----------------------------------------------------------------------------
// CCapInfo::GeneralInfoL()
// Write general device capability information.
// -----------------------------------------------------------------------------
//
void CCapInfo::GeneralInfoL()
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNone );
    TBuf<KBufSize> buf;
    TBuf<KBufSize> longName;
    TBuf<KBufSize> coutryCode;
    TBuf<KBufSize> networkID;
    
    WriteTagL(EGeneral, TXmlParser::EElementBegin);
    
    if ( !iSconInfo->IsReady() )
        {
        LOGGER_WRITE("iSconInfo wasn't ready, call iSconInfo->FetchInfoL");
        iSconInfo->FetchInfoL( iFsSession );
        }
    
    if ( iSconInfo->iManufacturer )
        {
        WriteValueL( EManufacturer, *iSconInfo->iManufacturer );
        }
    
    if ( iSconInfo->iProduct )
        {
        WriteValueL( EModel, *iSconInfo->iProduct );
        }
    
    // serial number
    if ( iSconInfo->iSerialNumber )
        {
        WriteValueL( ESN, *iSconInfo->iSerialNumber );
        }
    
    // software version 
    if ( iSconInfo->iSWVersion )
        {
        WriteAttributeL( ESW, *iSconInfo->iSWVersion, iSconInfo->iDate );
        }
    
    // language
    WriteValueL( ELanguage, iSconInfo->iLanguage );

    // this must succeed - do not trap
    MemoryInfoL();
    
    // Operator
    TRAP( err, CapUtil::GetOperatorNameL( longName, coutryCode, networkID ) );
    LOGGER_WRITE_1( "CapUtil::GGetOperatorNameL returned : %d", err );
    if( err == KErrNone )
        {
        LOGGER_WRITE_3( "CSConFsHandler::GetOperatorNameL longName =  %S coutryCode = %S networkID = %S", &longName, &coutryCode, &networkID );       
        _LIT( KNetworkInfo, "NetworkInfo" );
        _LIT( KCurrentNetwork, "CurrentNetwork=" );
        _LIT( KCountryCode, "CountryCode=" );
        _LIT( KNetworkID, "NetworkID=" );
        
        WriteTagL( EExt, TXmlParser::EElementBegin );
        WriteValueL( EXNam, KNetworkInfo );
        
        // write CurrentNetwork
        buf.Copy( KCurrentNetwork );
        buf.Append( longName );
        WriteValueL( EXVal, buf );
        
        // write CountryCode
        buf.Copy( KCountryCode );
        buf.Append( coutryCode );
        WriteValueL( EXVal, buf );
        
        // write NetworkID
        buf.Copy( KNetworkID );
        buf.Append( networkID );
        WriteValueL( EXVal, buf );
        
        WriteTagL( EExt, TXmlParser::EElementEnd );     
        }
    
    if ( iSconInfo->iModel )
        {
        // print modelname (phone model sales name. For example "N01".)
        WriteTagL( EExt, TXmlParser::EElementBegin );
        _LIT(KModelName, "ModelName");
        WriteValueL( EXNam, KModelName );
        WriteValueL( EXVal, *iSconInfo->iModel );
        WriteTagL( EExt, TXmlParser::EElementEnd ); 
        }
        
    if ( iSconInfo->iProductCode )
        {
        // print productcode
        WriteTagL( EExt, TXmlParser::EElementBegin );
        _LIT( KProductCode, "ProductCode" );
        WriteValueL( EXNam, KProductCode );
        WriteValueL( EXVal, *iSconInfo->iProductCode );
        WriteTagL( EExt, TXmlParser::EElementEnd );
        }
    
    
    // print advanced info
    WriteTagL( EExt, TXmlParser::EElementBegin );
    _LIT(KAdvancedDeviceInfo, "AdvancedDeviceInfo");
    WriteValueL( EXNam, KAdvancedDeviceInfo );
    
    TBuf<KSysVersionInfoTextLength> temp;
    
    if ( iSconInfo->iRevision )
        {
        // product revision. For example "01"
        _LIT( KRevision, "Revision=" );
        temp.Copy( KRevision );
        temp.Append( *iSconInfo->iRevision );
        LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
        WriteValueL( EXVal, temp );
        }
    
    
    if ( iSconInfo->iSysVersionInfo )
        {
        _LIT( KFWVersion, "FWVersion=" );
        temp.Copy( KFWVersion );
        temp.Append( *iSconInfo->iSysVersionInfo );
        LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
        WriteValueL( EXVal, temp );
        }
    
    SysVersionInfo::TSymbianOSVersion osVersion;
    err = iSconInfo->GetSymbianVersion( osVersion );
    LOGGER_WRITE_1("TSymbianOSVersion info error: %d", err)
    if ( !err )
        {
        // Now osVersion contains the Symbian OS version information
        _LIT( KSymbianOSVersion, "SymbianOSVersion=%d.%d" );
        temp.Format( KSymbianOSVersion,
                osVersion.iMajorVersion,
                osVersion.iMinorVersion  );
        LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
                        
        WriteValueL( EXVal, temp );
        }
    
    VersionInfo::TPlatformVersion platformVersion;
    err = iSconInfo->GetS60Version( platformVersion );
    if ( !err )
       {
       _LIT( KS60Version, "S60PlatformVersion=%d.%d" );
       temp.Format( KS60Version,
               platformVersion.iMajorVersion,
               platformVersion.iMinorVersion  );
       LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
       WriteValueL( EXVal, temp );
       }
    
    
    if ( iSconInfo->iLangVersion )
        {
        _LIT( KLangVersion, "LangVersion=" );
        temp.Copy( KLangVersion );
        temp.Append( *iSconInfo->iLangVersion );
        LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
        WriteValueL( EXVal, temp );
        }
    
    if ( iSconInfo->iLangSWVersion )
        {
        _LIT( KLangSWVersion, "LangSWVersion=" );
        temp.Copy( KLangSWVersion );
        temp.Append( *iSconInfo->iLangSWVersion );
        LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
        WriteValueL( EXVal, temp );
        }
    
    if ( iSconInfo->iOPVersion )
        {
        // CustomerVersion
        _LIT( KOPVersion, "OPVersion=" );
        temp.Copy( KOPVersion );
        temp.Append( *iSconInfo->iOPVersion );
        LOGGER_WRITE_1( "CCapInfo::GeneralInfoL() : %S ", &temp );
        WriteValueL( EXVal, temp );
        }
    
    // screen size
    LOGGER_WRITE_2("Size, width: %d, height: %d",
            iSconInfo->iScreenSize.iWidth,
            iSconInfo->iScreenSize.iHeight );
    _LIT( KWidth, "ScreenWidth=%d" );
    _LIT( KHeight, "ScreenHeight=%d" );
    
    temp.Format( KWidth, iSconInfo->iScreenSize.iWidth );
    WriteValueL( EXVal, temp );
    
    temp.Format( KHeight, iSconInfo->iScreenSize.iHeight );
    WriteValueL( EXVal, temp );
    
    WriteTagL( EExt, TXmlParser::EElementEnd ); 
    // end of advanced info
    
    WriteTagL(EGeneral, TXmlParser::EElementEnd);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::MemoryInfoL()
// Create memory info for all drives (RAM excluded).
// -----------------------------------------------------------------------------
//
void CCapInfo::MemoryInfoL()
    {
    TRACE_FUNC_ENTRY;
    
    TMemoryInfo info;
    TInt devDriveCount = 0;
    TInt mmcDriveCount = 0;
    
    //Write all drivers to folderlisting object
    TDriveList driveList;
    // Get all drives that are visible to the user.
    TInt driveCount;
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives( iFsSession, driveList, driveCount ) );
    
    for( TInt i = EDriveA; i < KMaxDrives; i++ )
        {
        if( driveList[i] )
            {
            TUint driveStatus;
            TInt err = DriveInfo::GetDriveStatus( iFsSession, i, driveStatus );
            if( err )
                {
                LOGGER_WRITE_1( "CCapInfo::MemoryInfoL() : DriveInfo::GetDriveStatus: %d", i);
                continue;
                }
            
            if( !(driveStatus & DriveInfo::EDrivePresent )
                || driveStatus & DriveInfo::EDriveCorrupt
                || (driveStatus & DriveInfo::EDriveRemote) )
                {
                LOGGER_WRITE_1( "skip drive %d", i);
                continue;
                }
            
            TRAP( err, CapUtil::GetMemoryInfoL( iFsSession, i, info ));
            if ( err )
                {
                LOGGER_WRITE_1( "CapUtil::GetMemoryInfoL() : Leaved with: %d", err);
                continue;
                }
            
            info.iDriveStatus = driveStatus;
            if( driveStatus & DriveInfo::EDriveInternal )
                {
                // if internal (DEV) drive, save drive number
                devDriveCount++;
                info.iMemNr = devDriveCount;
                }
            else if ( driveStatus & DriveInfo::EDriveRemovable )
                {
                // if removable (MMC) drive, save drive number
                mmcDriveCount++;
                info.iMemNr = mmcDriveCount;
                }
            else
                {
                // was not internal or removable, skip.
                LOGGER_WRITE( "CCapInfo::MemoryInfoL() : not internal or removable, skip." );
                continue;
                }
                
            WriteMemoryInfoL(info);
            
            }
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::MemoryInfoL(TMemoryInfo& aInfo)
// Create memory info for all drives (RAM excluded).
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteMemoryInfoL(TMemoryInfo& aInfo)
    {
    TRACE_FUNC_ENTRY;
    TBuf<KBufSize> buf;

    WriteTagL(EMemory, TXmlParser::EElementBegin);

    if( aInfo.iDriveStatus & DriveInfo::EDriveInternal )
        {
        buf = KMediaFlash;
        }
    else
        {
        buf = KMediaMMC;  // memory card (MMC)
        }
        
    if( aInfo.iMemNr > 1 )
        {
        // append memory number DEV2, MMC2..
        buf.AppendNum( aInfo.iMemNr ); 
        }
    
    WriteValueL(EMemType, buf);
    WriteValueL(ELocation, aInfo.iLocation);
    WriteValueL(EFree, aInfo.iFree);
    WriteValueL(EUsed, aInfo.iUsed);
    
    WriteValueL(EFileNLen, aInfo.iFileNameSize);

    WriteTagL(EMemory, TXmlParser::EElementEnd);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::ServiceInfoL(const TDesC& aDir)
// Reads service xml-files from directory and writes them into capability 
// document.
// -----------------------------------------------------------------------------
//
void CCapInfo::ServiceInfoL(const TDesC& aDir, const RArray<TFileName>& aExcludeList,
    RArray<TFileName>& aFilesAdded)
    {
    TRACE_FUNC_ENTRY;
    CCapList* capList=CapList();

    RArray<TFileName> array( KDefaultArrayGranularity );
    CleanupClosePushL(array);

    CStringList* stringList=CStringList::NewLC();

    CapUtil::GetFileListL(iFsSession, aDir, array);
    
    _LIT(KFormat, "%S%S");
    TFileName name;
    TInt count=array.Count();
    for (TInt i=0; i<count; i++)
        {
        TBool exludeFromList(EFalse);
        
        // Check if file is already added to list
        for ( TInt j=0; j<aExcludeList.Count() && !exludeFromList; j++ )
            {
            if ( array[i].CompareF(aExcludeList[j]) == 0 )
                {
                LOGGER_WRITE_1( "match found at pos %d", j);
                exludeFromList = ETrue;
                }
            }
        
        if ( !exludeFromList )
            {
            name.Format(KFormat, &aDir, &array[i]);
            if (!CapUtil::CheckFileType(name, KXmlExtension))
                {
                continue;  // only accept .xml files
                }
            
            capList->List()->Reset();
            capList->List()->ReadFromFileL( iFsSession, name );
            if (capList->FindServiceL(stringList))
                {
                WriteNewLineL();
                WriteFromListL(stringList);
                
                User::LeaveIfError( aFilesAdded.Append(array[i]) );
                LOGGER_WRITE_1( "File '%S' added to list", &array[i] );
                }
            }
        }

    CleanupStack::PopAndDestroy(stringList);
    CleanupStack::PopAndDestroy(&array);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::FolderServiceL()
// Writes folder service. Notice that this service is created in code because
// pathinfo component is used to get folder names.
// -----------------------------------------------------------------------------
//
void CCapInfo::FolderServiceL()
    {
    TRACE_FUNC_ENTRY;
    TBuf<KBufSize> rootPath;
    TBuf<KBufSize> memoryType(KNullDesC);
    TFileName folderName;
    _LIT( KFormat, "%S%S" );
    
    WriteNewLineL();
    WriteTagL(EService, TXmlParser::EElementBegin);

    WriteValueL(EName,  KFolderServiceName);
    WriteValueL(EUUID,  KFolderServiceUid);
    WriteValueL(EVersion,  KFolderServiceVersion);

    WriteTagL(EObject, TXmlParser::EElementBegin);
    WriteValueL(EType,  KFolderServiceType);
    WriteTagL(EObject, TXmlParser::EElementEnd);
    TInt drive;
    TInt err;
    
    CapUtil::GetDefaultRootPathL( iFsSession, rootPath );
    if( rootPath.Length() > 0 )
        {
        err = iFsSession.CharToDrive( rootPath[0], drive );
        if( err == KErrNone )
            {
            CapUtil::GetMemoryType( iFsSession, memoryType, drive );
            }
        }

    // EFolderImages
    folderName.Format( KFormat, &rootPath, &PathInfo::ImagesPath() );
    WriteFolderL( KFolderImages, folderName, memoryType );
    
    // EFolderGraphics
    folderName.Format( KFormat, &rootPath, &PathInfo::ImagesPath() );
    WriteFolderL(KFolderGraphics, folderName, memoryType);
    
    // EFolderTones
    folderName.Format( KFormat, &rootPath, &PathInfo::DigitalSoundsPath() );
    WriteFolderL(KFolderTones, folderName, memoryType);
    
    // EFolderMusic
    folderName.Format( KFormat, &rootPath, &PathInfo::SoundsPath() );
    WriteFolderL(KFolderMusic, folderName, memoryType);
    
    // EFolderVideos
    folderName.Format( KFormat, &rootPath, &PathInfo::VideosPath() );
    WriteFolderL(KFolderVideos, folderName, memoryType);
    
    // EFolderInstalls
    folderName.Format( KFormat, &rootPath, &PathInfo::InstallsPath() );
    WriteFolderL(KFolderInstalls, folderName, memoryType);


    WriteTagL(EService, TXmlParser::EElementEnd);
    WriteNewLineL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::SyncSolutionsServiceL()
// Reads last used syncSolutions from cenrep and writes to capability object
// -----------------------------------------------------------------------------
//
void CCapInfo::SyncSolutionsServiceL()
    {
    TRACE_FUNC_ENTRY;
    TSConSolutionInfo calendar;
    TSConSolutionInfo contact;
    TSConSolutionInfo sms;
    TSConSolutionInfo bookmark;
    
    CSConSyncService *syncService = CSConSyncService::NewLC();
    syncService->GetSolutionL( KSeconCalendarUid, calendar );
    syncService->GetSolutionL( KSeconContactsUid, contact );
    syncService->GetSolutionL( KSeconSmsUid, sms );
    syncService->GetSolutionL( KSeconBookmarkUid, bookmark );
    CleanupStack::PopAndDestroy( syncService );
    
    WriteSyncSolutionsServiceL( calendar, contact, sms, bookmark );
    
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CCapInfo::WriteSyncSolutionsServiceL()
// Writes SyncSolutionsService data to capability object.
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteSyncSolutionsServiceL( const TSConSolutionInfo& aCalendar,
        const TSConSolutionInfo& aContact,
        const TSConSolutionInfo& aSms,
        const TSConSolutionInfo& aBookmark)
    {
    TRACE_FUNC_ENTRY;
    
    WriteTagL( EService, TXmlParser::EElementBegin );

    WriteValueL( EName,  KSyncSolutionsServiceName );
    WriteValueL( EVersion,  KSyncSolutionsServiceVersion );
    
    if ( aCalendar.iUid != 0 )
        {
        WriteSolutionTagL( KSyncCalendar, aCalendar );
        }
    
    if ( aContact.iUid != 0 )
        {
        WriteSolutionTagL( KSyncContacts, aContact );
        }
    
    if ( aSms.iUid != 0 )
        {
        WriteSolutionTagL( KSyncSms, aSms );
        }
    
    if ( aBookmark.iUid != 0 )
        {
        WriteSolutionTagL( KSyncBookmark, aBookmark );
        }
    
    /*
     * <Ext>
     *   <XNam>DesktopSync</XNam>
     *   <XVal></XVal>
     * </Ext>
     * 
     */
    if ( !iSconInfo->iDesktopSyncError )
        {
        WriteTagL( EExt, TXmlParser::EElementBegin );
        WriteValueL( EXNam, KDesktopSync );
        
        TBuf<KBufSize> buffer;
        buffer.AppendNum( iSconInfo->iDesktopSync );
        
        WriteValueL( EXVal, buffer );
        WriteTagL( EExt, TXmlParser::EElementEnd );
        }
    
    
    
    WriteTagL( EService, TXmlParser::EElementEnd );
    TRACE_FUNC_EXIT;
    }



// -----------------------------------------------------------------------------
// CCapInfo::WriteSolutionTagL()
// Writes SyncSolutionsService solution data to capability object.
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteSolutionTagL( const TDesC& aContentName,
        const TSConSolutionInfo& aSolution )
    {
    TRACE_FUNC_ENTRY;
    _LIT( KFormatUID, "UID=0x%08x" );
    _LIT( KFormatName, "Name=%S" );
    _LIT( KFormatDate, "Timestamp=%04d%02d%02dT%02d%02d%02dZ" );
    
    WriteTagL( EExt, TXmlParser::EElementBegin );
    WriteValueL( EXNam, aContentName );
    
    TFileName temp;
    temp.Format( KFormatUID, aSolution.iUid );
    WriteValueL( EXVal, temp );
    
    temp.Format( KFormatName, &aSolution.iSolutionName );
    WriteValueL( EXVal, temp );
    
    if ( aSolution.iTime.Int64() != 0 )
        {
        // write time
        TDateTime time = aSolution.iTime.DateTime();
        temp.Format( KFormatDate, time.Year(), time.Month() + 1,
            time.Day() + 1, time.Hour(), time.Minute(), time.Second() );
        WriteValueL( EXVal, temp );
        
        }
    
    
    WriteTagL( EExt, TXmlParser::EElementEnd );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteFolderL(const TDesC& aType, const TDesC& aFullName, 
// const TDesC& aMemory)
// Writes one folder name element.
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteFolderL(const TDesC& aType, const TDesC& aFullName, 
                            const TDesC& aMemory)
    {
    TRACE_FUNC_ENTRY;
    _LIT(KFormat, "%S=%S");

    TBuf<KBufSize> buf; 
    const TInt KReservedSize = 10;
    if ( aFullName.Length() + KReservedSize > KBufSize )
        {
        return;
        }
    buf.Format(KFormat, &KFolderNameTag, &aFullName);
    

    WriteTagL(EExt, TXmlParser::EElementBegin);

    WriteValueL(EXNam, aType);
    WriteValueL(EXVal, buf);

    buf.Format(KFormat, &KFolderMemoryTypeTag, &aMemory);
    WriteValueL(EXVal, buf);
    

    WriteTagL(EExt, TXmlParser::EElementEnd);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteL(const TDesC& aText)
// Writes one element to capability buffer.
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteL(const TDesC& aText)
    {
    if (aText.Length() > iBuf.MaxLength())
        {
        User::Leave(KErrTooBig);
        }
        

    iBuf=aText;
    iBuf.Trim();
    FormatElement(iBuf);
    TPtr8 ptr = iHeapBuf->Des();

    if ( iBuf.Length()+2 > ptr.MaxLength() )
        {
        User::Leave(KErrTooBig);
        }
        
    //unicode conversion
    HBufC8* convBuf = HBufC8::NewLC( iBuf.Size() );
    TPtr8 convPtr = convBuf->Des();
    
    CnvUtfConverter::ConvertFromUnicodeToUtf8(convPtr, iBuf);

    ptr.Copy(convPtr);
    ptr.Append( KLineFeed );  // linefeed
    
    CleanupStack::PopAndDestroy( convBuf );
    
    TInt pos=iCapabilityBuf->Size();
    iCapabilityBuf->InsertL(pos, ptr);

    iBuf=KNullDesC;
    }

void CCapInfo::WriteNewLineL()
    {
    TInt pos=iCapabilityBuf->Size();
    iCapabilityBuf->InsertL(pos, KLineFeed );
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteValueL(TInt aId, const TDesC& aValue)
// Constructs element with value (eg <Language>en</Language>)
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteValueL(TInt aId, const TDesC& aValue)
    {
    // empty values not written to capability object
    if (aValue.Length()<1)
        {
        LOGGER_WRITE( "WriteValueL: value is empty" );
        return;
        }

    TBuf<KBufSize> buf;

    TRAPD(err, CapParser::MakeElementL(buf, aId, aValue));
    if (err==KErrNone)
        {
        WriteL(buf);
        }
    else if (err==KErrTooBig)
        {
        WriteBigValueL(aId, aValue);
        }
    else
        {
        User::Leave(err);
        }
    
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteBigValueL(TInt aId, const TDesC& aValue)
// Constructs element with value
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteBigValueL(TInt aId, const TDesC& aValue)
    {
    HBufC* hBuf=HBufC::NewL(aValue.Length()+KBufSize);
    CleanupStack::PushL(hBuf);

    TPtr pBuf(hBuf->Des());
    CapParser::MakeElementL(pBuf, aId, aValue);
    WriteL(*hBuf);
    
    CleanupStack::PopAndDestroy(hBuf);
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteValueL(TInt aId, TInt64 aValue)
// Constructs element with integer value
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteValueL(TInt aId, TInt64 aValue)
    {
    TBuf<KBufSize> buf;
    TBuf<KTagSize> value;
    CapUtil::IntToStr(value, aValue);
    CapParser::MakeElementL(buf, aId, value);
    WriteL(buf);
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteTagL(TInt aId, TInt aType)
// Constructs element without value (eg <Memory>)
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteTagL(TInt aId, TInt aType)
    {
    TBuf<KBufSize> buf;
    CapParser::MakeElementL(buf, aId, aType);
    WriteL(buf);
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteAttributeL(TInt aId, const TDesC& aVersion, const TDesC& aDate)
// Constructs element with attributes "Version" and "Date" 
// (eg <HW Version="1.5"/>)
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteAttributeL(TInt aId, const TDesC& aVersion, 
                                const TDesC& aDate)
    {
    // empty values not written to capability object
    if (aVersion.Length()<1 && aDate.Length()<1)
        {
        LOGGER_WRITE( "WriteAttributeL: attributes are empty" );
        return;
        }
    
    TBuf<KBufSize> buf;
    CapParser::MakeElementL(buf, aId, aVersion, aDate);
    WriteL(buf);
    }

// -----------------------------------------------------------------------------
// CCapInfo::WriteFromListL(CStringList* aList)
// Writes elements from list to capability buffer.
// -----------------------------------------------------------------------------
//
void CCapInfo::WriteFromListL(CStringList* aList)
    {
    TInt count=aList->Count();

    for (TInt i=0; i<count; i++)
        {
        WriteL(aList->ReadPtr(i));
        }
    }   

// -----------------------------------------------------------------------------
// CCapInfo::FormatElement(TDes& aText)
// Function adds indentation spaces to capability elements to improve 
// readability.
// -----------------------------------------------------------------------------
//
void CCapInfo::FormatElement(TDes& aText)
    {
    TXmlParser parser;

    parser.Set(aText);
    TInt type=parser.Type();
    TBuf<KTagSize> tag;
    parser.GetTag(tag);
        
    if (type==TXmlParser::EElementEnd)
        {
        TInt id1=CapParser::ElementId(tag);
        TInt id2=iIdStack.Pop();
        
        __ASSERT_DEBUG(id1==id2, CapUtil::Panic(KErrGeneral));
        id1=0;
        id2=0;
        }

    AddFormatText(aText, iIdStack.Size());

    if (type==TXmlParser::EElementBegin)
        {
        TInt id1=CapParser::ElementId(tag);
        iIdStack.Push(id1);
        }
    }

// -----------------------------------------------------------------------------
// CCapInfo::AddFormatText( TDes& aText, TInt aNum ) const
// Adds format text
// -----------------------------------------------------------------------------
//
void CCapInfo::AddFormatText( TDes& aText, TInt aNum ) const
    {
    TBuf<KBufSize> buf;
    buf = KNullDesC;

    if ( aNum > KNestingLimit )
        {
        aNum = KNestingLimit;
        }
        
    for ( TInt i=0; i<aNum; i++ )
        {
        buf.Append( KFormatText );
        }
    if ( aText.MaxLength()-aText.Length()>buf.Length() )
        {
        aText.Insert( 0, buf );
        }
    }
    
// End of file


