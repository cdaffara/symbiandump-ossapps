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
* Description:  CapUtil implementation
*
*/


// INCLUDE FILES

#include <pathinfo.h>
#include <sysutil.h>
#include <hal.h>
#include <hal_data.h>
#include <etelmm.h>      // for etel
#include <mmtsy_names.h> // for etel
#include <utf.h>
#include <eikenv.h>
#include <driveinfo.h>
#include <centralrepository.h>
#include <sysutildomaincrkeys.h>

#include "caputils.h"
#include "capability.h"
#include "debug.h"

const TInt KPackageSize = 65536;
const TInt KDefaultArrayGranularity = 5;
// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CapUtil::GetDefaultRootPathL( RFs& aFs, TDes& aRootPath )
// Gets default root path
// -----------------------------------------------------------------------------
//
void CapUtil::GetDefaultRootPathL( RFs& aFs, TDes& aRootPath )
    {
    TRACE_FUNC_ENTRY;
    // Use the default mass storage if it is internal drive
    TInt msDrive;
    User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, msDrive ) );
    LOGGER_WRITE_1("drive: %d", msDrive);
    
    TUint msStatus( 0 );
    TInt err = DriveInfo::GetDriveStatus( aFs, msDrive, msStatus );
    LOGGER_WRITE_1("DriveInfo::GetDriveStatus err: %d", err);
    
    // if no errors, also check drive status
    if( !err && !( msStatus & DriveInfo::EDrivePresent )
        || msStatus & DriveInfo::EDriveCorrupt )
        {
        LOGGER_WRITE( "Internal mass storage not present or corrupted" );
        err = KErrNotFound;
        }
    
    if ( !err && ( msStatus & DriveInfo::EDriveInternal ) )
        {
        // Use internal mass storage
        LOGGER_WRITE( "Use internal mass storage" );
        User::LeaveIfError( PathInfo::GetRootPath( aRootPath, msDrive ) );
        }
    else
        {
        // Use phone memory
        LOGGER_WRITE( "Use phone memory" );
        StrCopy( aRootPath, PathInfo::PhoneMemoryRootPath() );
        }
    LOGGER_WRITE_1( "rootPath: %S", &aRootPath );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::GetMemoryType( RFs& aFs, TDes& aMemoryType, const TInt aDrive )
