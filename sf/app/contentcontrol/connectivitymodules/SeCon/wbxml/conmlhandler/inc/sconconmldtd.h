/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SCONCONMLDTD_H__
#define __SCONCONMLDTD_H__

#include "sconxmlelement.h"

typedef void *voidPtr_t;
typedef long memSize_t;

typedef enum 
    {
    SML_PCDATA_UNDEFINED = 0,
    SML_PCDATA_STRING,                   // String type
    SML_PCDATA_OPAQUE,                   // Opaque type
    SML_PCDATA_EXTENSION,                // Extention type
    SML_PCDATA_CDATA                     // XML CDATA type   
    } pcdataType_t;

typedef enum 
    {
    SML_EXT_UNDEFINED = 0,
    } pcdataExtension_t;

// -----------------------------------------------------------------------------
// GenericListAddL
// -----------------------------------------------------------------------------
template<class T>
void GenericListAddL( T* aList, T aNewItem )
    {
    if( *aList == 0 )
        {
        *aList = aNewItem;
        }
    else
        {
        T c;
        for( c = *aList; c->next; c = c->next )
            {}; 
        c->next = aNewItem;
        }
    }

// -----------------------------------------------------------------------------
// pcdata_s
// -----------------------------------------------------------------------------
typedef NONSHARABLE_STRUCT( pcdata_s ) : public CXMLElement
    {
public:
    pcdataType_t      contentType;   // The type of data 
    pcdataExtension_t  extension;     // PCData Extension type
    memSize_t             length;        // length of the data 
    voidPtr_t             content;       // Pointer to the data itself
public:
    pcdata_s();
    ~pcdata_s();
    pcdata_s( const pcdataType_t aType, 
        const pcdataExtension_t aExt =  SML_EXT_UNDEFINED );
    void FreeContent();
    void SetDataL( const TDesC8& aData );
    TPtrC8 Data() const;
    void TruncateL( TInt aConsumed );
    } *pcdataPtr_t, pcdata_t;

// -----------------------------------------------------------------------------
// pcdata_list_s
// -----------------------------------------------------------------------------
typedef NONSHARABLE_STRUCT( pcdata_list_s ) : public CBase
    {
    pcdataPtr_t data;
    struct pcdata_list_s *next;

    pcdata_list_s();
    ~pcdata_list_s();
    } *pcdataListPtr_t, pcdataList_t;

