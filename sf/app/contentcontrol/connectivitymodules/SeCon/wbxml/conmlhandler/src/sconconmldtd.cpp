/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/


#include "sconconmldtd.h"
#include "sconconmltags.h"
#include "sconconmlhandler.h"
#include "sconconmlhandlererror.h"
#include "debug.h"

// -----------------------------------------------------------------------------
// Macros
// -----------------------------------------------------------------------------
#define CHECK_NO_DUPLICATE(a) if( a ) { User::Leave( KWBXMLParserErrorinvalidDocument ); }

// -----------------------------------------------------------------------------
// pcdata_s
// -----------------------------------------------------------------------------
pcdata_s::pcdata_s( const pcdataType_t aType, const pcdataExtension_t aExt )
	: contentType(aType), extension(aExt)
    {
    }

pcdata_s::pcdata_s() : contentType(SML_PCDATA_OPAQUE), 
                       extension(SML_EXT_UNDEFINED)
    {
    }

pcdata_s::~pcdata_s()
    {
    FreeContent();
    }

void pcdata_s::FreeContent()
    {
    User::Free(content);
    content = 0;
    }

void pcdata_s::SetDataL( const TDesC8& aData )
    {
    FreeContent();
    length = aData.Length();
    content = User::AllocL(length);
    Mem::Copy(content, aData.Ptr(), length);
    }

TPtrC8 pcdata_s::Data() const
    {
    return TPtrC8((TUint8*)content, length);
    }

void pcdata_s::TruncateL( TInt aConsumed )
    {
    HBufC8* buffer = TPtrC8((TUint8*)content + aConsumed, 
                             length - aConsumed).AllocLC();
    SetDataL(*buffer);
    CleanupStack::PopAndDestroy(); // buffer
    }

pcdata_list_s::pcdata_list_s()
    {
    }

pcdata_list_s::~pcdata_list_s()
    {
    delete data;
    delete next;
    }

// -----------------------------------------------------------------------------
// AddDriveL
// -----------------------------------------------------------------------------
CXMLElement* AddDriveL(  ConML_DriveListPtr_t* aList )
    {
    ConML_DriveListPtr_t itemL = new ( ELeave ) ConML_DriveList_t();
    CleanupStack::PushL( itemL );
    itemL->data = new ( ELeave ) ConML_Drive_t();
    GenericListAddL(aList, itemL);
    CleanupStack::Pop(); // itemL
    return itemL->data;
    }

// -----------------------------------------------------------------------------
// AddSIDL
// -----------------------------------------------------------------------------
CXMLElement* AddSIDL( ConML_SIDListPtr_t * aList )
    {
    ConML_SIDListPtr_t itemL = new ( ELeave ) ConML_SIDList_t();
    CleanupStack::PushL( itemL );
    itemL->data = new ( ELeave ) ConML_SID_t();
    GenericListAddL(aList, itemL);
    CleanupStack::Pop(); // itemL
    return itemL->data;
    }
		
// -----------------------------------------------------------------------------
// AddFileL
// -----------------------------------------------------------------------------
CXMLElement* AddFileL( ConML_FileListPtr_t * aList )
    {
    ConML_FileListPtr_t itemL = new ( ELeave ) ConML_FileList_t();
    CleanupStack::PushL( itemL );
    itemL->data = new ( ELeave ) ConML_File_t();
    GenericListAddL(aList, itemL);
    CleanupStack::Pop(); // itemL
    return itemL->data;
    }

// -----------------------------------------------------------------------------
// AddApplicationL
// -----------------------------------------------------------------------------
CXMLElement* AddApplicationL( ConML_ApplicationListPtr_t * aList )
    {   
    ConML_ApplicationListPtr_t itemL = new ( ELeave ) ConML_ApplicationList_t();
    CleanupStack::PushL( itemL );
    itemL->data = new ( ELeave ) ConML_Application_t();
    GenericListAddL(aList, itemL);
    CleanupStack::Pop(); // itemL
    return itemL->data;
    }

// -----------------------------------------------------------------------------
// AddTaskL
// -----------------------------------------------------------------------------
CXMLElement* AddTaskL( ConML_TaskListPtr_t * aList )
    {
    ConML_TaskListPtr_t itemL = new ( ELeave ) ConML_TaskList_t();
    CleanupStack::PushL( itemL );
    itemL->data = new ( ELeave ) ConML_Task_t();
    GenericListAddL(aList, itemL);
    CleanupStack::Pop(); // itemL
    return itemL->data;
    }
	
// -----------------------------------------------------------------------------
// AddParamL
// -----------------------------------------------------------------------------
CXMLElement* AddParamL( ConML_ParamListPtr_t * aList )
    {
    ConML_ParamListPtr_t itemL = new ( ELeave ) ConML_ParamList_t();
    CleanupStack::PushL( itemL );
    itemL->data = new ( ELeave ) ConML_Param_t();
    GenericListAddL(aList, itemL);
    CleanupStack::Pop(); // itemL
    return itemL->data;
    }
	