// Gets drive memory type
// -----------------------------------------------------------------------------
//
void CapUtil::GetMemoryType( RFs& aFs, TDes& aMemoryType, const TInt aDrive )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aDrive: %d", aDrive);
    aMemoryType = KNullDesC;
    TUint driveStatus;
    TInt err = DriveInfo::GetDriveStatus( aFs, aDrive, driveStatus );
    if( err )
        {
        LOGGER_WRITE_1( "CapUtil::GetMemoryType() : DriveInfo::GetDriveStatus err: %d", err );
        return;
        }
    // search internal or removable drives
    TBool searchInternalDrives;
    if( driveStatus & DriveInfo::EDriveInternal )
        {
        aMemoryType = KMediaFlash;
        searchInternalDrives = ETrue;
        }
    else
        {
        aMemoryType = KMediaMMC;
        searchInternalDrives = EFalse;
        }
    
    TInt typeNumber(0);
    TInt driveCount;
    TDriveList driveList;
    
    err = DriveInfo::GetUserVisibleDrives( aFs, driveList, driveCount );
    if( err )
        {
        LOGGER_WRITE_1( "CapUtil::GetMemoryType() : DriveInfo::GetUserVisibleDrives err: %d", err );
        return;
        }
    
    for( TInt i = EDriveA; i <= aDrive; i++ )
        {
        if( driveList[i] )
            {
            TUint driveStatus;
            err = DriveInfo::GetDriveStatus( aFs, i, driveStatus );
            if( err )
                {
                LOGGER_WRITE_1( "CapUtil::GetMemoryType() : DriveInfo::GetDriveStatus err: %d", err );
                continue;
                }

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
                    typeNumber++;
                    }
                }
            else if( driveStatus & DriveInfo::EDriveRemovable )
                {
                if( !searchInternalDrives )
                    {
                    typeNumber++;
                    }
                }
            }
        }
        
    if( typeNumber > 1 )
        {
        aMemoryType.AppendNum( typeNumber );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::GetOperatornameL(TDes& aLongName, TDes& aCountryCode, TDes& aNetworkID)
// Gets phone operator name, country code, networkID.
// -----------------------------------------------------------------------------
//
void CapUtil::GetOperatorNameL(TDes& aLongName, TDes& aCountryCode, TDes& aNetworkID)
    {
    TRACE_FUNC_ENTRY;
    TUint32 networkCaps;
    RTelServer::TPhoneInfo phoneInfo;
    
    RTelServer server;
    RMobilePhone mobilePhone;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );
    
    TInt numPhones;
    User::LeaveIfError( server.EnumeratePhones( numPhones ) );
    server.GetPhoneInfo( 0, phoneInfo );
    
    User::LeaveIfError( mobilePhone.Open( server, phoneInfo.iName ) );
    CleanupClosePushL( mobilePhone );
    User::LeaveIfError( mobilePhone.GetNetworkCaps( networkCaps ) );
    
    RMobilePhone::TMobilePhoneNetworkInfoV1 mobilePhoneNetworkInfo;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg mobilePhoneNetworkInfoPckg(
        mobilePhoneNetworkInfo );
    RMobilePhone::TMobilePhoneLocationAreaV1 mobilePhoneLocationArea;
    
    if (networkCaps & RMobilePhone::KCapsGetCurrentNetwork)
        {
        TRequestStatus status;
        mobilePhone.GetCurrentNetwork( 
            status, mobilePhoneNetworkInfoPckg, mobilePhoneLocationArea );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        }
    
    CleanupStack::PopAndDestroy( &mobilePhone );
    CleanupStack::PopAndDestroy( &server );
    if ( mobilePhoneNetworkInfo.iLongName.Length() > 0 )
        {
        StrCopy( aLongName, mobilePhoneNetworkInfo.iLongName );
        }
    else if ( mobilePhoneNetworkInfo.iShortName.Length() > 0 )
        {
        StrCopy( aLongName, mobilePhoneNetworkInfo.iShortName );
        }
    else
        {
        StrCopy( aLongName, mobilePhoneNetworkInfo.iDisplayTag );
        }
    StrCopy( aCountryCode, mobilePhoneNetworkInfo.iCountryCode );
    StrCopy( aNetworkID, mobilePhoneNetworkInfo.iNetworkId );
    
    TRACE_FUNC_EXIT;
    }