// -----------------------------------------------------------------------------
// ConML_Param_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Param_s ) : public CXMLElement
    {
public:
    pcdataPtr_t name;
    pcdataPtr_t value;
public:
    ConML_Param_s();
    ~ConML_Param_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_ParamPtr_t, ConML_Param_t;

// -----------------------------------------------------------------------------
// ConML_ParamList_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_ParamList_s ) : public CBase
    {
public:
    ConML_ParamPtr_t data;
    struct ConML_ParamList_s *next;
public:
    ConML_ParamList_s();
    ~ConML_ParamList_s();
    }*ConML_ParamListPtr_t, ConML_ParamList_t;  
    
// -----------------------------------------------------------------------------
// ConML_InstParams_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_InstParams_s ) : public CXMLElement
    {
public:
    ConML_ParamListPtr_t param;
public:
    ConML_InstParams_s();
    ~ConML_InstParams_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_InstParamsPtr_t, ConML_InstParams_t;

// -----------------------------------------------------------------------------
// ConML_Progress_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Progress_s ) : public CXMLElement
    {
public:
    pcdataPtr_t value;
public:
    ConML_Progress_s();
    ~ConML_Progress_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_ProgressPtr_t, ConML_Progress_t;

// -----------------------------------------------------------------------------
// ConML_Application_s
// -----------------------------------------------------------------------------        
typedef NONSHARABLE_STRUCT( ConML_Application_s ) : public CXMLElement
    {
public:
    pcdataPtr_t name;
    pcdataPtr_t uid;
public:
    ConML_Application_s();
    ~ConML_Application_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_ApplicationPtr_t, ConML_Application_t;

// -----------------------------------------------------------------------------
// ConML_ApplicationList_s
// -----------------------------------------------------------------------------        
    
typedef NONSHARABLE_STRUCT( ConML_ApplicationList_s ) : public CBase
    {
public:
    ConML_ApplicationPtr_t data;
    struct ConML_ApplicationList_s *next;
public:
    ConML_ApplicationList_s();
    ~ConML_ApplicationList_s();
    }*ConML_ApplicationListPtr_t, ConML_ApplicationList_t;

// -----------------------------------------------------------------------------
// ConML_Applications_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Applications_s ) : public CXMLElement
    {
public:
    ConML_ApplicationListPtr_t application;
    
public:
    ConML_Applications_s();
    ~ConML_Applications_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_ApplicationsPtr_t, ConML_Applications_t;

// -----------------------------------------------------------------------------
// ConML_Drive_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Drive_s ) : public CXMLElement
    {
public:
    pcdataPtr_t name;
public:
    ConML_Drive_s();
    ~ConML_Drive_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_DrivePtr_t, ConML_Drive_t;

// -----------------------------------------------------------------------------
// ConML_DriveList_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_DriveList_s ) : public CBase
    {
public:
    ConML_DrivePtr_t data;
    struct ConML_DriveList_s *next;
public:
    ConML_DriveList_s();
    ~ConML_DriveList_s();
    }*ConML_DriveListPtr_t, ConML_DriveList_t;
    
// -----------------------------------------------------------------------------
// ConML_Drives_s
// -----------------------------------------------------------------------------        
typedef NONSHARABLE_STRUCT( ConML_Drives_s ) : public CXMLElement
    {
public:
    ConML_DriveListPtr_t drive;
public:
    ConML_Drives_s();
    ~ConML_Drives_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_DrivesPtr_t, ConML_Drives_t;

// -----------------------------------------------------------------------------
// ConML_BUROptions_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_BUROptions_s ) : public CXMLElement
    {
public:
    pcdataPtr_t requiresReboot;
    pcdataPtr_t hasFiles;
    pcdataPtr_t supportsInc;
    pcdataPtr_t delayToPrepareData;
public:
    ConML_BUROptions_s();
    ~ConML_BUROptions_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_BUROptionsPtr_t, ConML_BUROptions_t;

// -----------------------------------------------------------------------------
// ConML_PackageInfo_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_PackageInfo_s ) : public CXMLElement
    {
public:
    pcdataPtr_t name;
public:
    ConML_PackageInfo_s();
    ~ConML_PackageInfo_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_PackageInfoPtr_t, ConML_PackageInfo_t;

// -----------------------------------------------------------------------------
// ConML_SID_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_SID_s ) : public CXMLElement
    {
public:
    pcdataPtr_t type;
    pcdataPtr_t uid;
    ConML_DrivesPtr_t drives;
    ConML_PackageInfoPtr_t packageInfo;
    ConML_BUROptionsPtr_t burOptions;
    pcdataPtr_t size;
    pcdataPtr_t transferDataType;
    pcdataPtr_t data;
    pcdataPtr_t moreData;
    pcdataPtr_t dataOwnerStatus;
    
public:
    ConML_SID_s();
    ~ConML_SID_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_SIDPtr_t, ConML_SID_t;

// -----------------------------------------------------------------------------
// ConML_SIDList_s
// -----------------------------------------------------------------------------
typedef NONSHARABLE_STRUCT( ConML_SIDList_s ) : public CBase
    {
public:
    ConML_SIDPtr_t data;
    struct ConML_SIDList_s *next;
public:
    ConML_SIDList_s();
    ~ConML_SIDList_s();
    }*ConML_SIDListPtr_t, ConML_SIDList_t;
    
// -----------------------------------------------------------------------------
// ConML_SupportedMethods_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_SupportedMethods_s ) : public CXMLElement
    {
public:
    pcdataPtr_t install;
    pcdataPtr_t unInstall;
    pcdataPtr_t setInstParams;
    pcdataPtr_t listInstalledApps;
    pcdataPtr_t listDataOwners;
    pcdataPtr_t setBurMode;
    pcdataPtr_t getDataSize;
    pcdataPtr_t requestData;
    pcdataPtr_t supplyData;
    pcdataPtr_t reboot;
public:
    ConML_SupportedMethods_s();
    ~ConML_SupportedMethods_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_SupportedMethodsPtr_t, ConML_SupportedMethods_t;
    
// -----------------------------------------------------------------------------
// ConML_DeviceInfo_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_DeviceInfo_s ) : public CXMLElement
        {
public:
    pcdataPtr_t version;
    ConML_SupportedMethodsPtr_t supportedMethods;
    pcdataPtr_t maxObjectSize;
public:
    ConML_DeviceInfo_s();
    ~ConML_DeviceInfo_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_DeviceInfoPtr_t, ConML_DeviceInfo_t;

// -----------------------------------------------------------------------------
// ConML_File_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_File_s ) : public CXMLElement
    {
public:
    pcdataPtr_t name;
    pcdataPtr_t modified;
    pcdataPtr_t size;
    pcdataPtr_t userPerm;
public:
    ConML_File_s();
    ~ConML_File_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_FilePtr_t, ConML_File_t;

// -----------------------------------------------------------------------------
// ConML_FileList_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_FileList_s ) : public CBase
    {
public:
    ConML_FilePtr_t data;
    struct ConML_FileList_s *next;
public:
    ConML_FileList_s();
    ~ConML_FileList_s();
    }*ConML_FileListPtr_t, ConML_FileList_t;

// -----------------------------------------------------------------------------
// ConML_Files_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Files_s ) : public CXMLElement
    {
public:
    ConML_FileListPtr_t  file;
public:
    ConML_Files_s();
    ~ConML_Files_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_FilesPtr_t, ConML_Files_t;
    
// -----------------------------------------------------------------------------
// ConML_DataOwners_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_DataOwners_s ) : public CXMLElement
    {
public:
    ConML_SIDListPtr_t sid;
public:
    ConML_DataOwners_s();
    ~ConML_DataOwners_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_DataOwnersPtr_t, ConML_DataOwners_t;

// -----------------------------------------------------------------------------
// ConML_Results_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Results_s ) : public CXMLElement
    {
public:
    pcdataPtr_t complete;
    ConML_ProgressPtr_t  progress;
    ConML_ApplicationsPtr_t applications;
    ConML_DataOwnersPtr_t dataOwners;
    pcdataPtr_t filename;
    pcdataPtr_t moreData;
    pcdataPtr_t data;
    ConML_DeviceInfoPtr_t deviceInfo;
    ConML_FilesPtr_t files; 
public:
    ConML_Results_s();
    ~ConML_Results_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_ResultsPtr_t, ConML_Results_t;

// -----------------------------------------------------------------------------
// ConML_Install_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Install_s ) : public CXMLElement
    {
public:
    pcdataPtr_t name;
    ConML_InstParamsPtr_t  instParams;
    ConML_ResultsPtr_t results; 
public:
    ConML_Install_s();
    ~ConML_Install_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_InstallPtr_t, ConML_Install_t;

// -----------------------------------------------------------------------------
// ConML_UnInstall_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_UnInstall_s ) : public CXMLElement
    {
public:
    ConML_ApplicationsPtr_t applications;
    ConML_InstParamsPtr_t instParams;
    ConML_ResultsPtr_t results;
public:
    ConML_UnInstall_s();
    ~ConML_UnInstall_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_UnInstallPtr_t, ConML_UnInstall_t;

// -----------------------------------------------------------------------------
// ConML_ListInstalledApps_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_ListInstalledApps_s ) : public CXMLElement
    {
public:
    ConML_DrivesPtr_t drives;
    pcdataPtr_t all;
    ConML_ResultsPtr_t results;
public:
    ConML_ListInstalledApps_s();
    ~ConML_ListInstalledApps_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_ListInstalledAppsPtr_t, ConML_ListInstalledApps_t;
    
// -----------------------------------------------------------------------------
// ConML_ListDataOwners_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_ListDataOwners_s ) : public CXMLElement
    {
public:
    ConML_ResultsPtr_t results;
public:
    ConML_ListDataOwners_s();
    ~ConML_ListDataOwners_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_ListDataOwnersPtr_t, ConML_ListDataOwners_t;

// -----------------------------------------------------------------------------
// ConML_SetBURMode_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_SetBURMode_s ) : public CXMLElement
    {
public:
    ConML_DrivesPtr_t drives;
    pcdataPtr_t partialType;
    pcdataPtr_t incType;
    ConML_ResultsPtr_t results;
public:
    ConML_SetBURMode_s();
    ~ConML_SetBURMode_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_SetBURModePtr_t, ConML_SetBURMode_t;

// -----------------------------------------------------------------------------
// ConML_GetDataSize_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_GetDataSize_s ) : public CXMLElement
    {
public:
    ConML_DataOwnersPtr_t dataOwners;
    ConML_ResultsPtr_t results;
public:
    ConML_GetDataSize_s();
    ~ConML_GetDataSize_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_GetDataSizePtr_t, ConML_GetDataSize_t;

// -----------------------------------------------------------------------------
// ConML_RequestData_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_RequestData_s ) : public CXMLElement
    {
public:
    ConML_SIDListPtr_t sid;
    ConML_ResultsPtr_t results;
public:
    ConML_RequestData_s();
    ~ConML_RequestData_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_RequestDataPtr_t, ConML_RequestData_t;

// -----------------------------------------------------------------------------
// ConML_UpdateDeviceInfo_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_UpdateDeviceInfo_s ) : public CXMLElement
    {
public:
    ConML_DeviceInfoPtr_t deviceInfo;
    ConML_ResultsPtr_t results;
public:
    ConML_UpdateDeviceInfo_s();
    ~ConML_UpdateDeviceInfo_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_UpdateDeviceInfoPtr_t, ConML_UpdateDeviceInfo_t;

// -----------------------------------------------------------------------------
// ConML_ListPublicFiles_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_ListPublicFiles_s ) : public CXMLElement
    {
public:
    ConML_ResultsPtr_t results;
    ConML_SIDListPtr_t sid;
public:
    ConML_ListPublicFiles_s();
    ~ConML_ListPublicFiles_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_ListPublicFilesPtr_t, ConML_ListPublicFiles_t;

// -----------------------------------------------------------------------------
// ConML_SupplyData_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_SupplyData_s ) : public CXMLElement
    {
public:
    ConML_SIDListPtr_t sid;
    ConML_ResultsPtr_t results;
public:
    ConML_SupplyData_s();
    ~ConML_SupplyData_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_SupplyDataPtr_t, ConML_SupplyData_t;

// -----------------------------------------------------------------------------
// ConML_GetDataOwnerStatus_s
// -----------------------------------------------------------------------------        
typedef NONSHARABLE_STRUCT( ConML_GetDataOwnerStatus_s ) : public CXMLElement
    {
public:
    ConML_DataOwnersPtr_t dataOwners;
    ConML_ResultsPtr_t results;
public:
    ConML_GetDataOwnerStatus_s();
    ~ConML_GetDataOwnerStatus_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_GetDataOwnerStatusPtr_t, ConML_GetDataOwnerStatus_t;

// -----------------------------------------------------------------------------
// ConML_GetMetadata_s
// -----------------------------------------------------------------------------
typedef NONSHARABLE_STRUCT( ConML_GetMetadata_s ) : public CXMLElement
    {
public:
    pcdataPtr_t filename;
    ConML_ResultsPtr_t results;
public:
    ConML_GetMetadata_s();
    ~ConML_GetMetadata_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_GetMetadataPtr_t, ConML_GetMetadata_t;

// -----------------------------------------------------------------------------
// ConML_Execute_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Execute_s ) : public CXMLElement
    {
public:
    pcdataPtr_t id;
    ConML_InstallPtr_t install;
    ConML_UnInstallPtr_t  unInstall;
    ConML_ListInstalledAppsPtr_t listInstalledApps;
    ConML_ListDataOwnersPtr_t listDataOwners;
    ConML_SetBURModePtr_t setBurMode;
    ConML_GetDataSizePtr_t getDataSize;
    ConML_RequestDataPtr_t requestData;
    ConML_UpdateDeviceInfoPtr_t updateDeviceInfo;
    ConML_ListPublicFilesPtr_t listPublicFiles;
    ConML_SupplyDataPtr_t supplyData;
    pcdataPtr_t reboot;
    ConML_GetDataOwnerStatusPtr_t getDataOwnerStatus;
    ConML_GetMetadataPtr_t getMetadata;
public:
    ConML_Execute_s();
    ~ConML_Execute_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_ExecutePtr_t, ConML_Execute_t;
    
// -----------------------------------------------------------------------------
// ConML_GetStatus_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_GetStatus_s ) : public CXMLElement
    {
public:
    pcdataPtr_t id;
    pcdataPtr_t all;
public:
    ConML_GetStatus_s();
    ~ConML_GetStatus_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_GetStatusPtr_t, ConML_GetStatus_t;

// -----------------------------------------------------------------------------
// ConML_Cancel_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Cancel_s ) : public CXMLElement
    {
public:
    pcdataPtr_t id;
    pcdataPtr_t all;
public:
    ConML_Cancel_s();
    ~ConML_Cancel_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_CancelPtr_t, ConML_Cancel_t;

// -----------------------------------------------------------------------------
// ConML_Reboot_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Reboot_s ) : public CXMLElement
    {
public:
    ConML_ResultsPtr_t results;
public:
    ConML_Reboot_s();
    ~ConML_Reboot_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    }*ConML_RebootPtr_t, ConML_Reboot_t;
    
// -----------------------------------------------------------------------------
// ConML_Task_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Task_s ) : public CXMLElement
    {
public:
    pcdataPtr_t id;
    ConML_InstallPtr_t install;
    ConML_UnInstallPtr_t unInstall;
    ConML_ListInstalledAppsPtr_t listInstalledApps;
    ConML_ListDataOwnersPtr_t listDataOwners;
    ConML_SetBURModePtr_t setBurMode;
    ConML_GetDataSizePtr_t getDataSize;
    ConML_RequestDataPtr_t requestData;
    ConML_UpdateDeviceInfoPtr_t updateDeviceInfo;
    ConML_ListPublicFilesPtr_t listPublicFiles;
    ConML_GetDataOwnerStatusPtr_t getDataOwnerStatus;
    ConML_SupplyDataPtr_t supplyData;
    ConML_RebootPtr_t reboot;
    ConML_GetMetadataPtr_t getMetadata;
public:
    ConML_Task_s();
    ~ConML_Task_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_TaskPtr_t, ConML_Task_t;

// -----------------------------------------------------------------------------
// ConML_TaskList_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_TaskList_s ) : public CBase
    {
public:
    ConML_TaskPtr_t data;
    struct ConML_TaskList_s *next;
public:
    ConML_TaskList_s();
    ~ConML_TaskList_s();
    }*ConML_TaskListPtr_t, ConML_TaskList_t;

// -----------------------------------------------------------------------------
// ConML_Status_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_Status_s ) : public CXMLElement
    {
public:
    ConML_TaskListPtr_t task;
public:
    ConML_Status_s();
    ~ConML_Status_s();
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );    
    }*ConML_StatusPtr_t, ConML_Status_t;

// -----------------------------------------------------------------------------
// ConML_ConML_s
// -----------------------------------------------------------------------------    
typedef NONSHARABLE_STRUCT( ConML_ConML_s ) : public CXMLElement
    {
public:
    ConML_ExecutePtr_t execute;
    ConML_GetStatusPtr_t getStatus;
    ConML_CancelPtr_t cancel;
    ConML_StatusPtr_t status;   
        
public:
    ConML_ConML_s();
    ~ConML_ConML_s();
    TBool NeedsCleanup() const;
    CXMLElement* BeginElementL
        ( TUint8 aTag, const TXMLElementParams& aParams = TXMLElementParams() );
    CXMLElement::TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
    } *ConML_ConMLPtr_t, ConML_ConML_t;
                                                                                    
#endif // __SCONCONMLDTD_H__