// -----------------------------------------------------------------------------
// ConML_Drive_s
// -----------------------------------------------------------------------------
ConML_Drive_s::ConML_Drive_s()
    {
    }

ConML_Drive_s::~ConML_Drive_s()
    {
    delete name;
    }

CXMLElement* ConML_Drive_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {
    switch ( aTag )
        {
        case EConMLName:
            CHECK_NO_DUPLICATE ( name );
            name = new ( ELeave ) pcdata_t();
            return name;
				
        default:
            LOGGER_WRITE_1( "ConML_Drive_s::BeginElement() : Unknown element  %02x", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_DriveList_s
// -----------------------------------------------------------------------------
ConML_DriveList_s::ConML_DriveList_s()	
    {
    }

ConML_DriveList_s::~ConML_DriveList_s()	
    {
    delete data;
    delete next;
    }

// -----------------------------------------------------------------------------
// ConML_Drives_s
// -----------------------------------------------------------------------------
ConML_Drives_s::ConML_Drives_s()
    {
    }
	
ConML_Drives_s::~ConML_Drives_s()
    {
    delete drive;
    }
	
CXMLElement* ConML_Drives_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {
    switch ( aTag )
        {
        case EConMLDrive:
            return AddDriveL ( &drive );
	
        default:
            LOGGER_WRITE_1( "ConML_Drives_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_BUROptions_s
// -----------------------------------------------------------------------------
ConML_BUROptions_s::ConML_BUROptions_s()
    {
    }

ConML_BUROptions_s::~ConML_BUROptions_s()
    {
    delete requiresReboot;
    delete hasFiles;
    delete supportsInc;
    delete delayToPrepareData;
    }

CXMLElement* ConML_BUROptions_s::BeginElementL ( 
        TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {
    switch ( aTag )
        {
        case EConMLRequiresReboot:
            CHECK_NO_DUPLICATE ( requiresReboot );
            requiresReboot = new ( ELeave ) pcdata_t();
            return requiresReboot;
		
        case EConMLHasFiles:
            CHECK_NO_DUPLICATE ( hasFiles );
            hasFiles = new ( ELeave ) pcdata_t();
            return hasFiles;
			
        case EConMLSupportsInc:
            CHECK_NO_DUPLICATE ( supportsInc );
            supportsInc = new ( ELeave ) pcdata_t();
            return supportsInc;
		
        case EConMLDelayToPrepareData:
            CHECK_NO_DUPLICATE ( delayToPrepareData );
            delayToPrepareData = new ( ELeave ) pcdata_t();
            return delayToPrepareData;
				
        default:
            LOGGER_WRITE_1( "ConML_BUROptions_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_PackageInfo_s
// -----------------------------------------------------------------------------
ConML_PackageInfo_s::ConML_PackageInfo_s()
    {
    }

ConML_PackageInfo_s::~ConML_PackageInfo_s()
    {
    delete name;
    }

CXMLElement* ConML_PackageInfo_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLName:
            CHECK_NO_DUPLICATE ( name )
            name = new ( ELeave ) pcdata_t();
            return name;
								
        default:
            LOGGER_WRITE_1( "ConML_PackageInfo_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// ConML_SID_s
// -----------------------------------------------------------------------------
ConML_SID_s::ConML_SID_s()
    {
    }

ConML_SID_s::~ConML_SID_s()
    {
    delete type;
    delete uid;
    delete drives;
    delete packageInfo;
    delete burOptions;
    delete size;
    delete transferDataType;
    delete data;
    delete moreData;
    delete dataOwnerStatus;
    }

CXMLElement* ConML_SID_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLType:
            CHECK_NO_DUPLICATE ( type );
            type = new ( ELeave ) pcdata_t();
            return type;
		
        case EConMLUID:
            CHECK_NO_DUPLICATE ( uid )
            uid = new ( ELeave ) pcdata_t();
            return uid;
		
        case EConMLDrives:
            drives = new ( ELeave ) ConML_Drives_t();
            return drives;
		
        case EConMLPackageInfo:
            CHECK_NO_DUPLICATE ( packageInfo );
            packageInfo = new ( ELeave ) ConML_PackageInfo_t();
            return packageInfo;
		
        case EConMLBUROptions:
            CHECK_NO_DUPLICATE ( burOptions )
            burOptions = new ( ELeave )	ConML_BUROptions_t();
            return burOptions;
			
        case EConMLSize:
            CHECK_NO_DUPLICATE ( size );
            size = new ( ELeave ) pcdata_t();
            return size;
		
        case EConMLTransferDataType:
            CHECK_NO_DUPLICATE ( transferDataType );
            transferDataType = new ( ELeave ) pcdata_t();
            return transferDataType;
		
        case EConMLData:
            CHECK_NO_DUPLICATE ( data );
            data = new ( ELeave ) pcdata_t();
            return data;
		
        case EConMLMoreData:
            CHECK_NO_DUPLICATE ( moreData );
            moreData = new ( ELeave ) pcdata_t();
            return moreData;
		
        case EConMLDataOwnerStatus:
            CHECK_NO_DUPLICATE ( dataOwnerStatus );
            dataOwnerStatus = new ( ELeave ) pcdata_t();
            return dataOwnerStatus;
		
        default:
            LOGGER_WRITE_1( "ConML_SID_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_SIDList_s
// -----------------------------------------------------------------------------
ConML_SIDList_s::ConML_SIDList_s()
    {
    }
	
ConML_SIDList_s::~ConML_SIDList_s()
    {
    delete data;
    delete next;
    }
	
// -----------------------------------------------------------------------------
// ConML_DataOwners_s
// -----------------------------------------------------------------------------
ConML_DataOwners_s::ConML_DataOwners_s()
    {
    }

ConML_DataOwners_s::~ConML_DataOwners_s()
    {
    delete sid;
    }

CXMLElement* ConML_DataOwners_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLSID:
            return AddSIDL( &sid );
				
        default:
            LOGGER_WRITE_1( "ConML_DataOwners_s::BeginElement()\
             : Unknown element  %02x ", aTag);
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_Application_s
// -----------------------------------------------------------------------------
ConML_Application_s::ConML_Application_s()
    {
    }
	
ConML_Application_s::~ConML_Application_s()
    {
    delete name;
    delete uid;
    }

CXMLElement* ConML_Application_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
    	{
    	case EConMLName:
            CHECK_NO_DUPLICATE ( name );
            name = new ( ELeave ) pcdata_t();
            return name;
		
        case EConMLUID:
            CHECK_NO_DUPLICATE ( uid );
            uid = new ( ELeave ) pcdata_t();
            return uid;
		
        default:
            LOGGER_WRITE_1( "ConML_Application_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_ApplicationList_s
// -----------------------------------------------------------------------------
ConML_ApplicationList_s::ConML_ApplicationList_s()
    {
    }

ConML_ApplicationList_s::~ConML_ApplicationList_s()
    {
    delete data;
    delete next;
    }
	
// -----------------------------------------------------------------------------
// ConML_Applications_s
// -----------------------------------------------------------------------------
ConML_Applications_s::ConML_Applications_s()	
    {
    }

ConML_Applications_s::~ConML_Applications_s()	
    {
    delete application;
    }
	
CXMLElement* ConML_Applications_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLApplication:
            return AddApplicationL( &application );
				
        default:
            LOGGER_WRITE_1( "ConML_DataOwners_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_SupportedMethod_s
// -----------------------------------------------------------------------------
ConML_SupportedMethods_s::ConML_SupportedMethods_s()
    {
    }

ConML_SupportedMethods_s::~ConML_SupportedMethods_s()
    {
    delete install;
    delete unInstall;
    delete setInstParams;
    delete listInstalledApps;
    delete listDataOwners;
    delete setBurMode;
    delete getDataSize;
    delete requestData;
    delete supplyData;
    delete reboot;
    }

CXMLElement* ConML_SupportedMethods_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {
    switch ( aTag )
        {
        case EConMLInstall:
            CHECK_NO_DUPLICATE ( install );
            install = new ( ELeave ) pcdata_t();
            return install;
		
        case EConMLUnInstall:
            CHECK_NO_DUPLICATE ( unInstall );
            unInstall = new ( ELeave ) pcdata_t();
            return unInstall;
	
        case EConMLListInstalledApps:
            CHECK_NO_DUPLICATE ( listInstalledApps );
            listInstalledApps = new ( ELeave ) pcdata_t();
            return listInstalledApps;
		
        case EConMLListDataOwners:
            CHECK_NO_DUPLICATE ( listDataOwners );
            listDataOwners = new ( ELeave ) pcdata_t();
            return listDataOwners;
		
        case EConMLSetBURMode:
            CHECK_NO_DUPLICATE ( setBurMode );
            setBurMode = new ( ELeave ) pcdata_t();
            return setBurMode;
		
        case EConMLGetDataSize:
            CHECK_NO_DUPLICATE ( getDataSize );
            getDataSize = new ( ELeave ) pcdata_t();
            return getDataSize;
		
        case EConMLRequestData:
            CHECK_NO_DUPLICATE ( requestData );
            requestData = new ( ELeave ) pcdata_t();
            return requestData;
		
        case EConMLSupplyData:
            CHECK_NO_DUPLICATE (supplyData );
            supplyData = new ( ELeave ) pcdata_t();
            return supplyData;
		
        case EConMLReboot:
            CHECK_NO_DUPLICATE ( reboot );
            reboot = new ( ELeave ) pcdata_t();
            return reboot;
		
        default:
            LOGGER_WRITE_1( "ConML_SupportedMethods_s::\
            BeginElement() : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_DeviceInfo_s
// -----------------------------------------------------------------------------
ConML_DeviceInfo_s::ConML_DeviceInfo_s()
    {
    }

ConML_DeviceInfo_s::~ConML_DeviceInfo_s()
    {
    delete version;
    delete supportedMethods;
    delete maxObjectSize;
    }

CXMLElement* ConML_DeviceInfo_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLVersion:
            CHECK_NO_DUPLICATE (version);
            version = new ( ELeave ) pcdata_t();
            return version;
		
        case EConMLSupportedMethods:
            CHECK_NO_DUPLICATE ( supportedMethods);
            supportedMethods = new ( ELeave ) ConML_SupportedMethods_t();
            return supportedMethods;
		
        case EConMLMaxObjectSize:
            CHECK_NO_DUPLICATE ( maxObjectSize );
            maxObjectSize = new ( ELeave ) pcdata_t();
            return maxObjectSize;
		
        default:
            LOGGER_WRITE_1( "ConML_DeviceInfo_s::BeginElement()\
             : Unknown element %d", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_DeviceInfo_s
// -----------------------------------------------------------------------------
ConML_File_s::ConML_File_s()	
    {
    }

ConML_File_s::~ConML_File_s()	
    {
    delete name;
    delete modified;
    delete size;
    delete userPerm;
    }

CXMLElement* ConML_File_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLName:
            CHECK_NO_DUPLICATE ( name );
            name = new ( ELeave ) pcdata_t();
            return name;
		
        case EConMLModified:
            CHECK_NO_DUPLICATE ( modified );
            modified = new ( ELeave ) pcdata_t();
            return modified;
		
        case EConMLSize:
            CHECK_NO_DUPLICATE ( size );
            size = new ( ELeave ) pcdata_t();
            return size;
		
        case EConMLUserPerm:
            CHECK_NO_DUPLICATE ( userPerm );
            userPerm = new ( ELeave ) pcdata_t();
            return userPerm;
		
        default:
            LOGGER_WRITE_1( "ConML_File_s::BeginElement()\
             : Unknown element %d", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_FileList_s
// -----------------------------------------------------------------------------
ConML_FileList_s::ConML_FileList_s()
    {
    }
	
ConML_FileList_s::~ConML_FileList_s()
    {
    delete data;
    delete next;
    }	

// -----------------------------------------------------------------------------
// ConML_Files_s
// -----------------------------------------------------------------------------
ConML_Files_s::ConML_Files_s()
    {
    }

ConML_Files_s::~ConML_Files_s()
    {
    delete file;
    }

CXMLElement* ConML_Files_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLFile:
            return AddFileL( &file );
		
        default:
            LOGGER_WRITE_1( "ConML_File_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_Progress_s
// -----------------------------------------------------------------------------
ConML_Progress_s::ConML_Progress_s()
    {
    }
	
// -----------------------------------------------------------------------------
// ~ConML_Progress_s
// -----------------------------------------------------------------------------
ConML_Progress_s::~ConML_Progress_s()
    {
    delete value;
    }
	
CXMLElement* ConML_Progress_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLValue:
            CHECK_NO_DUPLICATE ( value );
            value = new ( ELeave ) pcdata_t();
            return value;
		
        default:
            LOGGER_WRITE_1( "ConML_Progress_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_Results_s
// -----------------------------------------------------------------------------
ConML_Results_s::ConML_Results_s()
    {
    }

ConML_Results_s::~ConML_Results_s()
    {
    delete complete;
    delete progress;
    delete applications;
    delete dataOwners;
    delete filename;
    delete moreData;
    delete data;
    delete deviceInfo;
    delete files;
    }

CXMLElement* ConML_Results_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
    {
        case EConMLComplete:
            CHECK_NO_DUPLICATE ( complete );
            complete = new ( ELeave ) pcdata_t();
            return complete;
		
        case EConMLProgress:
            CHECK_NO_DUPLICATE ( progress );
            progress = new ( ELeave ) ConML_Progress_t();
            return progress;
		
        case EConMLApplications:
            CHECK_NO_DUPLICATE ( applications );
            applications = new ( ELeave ) ConML_Applications_t();
            return applications;
		
        case EConMLDataOwners:
            CHECK_NO_DUPLICATE ( dataOwners );
            dataOwners = new ( ELeave ) ConML_DataOwners_t();
            return dataOwners;
		
        case EConMLMoreData:
            CHECK_NO_DUPLICATE ( moreData );
            moreData = new ( ELeave ) pcdata_t();
            return moreData;
		
        case EConMLData:
            CHECK_NO_DUPLICATE ( data );
            data = new ( ELeave ) pcdata_t();
            return data;
		
        case EConMLDeviceInfo:
            CHECK_NO_DUPLICATE ( deviceInfo )
            deviceInfo = new ( ELeave ) ConML_DeviceInfo_t();
            return deviceInfo;
		
        case EConMLFiles:
            CHECK_NO_DUPLICATE ( files )
            files = new ( ELeave ) ConML_Files_t();
            return files;
		
        default:
            LOGGER_WRITE_1( "ConML_Results_s::BeginElement()\
             : Unknown element  %02x ", aTag);
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_Param_s
// -----------------------------------------------------------------------------
ConML_Param_s::ConML_Param_s()
    {
    }
	
ConML_Param_s::~ConML_Param_s()
    {
    delete name;
    delete value;
    }
	
CXMLElement* ConML_Param_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLName:
            CHECK_NO_DUPLICATE ( name );
            name = new ( ELeave ) pcdata_t();
            return name;
		
        case EConMLValue:
            CHECK_NO_DUPLICATE ( value );
            value = new ( ELeave ) pcdata_t();
            return value;
		
        default:
            LOGGER_WRITE_1( "ConML_Param_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_ParamList_s
// -----------------------------------------------------------------------------
ConML_ParamList_s::ConML_ParamList_s()
    {
    }

ConML_ParamList_s::~ConML_ParamList_s()
    {
    delete data;
    delete next;
    }
	
// -----------------------------------------------------------------------------
// ConML_InstParams_s
// -----------------------------------------------------------------------------
ConML_InstParams_s::ConML_InstParams_s()
    {
    }

ConML_InstParams_s::~ConML_InstParams_s()
    {
    delete param;
    }
	
CXMLElement* ConML_InstParams_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLParam:
            return AddParamL( &param );
			
        default:
            LOGGER_WRITE_1( "ConML_InstParams_s::BeginElement()\
             : Unknown element  %02x ", aTag);
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_UnInstall_s
// -----------------------------------------------------------------------------
ConML_UnInstall_s::ConML_UnInstall_s()
    {
    }

ConML_UnInstall_s::~ConML_UnInstall_s()
    {
    delete applications;
    delete instParams;
    delete results;
    }

CXMLElement* ConML_UnInstall_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLApplications:
            CHECK_NO_DUPLICATE ( applications );
            applications = new ( ELeave ) ConML_Applications_t();
            return applications;
		
        case EConMLInstParams:
            CHECK_NO_DUPLICATE ( instParams );
            instParams = new ( ELeave ) ConML_InstParams_t();
            return instParams;
				
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_UnInstall_s::BeginElement()\
             : Unknown element  %02x ", aTag);
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// ConML_ListDataOwners_s
// -----------------------------------------------------------------------------
ConML_ListDataOwners_s::ConML_ListDataOwners_s()
    {
    }

ConML_ListDataOwners_s::~ConML_ListDataOwners_s()
    {
    delete results;
    }

CXMLElement* ConML_ListDataOwners_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_ListDataOwners_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_Install_s
// -----------------------------------------------------------------------------
ConML_Install_s::ConML_Install_s()
    {
    }

ConML_Install_s::~ConML_Install_s()
    {
    delete name;
    delete instParams;
    delete results;
    }

CXMLElement* ConML_Install_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLName:
            CHECK_NO_DUPLICATE ( name );
            name = new ( ELeave ) pcdata_t();
            return name;
			
        case EConMLInstParams:
            CHECK_NO_DUPLICATE ( instParams );
            instParams = new ( ELeave ) ConML_InstParams_t();
            return instParams;
		
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_Install_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_ListInstalledApps
// -----------------------------------------------------------------------------
ConML_ListInstalledApps_s::ConML_ListInstalledApps_s()
    {
    }

ConML_ListInstalledApps_s::~ConML_ListInstalledApps_s()
    {
    delete drives;
    delete all;
    delete results;
    }

CXMLElement* ConML_ListInstalledApps_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLDrives:
            CHECK_NO_DUPLICATE ( drives );
            drives = new ( ELeave ) ConML_Drives_t();
            return drives;
		
        case EConMLAll:
            CHECK_NO_DUPLICATE ( all );
            all = new ( ELeave ) pcdata_t();
            return all;
		
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_ListInstalledApps_s::\
            BeginElement() : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_SetBURMode_s
// -----------------------------------------------------------------------------
ConML_SetBURMode_s::ConML_SetBURMode_s()
    {
    }

ConML_SetBURMode_s::~ConML_SetBURMode_s()
    {
    delete drives;
    delete partialType;
    delete incType;
    delete results;
    }

CXMLElement* ConML_SetBURMode_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLDrives:
            drives = new ( ELeave ) ConML_Drives_t();
            return drives;
		
        case EConMLPartialType:
            CHECK_NO_DUPLICATE ( partialType );
            partialType = new ( ELeave ) pcdata_t();
            return partialType;
		
        case EConMLIncType:
            CHECK_NO_DUPLICATE ( incType );
            incType = new ( ELeave ) pcdata_t();
            return incType;
		
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_SetBURMode_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
            }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_GetDataSize_s
// -----------------------------------------------------------------------------
ConML_GetDataSize_s::ConML_GetDataSize_s()
    {
    }

ConML_GetDataSize_s::~ConML_GetDataSize_s()
    {
    delete dataOwners;
    delete results;
    }

CXMLElement* ConML_GetDataSize_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLDataOwners:
            CHECK_NO_DUPLICATE ( dataOwners );
            dataOwners = new ( ELeave ) ConML_DataOwners_t();
            return dataOwners;
		
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
	
        default:
            LOGGER_WRITE_1( "ConML_GetDataSize_s::BeginElement()\
             : Unknown element  %02x ", aTag);
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }	
	
// -----------------------------------------------------------------------------
// ConML_RequestData_s
// -----------------------------------------------------------------------------
ConML_RequestData_s::ConML_RequestData_s()
    {
    }

ConML_RequestData_s::~ConML_RequestData_s()
    {
    delete sid;
    delete results;
    }

CXMLElement* ConML_RequestData_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLSID:
            return AddSIDL( &sid );
		
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_RequestData_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_UpdateDeviceInfo_s
// -----------------------------------------------------------------------------
ConML_UpdateDeviceInfo_s::ConML_UpdateDeviceInfo_s()
    {
    }

ConML_UpdateDeviceInfo_s::~ConML_UpdateDeviceInfo_s()
    {
    delete deviceInfo;
    delete results;
    }

CXMLElement* ConML_UpdateDeviceInfo_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLDeviceInfo:
            CHECK_NO_DUPLICATE ( deviceInfo )
            deviceInfo = new ( ELeave ) ConML_DeviceInfo_t();
            return deviceInfo;
		
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results )
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_UpdateDeviceInfo_s::\
            BeginElement() : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_ListPublicFiles_s
// -----------------------------------------------------------------------------
ConML_ListPublicFiles_s::ConML_ListPublicFiles_s()
    {
    }

ConML_ListPublicFiles_s::~ConML_ListPublicFiles_s()
    {
    delete results;
    delete sid;
    }

CXMLElement* ConML_ListPublicFiles_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        case EConMLSID:
            return AddSIDL ( &sid );
	
        default:
            LOGGER_WRITE_1( "ConML_ListPublicFiles_s::\
            BeginElement() : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_Task_s
// -----------------------------------------------------------------------------
ConML_GetDataOwnerStatus_s::ConML_GetDataOwnerStatus_s()
    {
    }

ConML_GetDataOwnerStatus_s::~ConML_GetDataOwnerStatus_s()
    {
    delete dataOwners;
    delete results;
    }
    
CXMLElement* ConML_GetDataOwnerStatus_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {   
    switch ( aTag )
        {
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        case EConMLDataOwners:
            CHECK_NO_DUPLICATE ( dataOwners );
            dataOwners = new ( ELeave ) ConML_DataOwners_t();
            return dataOwners;

        default:
            LOGGER_WRITE_1( "ConML_GetDataOwnerStatus_s::\
            BeginElement() : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_GetMetadata_s
// -----------------------------------------------------------------------------
ConML_GetMetadata_s::ConML_GetMetadata_s()
    {
    }

ConML_GetMetadata_s::~ConML_GetMetadata_s()
    {
    delete filename;
    delete results;
    }
    
CXMLElement* ConML_GetMetadata_s::BeginElementL(
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {   
    switch ( aTag )
        {
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        case EConMLFilename:
            CHECK_NO_DUPLICATE ( filename );
            filename = new ( ELeave ) pcdata_t();
            return filename;

        default:
            LOGGER_WRITE_1( "ConML_GetMetadata_s::\
            BeginElement() : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

	
// -----------------------------------------------------------------------------
// ConML_Reboot_s
// -----------------------------------------------------------------------------
ConML_Reboot_s::ConML_Reboot_s()
    {
    }

ConML_Reboot_s::~ConML_Reboot_s()
    {
    delete results;
    }

CXMLElement* ConML_Reboot_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {
    switch ( aTag )
        {
        case EConMLResults:
            CHECK_NO_DUPLICATE ( results );
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_Reboot_s::BeginElementL()\
             : Unknown element %d ", aTag );
            User::Leave ( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_Task_s
// -----------------------------------------------------------------------------
ConML_Task_s::ConML_Task_s()
    {
    }

ConML_Task_s::~ConML_Task_s()
    {
    delete id;
    delete install;
    delete unInstall;
    delete listInstalledApps;
    delete listDataOwners;
    delete setBurMode;
    delete getDataSize;
    delete requestData;
    delete updateDeviceInfo;
    delete listPublicFiles;
    delete getDataOwnerStatus;
    delete supplyData;
    delete reboot;
    delete getMetadata;
    }

CXMLElement* ConML_Task_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLID:
            CHECK_NO_DUPLICATE ( id )
            id = new ( ELeave ) pcdata_t();
            return id;
		
        case EConMLInstall:
            CHECK_NO_DUPLICATE ( install );
            install = new ( ELeave ) ConML_Install_t();
            return install;
		
        case EConMLUnInstall:
            CHECK_NO_DUPLICATE ( unInstall );
            unInstall = new ( ELeave ) ConML_UnInstall_t();
            return unInstall;
		
        case EConMLListInstalledApps:
            CHECK_NO_DUPLICATE ( listInstalledApps );
            listInstalledApps = new ( ELeave ) ConML_ListInstalledApps_t();
            return listInstalledApps;
		
        case EConMLListDataOwners:
            CHECK_NO_DUPLICATE ( listDataOwners );
            listDataOwners = new ( ELeave ) ConML_ListDataOwners_t();
            return listDataOwners;
		
        case EConMLSetBURMode:
            CHECK_NO_DUPLICATE ( setBurMode );
            setBurMode = new ( ELeave ) ConML_SetBURMode_t();
            return setBurMode;
		
        case EConMLGetDataSize:
            CHECK_NO_DUPLICATE ( getDataSize );
            getDataSize = new ( ELeave ) ConML_GetDataSize_t();
            return getDataSize;
		
        case EConMLRequestData:
            CHECK_NO_DUPLICATE ( requestData )
            requestData = new ( ELeave ) ConML_RequestData_t();
            return requestData;
		
        case EConMLUpdateDeviceInfo:
            CHECK_NO_DUPLICATE ( updateDeviceInfo );
            updateDeviceInfo = new ( ELeave ) ConML_UpdateDeviceInfo_t();
            return updateDeviceInfo;
		
        case EConMLListPublicFiles:
            CHECK_NO_DUPLICATE ( listPublicFiles );
            listPublicFiles = new ( ELeave ) ConML_ListPublicFiles_t();
            return listPublicFiles;
		
        case EConMLGetDataOwnerStatus:
            CHECK_NO_DUPLICATE ( getDataOwnerStatus );
            getDataOwnerStatus = new ( ELeave ) ConML_GetDataOwnerStatus_t();
            return getDataOwnerStatus;
		
        case EConMLSupplyData:
            CHECK_NO_DUPLICATE ( supplyData );
            supplyData = new ( ELeave ) ConML_SupplyData_t();
            return supplyData;
		
        case EConMLReboot:
            CHECK_NO_DUPLICATE ( reboot );
            reboot = new ( ELeave ) ConML_Reboot_t();
            return reboot;
        case EConMLGetMetadata:
            CHECK_NO_DUPLICATE ( getMetadata );
            getMetadata = new ( ELeave ) ConML_GetMetadata_t();
            return getMetadata;
		
        default:
            LOGGER_WRITE_1( "ConML_Task_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_TaskList_s
// -----------------------------------------------------------------------------
ConML_TaskList_s::ConML_TaskList_s()
    {
    }
	
ConML_TaskList_s::~ConML_TaskList_s()
    {
    delete data;
    delete next;
    }	

// -----------------------------------------------------------------------------
// ConML_Status_s
// -----------------------------------------------------------------------------
ConML_Status_s::ConML_Status_s()
    {
    }
	
ConML_Status_s::~ConML_Status_s()
    {
    delete task;
    }

CXMLElement* ConML_Status_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLTask:
            return AddTaskL( &task );
				
        default:
            LOGGER_WRITE_1( "ConML_Status_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_Cancel_s
// -----------------------------------------------------------------------------
ConML_Cancel_s::ConML_Cancel_s()
    {
    }

ConML_Cancel_s::~ConML_Cancel_s()
    {
    delete id;
    delete all;
    }

CXMLElement* ConML_Cancel_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLID:
            CHECK_NO_DUPLICATE ( id );
            id = new ( ELeave ) pcdata_t();
            return id;
		
        case EConMLAll:
            CHECK_NO_DUPLICATE ( all );
            all = new ( ELeave ) pcdata_t();
            return all;
		
        default:
            LOGGER_WRITE_1( "ConML_Cancel_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_GetStatus_s
// -----------------------------------------------------------------------------
ConML_GetStatus_s::ConML_GetStatus_s()
    {
    }

ConML_GetStatus_s::~ConML_GetStatus_s()
    {
    delete id;
    delete all;
    }

CXMLElement* ConML_GetStatus_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLID:
        CHECK_NO_DUPLICATE ( id );
        id = new ( ELeave ) pcdata_t();
        return id;
		
        case EConMLAll:
            CHECK_NO_DUPLICATE ( all );
            all = new ( ELeave ) pcdata_t();
            return all;
		
        default:
            LOGGER_WRITE_1( "ConML_GetStatus_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_SupplyData_s
// -----------------------------------------------------------------------------
ConML_SupplyData_s::ConML_SupplyData_s()
    {
    }

ConML_SupplyData_s::~ConML_SupplyData_s()
    {
    delete sid;
    delete results;
    }

CXMLElement* ConML_SupplyData_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    switch ( aTag )
        {
        case EConMLSID:
            return AddSIDL ( &sid );
		
        case EConMLResults:
            results = new ( ELeave ) ConML_Results_t();
            return results;
		
        default:
            LOGGER_WRITE_1( "ConML_SupplyData_s::BeginElement() :\
             Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ConML_Execute_
// -----------------------------------------------------------------------------
ConML_Execute_s::ConML_Execute_s()
    {
    }

ConML_Execute_s::~ConML_Execute_s()
    {
    delete id;
    delete install;
    delete unInstall;
    delete listInstalledApps;
    delete listDataOwners;
    delete setBurMode;
    delete getDataSize;
    delete requestData;
    delete updateDeviceInfo;
    delete listPublicFiles;
    delete supplyData;
    delete reboot;
    delete getDataOwnerStatus;
    delete getMetadata;
    }

CXMLElement* ConML_Execute_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )	
    {
    
    switch ( aTag )
        {
        case EConMLID:
            CHECK_NO_DUPLICATE ( id );
            id = new ( ELeave ) pcdata_t();
            return id;
		
        case EConMLInstall:
            CHECK_NO_DUPLICATE ( install );
            install = new ( ELeave ) ConML_Install_t();
            return install;

        case EConMLUpdateDeviceInfo:
            CHECK_NO_DUPLICATE (updateDeviceInfo);
            updateDeviceInfo = new ( ELeave ) ConML_UpdateDeviceInfo_t();
            return updateDeviceInfo;
		
        case EConMLUnInstall:
            CHECK_NO_DUPLICATE ( unInstall );
            unInstall = new ( ELeave ) ConML_UnInstall_t();
            return unInstall;
			
        case EConMLListInstalledApps:
            CHECK_NO_DUPLICATE ( listInstalledApps );
            listInstalledApps = new ( ELeave ) ConML_ListInstalledApps_t();
            return listInstalledApps;
		
        case EConMLListDataOwners:
            CHECK_NO_DUPLICATE ( listDataOwners );
            listDataOwners = new ( ELeave ) ConML_ListDataOwners_t();
            return listDataOwners;
		
        case EConMLSetBURMode:
            CHECK_NO_DUPLICATE ( setBurMode );
            setBurMode = new ( ELeave ) ConML_SetBURMode_t();
            return setBurMode;
		
        case EConMLGetDataSize:
            CHECK_NO_DUPLICATE ( getDataSize );
            getDataSize = new ( ELeave ) ConML_GetDataSize_t();
            return getDataSize;
		
        case EConMLRequestData:
            CHECK_NO_DUPLICATE ( requestData );
            requestData = new ( ELeave ) ConML_RequestData_t();
            return requestData;
		
        case EConMLListPublicFiles:
            CHECK_NO_DUPLICATE ( listPublicFiles );
            listPublicFiles = new ( ELeave ) ConML_ListPublicFiles_t();
            return listPublicFiles;
		
        case EConMLSupplyData:
            CHECK_NO_DUPLICATE ( supplyData );
            supplyData = new ( ELeave ) ConML_SupplyData_t();
            return supplyData;
		
        case EConMLReboot:
            CHECK_NO_DUPLICATE ( reboot );
            reboot = new ( ELeave ) pcdata_t();
            return reboot;
		
        case EConMLGetDataOwnerStatus:
            CHECK_NO_DUPLICATE ( getDataOwnerStatus );
            getDataOwnerStatus = new ( ELeave ) ConML_GetDataOwnerStatus_t();
            return getDataOwnerStatus;
        
        case EConMLGetMetadata:
            CHECK_NO_DUPLICATE ( getMetadata );
            getMetadata = new ( ELeave ) ConML_GetMetadata_t();
            return getMetadata;
		
        default:
            LOGGER_WRITE_1( "ConML_Execute_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// ConML_ConML_s
// -----------------------------------------------------------------------------
ConML_ConML_s::ConML_ConML_s()
    {
    }

ConML_ConML_s::~ConML_ConML_s()
    {
    delete execute;
    delete getStatus;
    delete cancel;
    delete status;
    }

CXMLElement* ConML_ConML_s::BeginElementL( 
    TUint8 aTag, const TXMLElementParams& /*aParams*/ )
    {
    switch( aTag )
        {
        case EConMLExecute:
            CHECK_NO_DUPLICATE( execute );
            execute = new (ELeave) ConML_Execute_t();
            return execute;
		
        case EConMLGetStatus:
            CHECK_NO_DUPLICATE ( getStatus );
            getStatus = new ( ELeave) ConML_GetStatus_t();
            return getStatus;
		
        case EConMLCancel:
            CHECK_NO_DUPLICATE ( cancel );
            cancel = new ( ELeave ) ConML_Cancel_t();
            return cancel;
		
        case EConMLStatus:
            CHECK_NO_DUPLICATE ( status )
            status = new ( ELeave ) ConML_Status_t();
            return status;
			
        default:
            LOGGER_WRITE_1( "ConML_ConML_s::BeginElement()\
             : Unknown element  %02x ", aTag );
            User::Leave( KWBXMLParserErrorInvalidTag );
        }
    return KErrNone;
    }

CXMLElement::TAction ConML_ConML_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
    {
    switch( aTag)
        {
        case EConML:
            ((MWBXMLConMLCallback*) aCallbacks)->ConMLL(this);
            return EPopAndDestroy;
				
        default:
            return EPop;
        }
    }

TBool ConML_ConML_s::NeedsCleanup() const
    {
    return ETrue;
    }
	

	
		