// -----------------------------------------------------------------------------
// CapUtil::GetManufacturer(TDes& aText)
// Gets phone manufacturer from HAL. In case manufacturer is not known,
// -----------------------------------------------------------------------------
//
void CapUtil::GetManufacturer(TDes& aText)
    {
    TRACE_FUNC_ENTRY;
    TInt num(KErrNotFound);
    
    HAL::Get(HALData::EManufacturer, num);
    
    switch (num)
        {
        case HALData::EManufacturer_Ericsson:
            aText=KManufacturerEricsson; 
            break;
        case HALData::EManufacturer_Motorola:
            aText=KManufacturerMotorola; 
            break;
        case HALData::EManufacturer_Nokia:
            aText=KManufacturerNokia; 
            break;
        case HALData::EManufacturer_Panasonic:
            aText=KManufacturerPanasonic; 
            break;
        case HALData::EManufacturer_Psion:
            aText=KManufacturerPsion; 
            break;
        case HALData::EManufacturer_Intel:
            aText=KManufacturerIntel; 
            break;
        case HALData::EManufacturer_Cogent:
            aText=KManufacturerCogent; 
            break;
        case HALData::EManufacturer_Cirrus:
            aText=KManufacturerCirrus; 
            break;
        case HALData::EManufacturer_Linkup:
            aText=KManufacturerLinkup; 
            break;
        case HALData::EManufacturer_TexasInstruments:
            aText=KManufacturerTexasInstruments; 
            break;
        default: 
            aText=KNullDesC; 
            break;
        }

    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::GetLanguage(TDes& aText)
// Gets language
// -----------------------------------------------------------------------------
//
void CapUtil::GetLanguage(TDes& aText)
    {
    TRACE_FUNC_ENTRY;
    TLanguage lang=User::Language();
    GetLanguageString(lang, aText);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::GetSWVersionL(TDes& aVersion, TDes& aDate, TDes& aModel)
// Gets SW version, SW version date  and device model from SysUtil. 
// -----------------------------------------------------------------------------
//
void CapUtil::GetSWVersionL(TDes& aVersion, TDes& aDate, TDes& aModel)
    {
    TRACE_FUNC_ENTRY;
    TBuf<KBufSize> buf;
    aVersion=KNullDesC;
    aDate=KNullDesC;
    aModel=KNullDesC;

    User::LeaveIfError( SysUtil::GetSWVersion( buf ) );
    RArray<TPtrC> arr(KDefaultArrayGranularity);
    CleanupClosePushL( arr );

    CapUtil::SplitL(buf, '\n', arr);
    const TInt KFieldsToFind = 3;
    if ( arr.Count() < KFieldsToFind )
        {
        User::Leave( KErrNotFound );
        }
    
    StrCopy( aVersion, arr[0] );
    aVersion.Trim();

    StrCopy(aModel,arr[2]);
    aModel.Trim();

    TBuf<KTagSize> date;
    StrCopy(date, arr[1]);
    TTime t;
    t.UniversalTime(); // this is to avoid warnings
    TRAPD( err, t = ParseDateL( date ) );
    if ( err == KErrNone )
        {
        CapabilityDate( aDate, t );
        }
    
    CleanupStack::PopAndDestroy( &arr );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::InitMemoryInfo(TMemoryInfo& aInfo)
// Initializes TMemoryInfo.
// -----------------------------------------------------------------------------
//
void CapUtil::InitMemoryInfo(TMemoryInfo& aInfo)
    {
    aInfo.iDriveNum        = KErrNotFound;
    aInfo.iDriveLetter     = KNoChar;
    aInfo.iLocation        = KNullDesC;
    aInfo.iFree            = KErrNotFound;
    aInfo.iUsed            = KErrNotFound;
    aInfo.iShared          = EFalse;
    aInfo.iFileSize        = KErrNotFound;
    aInfo.iFolderSize      = KErrNotFound;
    aInfo.iFileNameSize    = KMaxFileName;    // TFileName 256
    aInfo.iFolderNameSize  = KMaxFileName;    // TFileName 256
    aInfo.iCaseSensitivity = EFalse;
    }

// -----------------------------------------------------------------------------
// CapUtil::GetMemoryInfoL( const RFs& aFs, TInt aDriveNumber, TMemoryInfo& aInfo )
// Get memory information for one drive.
// -----------------------------------------------------------------------------
//
void CapUtil::GetMemoryInfoL( const RFs& aFs, const TInt aDriveNumber, TMemoryInfo& aInfo )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("  aDriveNumber: %d", aDriveNumber);
    
    TVolumeInfo volumeInfo;
    TDriveInfo driveInfo;

    InitMemoryInfo( aInfo );

    User::LeaveIfError( aFs.Drive(driveInfo, aDriveNumber) ); 
    if ( driveInfo.iDriveAtt == (TUint)KDriveAbsent ) 
        {
        LOGGER_WRITE(" iDriveAtt == (TUint)KDriveAbsent, Leave KErrNotFound");
        User::Leave( KErrNotFound );
        }
                
    User::LeaveIfError( aFs.Volume(volumeInfo, aDriveNumber) );

    User::LeaveIfError( aFs.DriveToChar(aDriveNumber, aInfo.iDriveLetter) );
    aInfo.iDriveNum = aDriveNumber;
    aInfo.iLocation.Append( aInfo.iDriveLetter );
    aInfo.iLocation.Append( KDriveDelimiter );
    aInfo.iLocation.Append( KPathDelimiter );
    aInfo.iFree = volumeInfo.iFree;
    aInfo.iUsed = volumeInfo.iSize - volumeInfo.iFree;
    
    // set free memory up to critical level for all drives
    CRepository* repository = CRepository::NewLC( KCRUidDiskLevel );
    TInt criticalLevel(0);
    User::LeaveIfError( repository->Get( KDiskCriticalThreshold, criticalLevel ) );
    CleanupStack::PopAndDestroy( repository );
    criticalLevel += KPackageSize; // add obex package size to critical level
    LOGGER_WRITE_1( "CapUtil::GetMemoryInfoL() criticalLevel: %d", criticalLevel ) ;
    if ( aInfo.iFree > criticalLevel )
        {
        aInfo.iFree = aInfo.iFree - criticalLevel;
        }       
    else
        {
        aInfo.iFree = 0;
        }
    
    aInfo.iFileSize = aInfo.iFree;
    aInfo.iFolderSize = aInfo.iFree;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::GetFileListL( RFs& aFs, const TDesC& aDir, 
// RArray<TFileName>& aList)
// Finds all files in aDir.
// -----------------------------------------------------------------------------
//
void CapUtil::GetFileListL( const RFs& aFs, const TDesC& aDir, 
                            RArray<TFileName>& aList)
    {
    TRACE_FUNC_ENTRY;
    
    aList.Reset();
    
    CDir* dir( NULL );
    User::LeaveIfError( aFs.GetDir( aDir, KEntryAttMatchMask, ESortByName, dir ) );
    CleanupStack::PushL( dir );
    
    for ( TInt i=0; i < dir->Count(); ++i )
        {
        TEntry entry = (*dir)[i];
        if ( !entry.IsDir() )
            {
            User::LeaveIfError( aList.Append( entry.iName ) );
            }
        }
    CleanupStack::PopAndDestroy( dir );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::CheckFileType(const TDesC& aFile, const TDesC& aExt)
// Function checks file extension.
// -----------------------------------------------------------------------------
//
TBool CapUtil::CheckFileType(const TDesC& aFile, const TDesC& aExt)
    {
    TRACE_FUNC_ENTRY;
    TParse parse;
    parse.Set(aFile, NULL, NULL);
    TPtrC ptr=parse.Ext();

    TBuf<KTagSize> buf1;
    TBuf<KTagSize> buf2;
    
    buf1=ptr;
    buf2=aExt;
    
    buf1.LowerCase();
    buf2.LowerCase();

    if (buf1.Compare(buf2)==0)
        {
        LOGGER_WRITE( "CapUtil::CheckFileType(const TDesC& aFile, const TDesC& aExt) : returned ETrue" );
        return ETrue;
        }
        
    else
        {
        LOGGER_WRITE( "CapUtil::CheckFileType(const TDesC& aFile, const TDesC& aExt) : returned EFalse" );
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CapUtil::GetLanguageString(TLanguage aId, TDes& aText)
// Get language string for aId.
// -----------------------------------------------------------------------------
//
void CapUtil::GetLanguageString(TLanguage aId, TDes& aText)
    {
    TRACE_FUNC_ENTRY;
    aText=KNullDesC;
    
    TInt count=NUMLANGSTRINGS;
    for (TInt i=0; i<count; i++)
        {
        TLangStringStruct t=KLangStrings[i];
        if (t.id == (TInt)aId)
            {
            aText=t.lang;
            return;
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::Panic(TInt aReason)
// Panic.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CapUtil::Panic(TInt aReason)
#else
void CapUtil::Panic(TInt /*aReason*/)
#endif
    {
    TRACE_FUNC_ENTRY;
#ifdef _DEBUG
    _LIT(KPanicCategory,"CapabilitySC");

    User::Panic(KPanicCategory, aReason);
#endif
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::StrCopy(TDes& aTarget, const TDesC& aSource)
// String copy with lenght check.
// -----------------------------------------------------------------------------
//
void CapUtil::StrCopy(TDes& aTarget, const TDesC& aSource)
    {
    TInt len=aTarget.MaxLength();
    if(len<aSource.Length()) 
        {
        aTarget.Copy(aSource.Left(len));
        return;
        }
    aTarget.Copy(aSource);
    }

// -----------------------------------------------------------------------------
// CapUtil::IntToStr(TDes& aText, TInt64 aNum)
// Function converts ínteger to string.
// -----------------------------------------------------------------------------
//
void CapUtil::IntToStr(TDes& aText, TInt64 aNum)
    {
    aText.Num(aNum);
    }

// -----------------------------------------------------------------------------
// CapUtil::StrToInt(const TDesC& aText, TInt& aNum)
// Function converts string to integer. If string cannot be converted,
// error code is returned.
// -----------------------------------------------------------------------------
//
TInt CapUtil::StrToInt(const TDesC& aText, TInt& aNum)
    {
    TLex lex(aText);

    TInt err=lex.Val(aNum); 
    return err;
    }

// -----------------------------------------------------------------------------
// CapUtil::SplitL(const TDesC& aText, const TChar aSeparator, 
// RArray<TPtrC>& aArray)
// Function splits string (eg "name1, name2, name3") into substrings.
// -----------------------------------------------------------------------------
//
void CapUtil::SplitL(const TDesC& aText, const TChar aSeparator, 
                    RArray<TPtrC>& aArray)
    {
    TRACE_FUNC_ENTRY;
    TPtrC ptr;
    ptr.Set(aText);

    for (;;)
        {
        TInt pos=ptr.Locate(aSeparator);
        if (pos==KErrNotFound)
            {
            aArray.AppendL(ptr);
            break;
            }

        TPtrC subStr=ptr.Left(pos); // get pos characters starting from position 0
        aArray.AppendL(subStr);

        if (!(ptr.Length()>pos+1))
            {
            break;
            }
            
        ptr.Set(ptr.Mid(pos+1));// get all characters starting from position pos+1
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CapUtil::ParseDateL(const TDesC& aText)
// Function parses date string of the format "dd-mm-yy".
// -----------------------------------------------------------------------------
//
TTime CapUtil::ParseDateL(const TDesC& aText)
    {
    TRACE_FUNC_ENTRY;
    RArray<TPtrC> arr(KDefaultArrayGranularity);
    CleanupClosePushL( arr );

    CapUtil::SplitL( aText, '-', arr );
    if ( arr.Count() != 3 )
        {
        User::Leave( KErrNotSupported );
        }
           
    TInt day; 
    TInt month; 
    TInt year;
    
    User::LeaveIfError( StrToInt(arr[0], day) );
    User::LeaveIfError( StrToInt(arr[1], month) );
    User::LeaveIfError( StrToInt(arr[2], year) );

    TDateTime td;
    TMonth month2 = Month( month );
    // if year is defined as two digit, add currect millenium for it
    const TInt KDefaultMillenium = 2000;
    if ( year < KDefaultMillenium )
        {
        year = year + KDefaultMillenium;
        }
        

    User::LeaveIfError( td.Set(year, month2, day-1, 0, 0, 0, 0) );

    CleanupStack::PopAndDestroy( &arr );
    
    TTime t(td);
    TRACE_FUNC_EXIT;
    return t;
}

// -----------------------------------------------------------------------------
// CapUtil::Month(TInt aNum)
// Function return TMonth presentation of integer
// -----------------------------------------------------------------------------
//
TMonth CapUtil::Month(TInt aNum)
    {
    TRACE_FUNC_ENTRY;
    __ASSERT_DEBUG(aNum>=1 && aNum<=12, Panic(KErrArgument));

    switch (aNum)
        {
        case 1: return EJanuary;
        case 2: return EFebruary;
        case 3: return EMarch;
        case 4: return EApril;
        case 5: return EMay;
        case 6: return EJune;
        case 7: return EJuly;
        case 8: return EAugust;
        case 9: return ESeptember;
        case 10: return EOctober;
        case 11: return ENovember;
        case 12: return EDecember;
        default: return EJanuary;
        }
    }

// -----------------------------------------------------------------------------
// CapUtil::CapabilityDate(TDes& aText, const TTime aTime)
// Constructs capability date as string
// -----------------------------------------------------------------------------
//
void CapUtil::CapabilityDate(TDes& aText, const TTime aTime)
    {
    TRACE_FUNC_ENTRY;
    _LIT(KFormat,"%04d%02d%02dT%02d%02d%02dZ");

    TDateTime dt=aTime.DateTime();
    aText.Format(KFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(),
     dt.Minute(), dt.Second());
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// TIdStack::Pop()
// Pop id from the stack
// -----------------------------------------------------------------------------
//
TInt TIdStack::Pop()
    {
    if (iPos<0)
        {
        LOGGER_WRITE( "TIdStack::Pop() returned KErrNotFound" );
        return KErrNotFound;
        }
        
    TInt id=iArray[iPos];
    iPos--;
    return id;
    }

// -----------------------------------------------------------------------------
// TIdStack::Push( TInt aId )
// Push id to the stack
// -----------------------------------------------------------------------------
//
void TIdStack::Push( TInt aId )
    {
    if ( Size() >= KNestingLimit )
        {
        return;
        }
        
    iPos++;
    iArray[iPos] = aId;
    }

// -----------------------------------------------------------------------------
// TIdStack::Size() const
// Size of the stack
// -----------------------------------------------------------------------------
//
TInt TIdStack::Size() const
    {
    return iPos+1;
    }

// -----------------------------------------------------------------------------
// TIdStack::Reset()
// Reset the stack.
// -----------------------------------------------------------------------------
//
void TIdStack::Reset()
    {
    iArray.Reset();
    iPos=-1;
    }

// End of file
