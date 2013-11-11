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
* Description:  ConML Task implementations
*
*/


// INCLUDE FILES

#include "sconconmltask.h"

// -----------------------------------------------------------------------------
// CSConReboot::CSConReboot()
// 
// -----------------------------------------------------------------------------
//
CSConReboot::CSConReboot()
    {
    }
        
// -----------------------------------------------------------------------------
// CSConReboot::~CSConReboot()
// 
// -----------------------------------------------------------------------------
//  
CSConReboot::~CSConReboot()
    {
    }

// -----------------------------------------------------------------------------
// CSConReboot::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConReboot* CSConReboot::CopyL()
    {
    CSConReboot* copy = new (ELeave) CSConReboot();
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
    return copy;
    }
    
// -----------------------------------------------------------------------------
// CSConDataOwner::CSConDataOwner()
// 
// -----------------------------------------------------------------------------
//  
CSConDataOwner::CSConDataOwner()
    {
    iHasFiles = ENoFiles;
    iJavaHash = NULL;
    iUid.iUid = 0;
    
    //Initialize iDriveList with zeros
    iDriveList.Fill( '\x0' );
    }
    
// -----------------------------------------------------------------------------
// CSConDataOwner::~CSConDataOwner()
// 
// -----------------------------------------------------------------------------
//  
CSConDataOwner::~CSConDataOwner()
    {
    delete iJavaHash;
    iJavaHash = NULL;
    }
    
// -----------------------------------------------------------------------------
// CSConDataOwner::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConDataOwner* CSConDataOwner::CopyL()
    {
    CSConDataOwner* copy = new (ELeave) CSConDataOwner();
    CleanupStack::PushL( copy );
    copy->iType = iType;
    copy->iUid = iUid;
    copy->iDriveList.Copy( iDriveList );
    copy->iPackageName = iPackageName;
    copy->iReqReboot = iReqReboot;
    copy->iHasFiles = iHasFiles;
    copy->iSupportsInc = iSupportsInc;
    copy->iSupportsSel = iSupportsSel;
    copy->iDelayToPrep = iDelayToPrep;
    copy->iSize = iSize;
    copy->iDataOwnStatus = iDataOwnStatus;
    copy->iTransDataType = iTransDataType;
    
    if ( iJavaHash )
        {
        if ( copy->iJavaHash )
            {
            delete copy->iJavaHash;
            copy->iJavaHash = NULL;
            }
        copy->iJavaHash = iJavaHash->AllocL();
        }
    CleanupStack::Pop( copy );
    return copy;
    }
    
// -----------------------------------------------------------------------------
// CSConUpdateDeviceInfo::CSConUpdateDeviceInfo()
// 
// -----------------------------------------------------------------------------
//  
CSConUpdateDeviceInfo::CSConUpdateDeviceInfo()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConUpdateDeviceInfo::~CSConUpdateDeviceInfo()
// 
// -----------------------------------------------------------------------------
//           
CSConUpdateDeviceInfo::~CSConUpdateDeviceInfo()
    {
    }

// -----------------------------------------------------------------------------
// CSConUpdateDeviceInfo::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConUpdateDeviceInfo* CSConUpdateDeviceInfo::CopyL()
    {
    CSConUpdateDeviceInfo* copy = new (ELeave) CSConUpdateDeviceInfo();
    copy->iVersion.Copy( iVersion );
    copy->iInstallSupp = iInstallSupp;
    copy->iUninstallSupp = iUninstallSupp;
    copy->iInstParamsSupp = iInstParamsSupp;
    copy->iInstAppsSupp = iInstAppsSupp;
    copy->iDataOwnersSupp = iDataOwnersSupp;
    copy->iSetBURModeSupp = iSetBURModeSupp;
    copy->iGetSizeSupp = iGetSizeSupp;
    copy->iReqDataSupp = iReqDataSupp;
    copy->iSupplyDataSupp = iSupplyDataSupp;
    copy->iMaxObjectSize = iMaxObjectSize;
    copy->iRebootSupp = iRebootSupp;
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;        
                
    return copy;
    }

CSConInstApp::~CSConInstApp()
    {
    delete iWidgetBundleId;
    }
// -----------------------------------------------------------------------------
// CSConInstApp::Copy()
// 
// -----------------------------------------------------------------------------
//  
CSConInstApp* CSConInstApp::CopyL()
    {
    CSConInstApp* copy = new (ELeave) CSConInstApp();
            
    copy->iName.Copy( iName ); 
    copy->iParentName.Copy( iParentName );
    copy->iVendor.Copy( iVendor );
    copy->iVersion.Copy( iVersion );
    copy->iSize = iSize;
    copy->iType = iType;
    copy->iUid = iUid;
    if ( iWidgetBundleId )
        {
        copy->iWidgetBundleId = iWidgetBundleId->AllocL();
        }
    else
        {
        copy->iWidgetBundleId = NULL;
        }

    return copy;
    }

// -----------------------------------------------------------------------------
// CSConListInstApps::CSConListInstApps()
// 
// -----------------------------------------------------------------------------
//      
CSConListInstApps::CSConListInstApps()
    {
    //Initialize iDriveList with zeros
    iDriveList.Fill( '\x0' );
    }

// -----------------------------------------------------------------------------
// CSConListInstApps::~CSConListInstApps()
// 
// -----------------------------------------------------------------------------
//      
CSConListInstApps::~CSConListInstApps()
    {
    iApps.ResetAndDestroy();
    iApps.Close();
    }

// -----------------------------------------------------------------------------
// CSConListInstApps::CopyL()
// 
// -----------------------------------------------------------------------------
//              
CSConListInstApps* CSConListInstApps::CopyL()
    {
    CSConListInstApps* copy = new (ELeave) CSConListInstApps();
    CleanupStack::PushL( copy );
    copy->iAllApps = iAllApps;
    copy->iDriveList = iDriveList;
            
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;            
        
    for( TInt i = 0; i < iApps.Count(); i++ )
        {
        CSConInstApp* temp = iApps[i]->CopyL();
        CleanupStack::PushL( temp );
        copy->iApps.AppendL( temp );
        CleanupStack::Pop( temp );
        }
    CleanupStack::Pop( copy );
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConFile::CSConFile
// 
// -----------------------------------------------------------------------------
//  
CSConFile::CSConFile()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConFile::~CSConFile
// 
// -----------------------------------------------------------------------------
//  
CSConFile::~CSConFile()
    {
    
    }
    
// -----------------------------------------------------------------------------
// CSConFile::Copy()
// 
// -----------------------------------------------------------------------------
//  
CSConFile* CSConFile::CopyL()
    {
    CSConFile* copy = new (ELeave) CSConFile();
            
    copy->iPath.Copy( iPath ); 
    copy->iModified.Copy( iModified );
    copy->iSize = iSize;
    copy->iUserPerm = iUserPerm;

    return copy;
    }

// -----------------------------------------------------------------------------
// CSConInstall::CSConInstall()
// 
// -----------------------------------------------------------------------------
//      
CSConInstall::CSConInstall() : iMode( EUnknown ) 
    {
    }

// -----------------------------------------------------------------------------
// CSConInstall::~CSConInstall()
// 
// -----------------------------------------------------------------------------
//              
CSConInstall::~CSConInstall()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CSConInstall::CopyL()
// 
// -----------------------------------------------------------------------------
//              
CSConInstall* CSConInstall::CopyL()
    {
    CSConInstall* copy = new (ELeave) CSConInstall();
    copy->iPath = iPath;
    copy->iMode = iMode;
            
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
    
    if ( iData )
        {
        copy->iData = iData->Alloc();
        }
            
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConUninstall::CSConUninstall()
// 
// -----------------------------------------------------------------------------
//  
CSConUninstall::CSConUninstall() : iMode( EUnknown )
    {
    }

// -----------------------------------------------------------------------------
// CSConUninstall::~CSConUninstall()
// 
// -----------------------------------------------------------------------------
//          
CSConUninstall::~CSConUninstall()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CSConUninstall::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConUninstall* CSConUninstall::CopyL()
    {
    CSConUninstall* copy = new (ELeave) CSConUninstall();
    copy->iName = iName;
    copy->iVendor = iVendor;
    copy->iUid = iUid;
    copy->iMode = iMode;
            
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
    
    if ( iData )
        {
        copy->iData = iData->Alloc();
        }
            
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConListDataOwners::CSConListDataOwners()
// 
// -----------------------------------------------------------------------------
//      
CSConListDataOwners::CSConListDataOwners()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConListDataOwners::~CSConListDataOwners()
// 
// -----------------------------------------------------------------------------
//          
CSConListDataOwners::~CSConListDataOwners()
    {
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }

// -----------------------------------------------------------------------------
// CSConListDataOwners::CopyL()
// 
// -----------------------------------------------------------------------------
//              
CSConListDataOwners* CSConListDataOwners::CopyL()
    {
    CSConListDataOwners* copy = new (ELeave) CSConListDataOwners();
    CleanupStack::PushL( copy );
    for( TInt i = 0; i < iDataOwners.Count(); i++ )
        {
        CSConDataOwner* temp = iDataOwners[i]->CopyL();
        CleanupStack::PushL( temp );
        copy->iDataOwners.AppendL( temp );
        CleanupStack::Pop( temp );
        }
    CleanupStack::Pop( copy );
    
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
        
    return copy;
    }
    
// -----------------------------------------------------------------------------
// CSConListDataOwners::DeleteDataOwners()
// 
// -----------------------------------------------------------------------------
//          
void CSConListDataOwners::DeleteDataOwners()
    {
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }
    
// -----------------------------------------------------------------------------
// CCSConSetBURMode::CSConSetBURMode()
// 
// -----------------------------------------------------------------------------
//      
CSConSetBURMode::CSConSetBURMode()
    {
    //Initialize iDriveList with zeros
    iDriveList.Fill( '\x0' );
    }
        
// -----------------------------------------------------------------------------
// CSConSetBURMode::~CSConSetBURMode()
// 
// -----------------------------------------------------------------------------
//      
CSConSetBURMode::~CSConSetBURMode()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConSetBURMode::Copy()
// 
// -----------------------------------------------------------------------------
//      
CSConSetBURMode* CSConSetBURMode::CopyL()
    {
    CSConSetBURMode* copy = new (ELeave) CSConSetBURMode();
    copy->iDriveList.Copy( iDriveList );
    copy->iPartialType = iPartialType;
    copy->iIncType = iIncType;
            
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;

    return copy;
    }

// -----------------------------------------------------------------------------
// CSConGetDataSize::CSConGetDataSize()
// 
// -----------------------------------------------------------------------------
//          
CSConGetDataSize::CSConGetDataSize()
    {
    }

// -----------------------------------------------------------------------------
// CSConGetDataSize::~CSConGetDataSize()
// 
// -----------------------------------------------------------------------------
//      
CSConGetDataSize::~CSConGetDataSize()
    {
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }
        
// -----------------------------------------------------------------------------
// CSConGetDataSize::Copy()
// 
// -----------------------------------------------------------------------------
//      
CSConGetDataSize* CSConGetDataSize::CopyL()
    {
    CSConGetDataSize* copy = new (ELeave) CSConGetDataSize();
    CleanupStack::PushL( copy );
    for( TInt i = 0; i < iDataOwners.Count(); i++ )
        {
        CSConDataOwner* temp = iDataOwners[i]->CopyL();
        CleanupStack::PushL( temp );
        copy->iDataOwners.AppendL( temp );
        CleanupStack::Pop( temp );
        }
    CleanupStack::Pop( copy );
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
        
    return copy;
    }
    
// -----------------------------------------------------------------------------
// CSConGetDataSize::DeleteDataOwners()
// 
// -----------------------------------------------------------------------------
//              
void CSConGetDataSize::DeleteDataOwners()
    {
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }   
    
// -----------------------------------------------------------------------------
// CSConListPublicFiles::CSConListPublicFiles()
// 
// -----------------------------------------------------------------------------
//  
CSConListPublicFiles::CSConListPublicFiles()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConListPublicFiles::~CSConListPublicFiles()
// 
// -----------------------------------------------------------------------------
//          
CSConListPublicFiles::~CSConListPublicFiles()
    {
    iFiles.ResetAndDestroy();
    iFiles.Close();
    
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }

// -----------------------------------------------------------------------------
// CSConListPublicFiles::CopyL()
// 
// -----------------------------------------------------------------------------
//              
CSConListPublicFiles* CSConListPublicFiles::CopyL()
    {
    CSConListPublicFiles* copy = new (ELeave) CSConListPublicFiles();
    CleanupStack::PushL( copy );
    for( TInt i = 0; i < iFiles.Count(); i++ )
        {
        CSConFile* temp = iFiles[i]->CopyL();
        CleanupStack::PushL( temp );
        copy->iFiles.AppendL( temp );
        CleanupStack::Pop( temp );
        }
        
    for( TInt j = 0; j < iDataOwners.Count(); j++ )
        {
        CSConDataOwner* temp = iDataOwners[j]->CopyL();
        CleanupStack::PushL( temp );
        copy->iDataOwners.AppendL( temp );
        CleanupStack::Pop( temp );
        }
    CleanupStack::Pop( copy );
    
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
        
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConRequestData::CSConRequestData()
// 
// -----------------------------------------------------------------------------
//
CSConRequestData::CSConRequestData()
    {
    iDataOwner = new CSConDataOwner();
    }
            
// -----------------------------------------------------------------------------
// CSConRequestData::~CSConRequestData()
// 
// -----------------------------------------------------------------------------
//
CSConRequestData::~CSConRequestData()
    {
    if ( iDataOwner )
        {
        delete iDataOwner;
        iDataOwner = NULL;
        }
        
    if ( iBackupData )
        {
        delete iBackupData;
        iBackupData = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConRequestData::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConRequestData* CSConRequestData::CopyL()
    {
    CSConRequestData* copy = new (ELeave) CSConRequestData();
    
    if ( iDataOwner )
        {
        if ( copy->iDataOwner )
            {
            delete copy->iDataOwner;
            copy->iDataOwner = NULL;
            }
        copy->iDataOwner = iDataOwner->CopyL();
        }
    
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
        
    if ( iBackupData )
        {
        if ( copy->iBackupData )
            {
            delete copy->iBackupData;
            copy->iBackupData = NULL;
            }
        copy->iBackupData = iBackupData->Alloc();
        }
        
    copy->iMoreData = iMoreData;
    
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConRequestData::DeleteDataAndDataOwner()
// 
// -----------------------------------------------------------------------------
//          
void CSConRequestData::DeleteDataAndDataOwner()
    {
    if ( iDataOwner )
        {
        delete iDataOwner;
        iDataOwner = NULL;
        }
    
    if ( iBackupData )
        {
        delete iBackupData;
        iBackupData = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSConGetDataOwnerStatus::CSConGetDataOwnerStatus()
// 
// -----------------------------------------------------------------------------
//      
CSConGetDataOwnerStatus::CSConGetDataOwnerStatus()
    {
    }

// -----------------------------------------------------------------------------
// CSConGetDataOwnerStatus::~CSConGetDataOwnerStatus()
// 
// -----------------------------------------------------------------------------
//              
CSConGetDataOwnerStatus::~CSConGetDataOwnerStatus()
    {
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }

// -----------------------------------------------------------------------------
// CSConGetDataOwnerStatus::Copy()
// 
// -----------------------------------------------------------------------------
//              
CSConGetDataOwnerStatus* CSConGetDataOwnerStatus::CopyL()
    {
    CSConGetDataOwnerStatus* copy = new (ELeave) CSConGetDataOwnerStatus();
    CleanupStack::PushL( copy );
    for( TInt i = 0; i < iDataOwners.Count(); i++ )
        {
        CSConDataOwner* temp = iDataOwners[i]->CopyL();
        CleanupStack::PushL( temp );
        copy->iDataOwners.AppendL( temp );
        CleanupStack::Pop( temp );
        }           
    CleanupStack::Pop( copy );
    
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
        
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConGetDataOwnerStatus::DeleteDataOwners()
// 
// -----------------------------------------------------------------------------
//              
void CSConGetDataOwnerStatus::DeleteDataOwners()
    {
    iDataOwners.ResetAndDestroy();
    iDataOwners.Close();
    }
    
// -----------------------------------------------------------------------------
// CSConSupplyData::CSConSupplyData()
// 
// -----------------------------------------------------------------------------
//  
CSConSupplyData::CSConSupplyData()
    {
    iDataOwner = new CSConDataOwner();
    }
    
// -----------------------------------------------------------------------------
// CSConSupplyData::~CSConSupplyData()
// 
// -----------------------------------------------------------------------------
//          
CSConSupplyData::~CSConSupplyData()
    {
    if ( iDataOwner )
        {
        delete iDataOwner;
        iDataOwner = NULL;
        }
    
    if ( iRestoreData )
        {
        delete iRestoreData;
        iRestoreData = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConSupplyData::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConSupplyData* CSConSupplyData::CopyL()
    {
    CSConSupplyData* copy = new (ELeave) CSConSupplyData();
    CleanupStack::PushL( copy );
    
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
        
    if ( iDataOwner )
        {
        if ( copy->iDataOwner )
            {
            delete copy->iDataOwner;
            copy->iDataOwner = NULL;
            }
            
        copy->iDataOwner = iDataOwner->CopyL();
        }
    CleanupStack::Pop( copy ); 
        
    if ( iRestoreData )
        {
        if ( copy->iRestoreData )
            {
            delete copy->iRestoreData;
            copy->iRestoreData = NULL;
            }
            
        copy->iRestoreData = iRestoreData->Alloc();
        }
    
    copy->iMoreData = iMoreData;    
    
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConGetMetadata::CSConGetMetadata()
// 
// -----------------------------------------------------------------------------
//  
CSConGetMetadata::CSConGetMetadata()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConGetMetadata::~CSConGetMetadata()
// 
// -----------------------------------------------------------------------------
//          
CSConGetMetadata::~CSConGetMetadata()
    {
    delete iData;
    }
    
// -----------------------------------------------------------------------------
// CSConGetMetadata::Copy()
// 
// -----------------------------------------------------------------------------
//          
CSConGetMetadata* CSConGetMetadata::CopyL()
    {
    CSConGetMetadata* copy = new (ELeave) CSConGetMetadata();
    
    copy->iFilename = iFilename;
    
    if ( iData )
        {
        if ( copy->iData )
            {
            delete copy->iData;
            copy->iData = NULL;
            }
        
        copy->iData = iData->Alloc();
        }
    
    copy->iMoreData = iMoreData;
    copy->iComplete = iComplete;
    copy->iProgress = iProgress;
    
    return copy;
    }


// -----------------------------------------------------------------------------
// CSConTask::NewL( TSConMethodName aMethod )
// 
// -----------------------------------------------------------------------------
//
CSConTask* CSConTask::NewL( TSConMethodName aMethod )
    {
    CSConTask* self = NewLC( aMethod );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSConTask::NewLC( TSConMethodName aMethod )
// 
// -----------------------------------------------------------------------------
//
CSConTask* CSConTask::NewLC( TSConMethodName aMethod )
    {
    CSConTask* self = new (ELeave) CSConTask();
    CleanupStack::PushL( self );
    self->ConstructL( aMethod );
    return self;
    }

// -----------------------------------------------------------------------------
// CSConTask::ConstructL( TSConMethodName aMethod )
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConTask::ConstructL( TSConMethodName aMethod )
    {
    iMethod = aMethod;
    
    switch( aMethod )
        {
        case ECancel :
            break;
        case EGetDataOwnerStatus :
            iGetDataOwnerParams = new (ELeave) CSConGetDataOwnerStatus();
            break;
        case EGetDataSize :
            iGetDataSizeParams = new (ELeave) CSConGetDataSize();
            break;
        case EGetStatus :
            iGetStatusParams = new (ELeave) CSConGetStatus();
            break;
        case EInstall :
            iInstallParams = new (ELeave) CSConInstall();
            break;
        case EListDataOwners :
            iListDataOwnersParams = new (ELeave) CSConListDataOwners();
            break;
        case EListInstalledApps :
            iListAppsParams = new (ELeave) CSConListInstApps();
            break;
        case EListPublicFiles :
            iPubFilesParams = new (ELeave) CSConListPublicFiles();
            break;
        case ERequestData :
            iRequestDataParams = new (ELeave) CSConRequestData();
            break;
        case ESetBURMode :
            iBURModeParams = new (ELeave) CSConSetBURMode();
            break;
        case ESetInstParams :
            break;
        case ESupplyData :
            iSupplyDataParams = new (ELeave) CSConSupplyData();
            break;
        case EUninstall :
            iUninstallParams = new (ELeave) CSConUninstall();
            break;
        case EUpdateDeviceInfo :
            iDevInfoParams = new (ELeave) CSConUpdateDeviceInfo();
            break;
        case EReboot :
            iRebootParams = new (ELeave) CSConReboot();
            break;
        case EGetMetadata :
            iGetMetadataParams = new (ELeave) CSConGetMetadata();
            break;
        default :
            break;
        }
    }
// -----------------------------------------------------------------------------
// CSConTask::CSConTask()
// 
// -----------------------------------------------------------------------------
//      
CSConTask::CSConTask()
    {
    }

// -----------------------------------------------------------------------------
// CSConTask::~CSConTask()
// 
// -----------------------------------------------------------------------------
//          
CSConTask::~CSConTask()
    {
    delete iDevInfoParams;
    delete iListAppsParams;
    delete iGetStatusParams;
    delete iInstallParams;
    delete iUninstallParams;
    delete iBURModeParams;
    delete iGetDataSizeParams;
    delete iRequestDataParams;
    delete iGetDataOwnerParams;
    delete iSupplyDataParams;
    delete iPubFilesParams;
    delete iListDataOwnersParams;
    delete iRebootParams;
    delete iGetMetadataParams;
    }

// -----------------------------------------------------------------------------
// CSConTask::GetServiceId() const
// 
// -----------------------------------------------------------------------------
//          
TSConMethodName CSConTask::GetServiceId() const
    { 
    return iMethod; 
    }

// -----------------------------------------------------------------------------
// CSConTask::Copy() const
// 
// -----------------------------------------------------------------------------
//          
CSConTask* CSConTask::CopyL() const
    {
    CSConTask* copy = new (ELeave) CSConTask();
    copy->iMethod = iMethod;
    
    if ( iDevInfoParams )
        {
        copy->iDevInfoParams = iDevInfoParams->CopyL();
        }
    if ( iListAppsParams )
        {
        copy->iListAppsParams = iListAppsParams->CopyL();
        }
    if ( iGetStatusParams ) 
        {
        
        }
    if ( iInstallParams )
        {
        copy->iInstallParams = iInstallParams->CopyL();
        }
    if ( iUninstallParams )
        {
        copy->iUninstallParams = iUninstallParams->CopyL();
        }
    if ( iBURModeParams )
        {
        copy->iBURModeParams = iBURModeParams->CopyL();
        }
    if ( iGetDataSizeParams )
        {
        copy->iGetDataSizeParams = iGetDataSizeParams->CopyL();
        }
    if ( iRequestDataParams )
        {
        copy->iRequestDataParams = iRequestDataParams->CopyL();
        }
    if ( iGetDataOwnerParams )
        {
        copy->iGetDataOwnerParams = iGetDataOwnerParams->CopyL();
        }
    if ( iSupplyDataParams )
        {
        copy->iSupplyDataParams = iSupplyDataParams->CopyL();
        }
    if ( iPubFilesParams )
        {
        copy->iPubFilesParams = iPubFilesParams->CopyL();
        }
    if ( iListDataOwnersParams )
        {
        copy->iListDataOwnersParams = iListDataOwnersParams->CopyL();
        }
    if ( iRebootParams )
        {
        copy->iRebootParams = iRebootParams->CopyL();
        }
    if ( iGetMetadataParams )
        {
        copy->iGetMetadataParams = iGetMetadataParams->CopyL();
        }
    
    return copy;
    }
            
// -----------------------------------------------------------------------------
// CSConTask::GetComplete()
// 
// -----------------------------------------------------------------------------
//  
TBool CSConTask::GetComplete()
    {
    TBool complete( EFalse );
    
    switch( iMethod )
        {
        case EInstall :
            complete = this->iInstallParams->iComplete;
            break;
        case EUninstall :
            complete =  this->iUninstallParams->iComplete;
            break;
        case EListInstalledApps :
            complete =  this->iListAppsParams->iComplete;
            break;
        case ESetInstParams :
            break;
        case ESetBURMode :
            complete = this->iBURModeParams->iComplete;
            break;
        case EListPublicFiles :
            complete = this->iPubFilesParams->iComplete;
            break;
        case EListDataOwners :
            complete = this->iListDataOwnersParams->iComplete;
            break;
        case EGetDataSize :
            complete = this->iGetDataSizeParams->iComplete;
            break;
        case EReboot :
            complete = this->iRebootParams->iComplete;
            break;
        case ERequestData :
            //If task is partially completed, 
            //it can be removed from the queue
            if ( this->iRequestDataParams->iProgress == KSConTaskPartiallyCompleted )
                {
                complete = ETrue;
                }
            else
                {
                complete = this->iRequestDataParams->iComplete;
                }
            
            break;
        case EGetDataOwnerStatus :
            complete = this->iGetDataOwnerParams->iComplete;
            break;
        case ESupplyData :
            //If task is partially completed, 
            //it can be removed from the queue
            if ( this->iSupplyDataParams->iProgress == KSConTaskPartiallyCompleted )
                {
                complete = ETrue;
                }
            else
                {
                complete = this->iSupplyDataParams->iComplete;
                }
            
            break;
        case EGetMetadata :
            complete = this->iGetMetadataParams->iComplete;
            break;
        default :
            break;                      
        }
    
    return complete;
    }
    
// -----------------------------------------------------------------------------
// CSConTask::SetCompleteValue( TBool aValue )
// 
// -----------------------------------------------------------------------------
//          
void CSConTask::SetCompleteValue( TBool aValue )   
    {
    switch( iMethod )
        {
        case EInstall :
            this->iInstallParams->iComplete = aValue;
            break;
        case EUninstall :
            this->iUninstallParams->iComplete = aValue;         
            break;
        case EListInstalledApps :
            this->iListAppsParams->iComplete = aValue;
            break;
        case ESetInstParams :
            this->iInstallParams->iComplete = aValue;
            break;
        case ESetBURMode :
            this->iBURModeParams->iComplete = aValue;
            break;
        case EListPublicFiles :
            this->iPubFilesParams->iComplete = aValue;
            break;
        case EListDataOwners :
            this->iListDataOwnersParams->iComplete = aValue;
            break;
        case EGetDataSize :
            this->iGetDataSizeParams->iComplete = aValue;
            break;
        case EReboot :
            this->iRebootParams->iComplete = aValue;
            break;
        case ERequestData :
            this->iRequestDataParams->iComplete = aValue;
            break;
        case EGetDataOwnerStatus :
            this->iGetDataOwnerParams->iComplete = aValue;
            break;
        case ESupplyData :
            this->iSupplyDataParams->iComplete = aValue;
            break;
        case EGetMetadata :
            this->iGetMetadataParams->iComplete = aValue;
            break;
        default:
            break;      
        }
    }
        
// -----------------------------------------------------------------------------
// CSConTask::GetCompleteValue()
// 
// -----------------------------------------------------------------------------
//  
TBool CSConTask::GetCompleteValue()
    {
    TBool complete( EFalse );
    
    switch( iMethod )
        {
        case EInstall :
            complete = this->iInstallParams->iComplete;
            break;
        case EUninstall :
            complete = this->iUninstallParams->iComplete;           
            break;
        case EListInstalledApps :
            complete = this->iListAppsParams->iComplete;
            break;
        case ESetInstParams :
            complete = this->iInstallParams->iComplete;
            break;
        case ESetBURMode :
            complete = this->iBURModeParams->iComplete;
            break;
        case EListPublicFiles :
            complete = this->iPubFilesParams->iComplete;
            break;
        case EListDataOwners :
            complete = this->iListDataOwnersParams->iComplete;
            break;
        case EGetDataSize :
            complete = this->iGetDataSizeParams->iComplete;
            break;
        case EReboot :
            complete = this->iRebootParams->iComplete;
            break;
        case ERequestData :
            if ( this->iRequestDataParams->iProgress != KSConTaskPartiallyCompleted )
                {
                complete = this->iRequestDataParams->iComplete;
                }
            else
                {
                complete = ETrue;
                }
            
            break;
        case EGetDataOwnerStatus :
            complete = this->iGetDataOwnerParams->iComplete;
            break;
        case ESupplyData :
            if ( this->iSupplyDataParams->iProgress != KSConTaskPartiallyCompleted )
                {
                complete = this->iSupplyDataParams->iComplete;
                }
            else
                {
                complete = ETrue;
                }
            break;
        case EGetMetadata :
            complete = this->iGetMetadataParams->iComplete;
            break;
        default:
            break;      
        }
    return complete;
    }

// -----------------------------------------------------------------------------
// CSConTask::SetProgressValue( TInt aValue )
// 
// -----------------------------------------------------------------------------
//              
void CSConTask::SetProgressValue( TInt aValue )
    {
    switch( iMethod )
        {
        case EInstall :
            this->iInstallParams->iProgress = aValue;
            break;
        case EUninstall :
            this->iUninstallParams->iProgress = aValue;
            break;
        case EListInstalledApps :
            this->iListAppsParams->iProgress = aValue;
            break;
        case ESetInstParams :
            this->iInstallParams->iProgress = aValue;
            break;
        case ESetBURMode :
            this->iBURModeParams->iProgress = aValue;
            break;
        case EListPublicFiles :
            this->iPubFilesParams->iProgress = aValue;
            break;
        case EListDataOwners :
            this->iListDataOwnersParams->iProgress = aValue;
            break;
        case EGetDataSize :
            this->iGetDataSizeParams->iProgress = aValue;
            break;
        case EReboot :
            this->iRebootParams->iProgress = aValue;
            break;
        case ERequestData :
            this->iRequestDataParams->iProgress = aValue;
            break;
        case EGetDataOwnerStatus :
            this->iGetDataOwnerParams->iProgress = aValue;
            break;
        case ESupplyData :
            this->iSupplyDataParams->iProgress = aValue;
            break;
        case EGetMetadata :
            this->iGetMetadataParams->iProgress = aValue;
            break;
        default:
            break;      
        }
    }

// -----------------------------------------------------------------------------
// CSConTaskReply::CSConTaskReply()
// 
// -----------------------------------------------------------------------------
//      
CSConTaskReply::CSConTaskReply()
    {
    }
   
// -----------------------------------------------------------------------------
// CSConTaskReply::CSConTaskReply( TSConMethodName aMethod )
// 
// -----------------------------------------------------------------------------
//          
CSConTaskReply::CSConTaskReply( TSConMethodName aMethod )
    {
    iMethod = aMethod;
    switch( aMethod )
        {
        case ECancel :
            break;
        case EGetDataOwnerStatus :
            iGetDataOwnerParams = new CSConGetDataOwnerStatus();
            break;
        case EGetDataSize :
            iGetDataSizeParams = new CSConGetDataSize();
            break;
        case EGetStatus :
            iGetStatusParams = new CSConGetStatus();
            break;
        case EInstall :
            iInstallParams = new CSConInstall();
            break;
        case EListDataOwners :
            iListDataOwnersParams = new CSConListDataOwners();
            break;
        case EListInstalledApps :
            iListAppsParams = new CSConListInstApps();
            break;
        case EListPublicFiles :
            iPubFilesParams = new CSConListPublicFiles();
            break;
        case ERequestData :
            iRequestDataParams = new CSConRequestData();
            break;
        case ESetBURMode :
            iBURModeParams = new CSConSetBURMode();
            break;
        case ESetInstParams :
            break;
        case ESupplyData :
            iSupplyDataParams = new CSConSupplyData();
            break;
        case EUninstall :
            iUninstallParams = new CSConUninstall();
            break;
        case EUpdateDeviceInfo :
            iDevInfoParams = new CSConUpdateDeviceInfo();
            break;
        case EReboot :
            iRebootParams = new CSConReboot();
            break;
        case EGetMetadata :
            iGetMetadataParams = new CSConGetMetadata();
            break;
        default :
            break;
        }
    }
      
// -----------------------------------------------------------------------------
// CSConTaskReply::~CSConTaskReply()
// 
// -----------------------------------------------------------------------------
//          
CSConTaskReply::~CSConTaskReply()
    {
    delete iDevInfoParams;
    delete iListAppsParams;
    delete iGetStatusParams;
    delete iInstallParams;
    delete iUninstallParams;
    delete iBURModeParams;
    delete iGetDataSizeParams;
    delete iRequestDataParams;
    delete iGetDataOwnerParams;
    delete iSupplyDataParams;
    delete iPubFilesParams;
    delete iListDataOwnersParams;
    delete iRebootParams;
    delete iGetMetadataParams;
    }

// -----------------------------------------------------------------------------
// CSConTaskReply::Initialize( const CSConTask& aTask )
// 
// -----------------------------------------------------------------------------
// 
void CSConTaskReply::InitializeL( const CSConTask& aTask )
    {
    iTaskId = aTask.iTaskId;
    iMethod = aTask.iMethod;
    
    if ( iMethod == EInstall )
        {
        if ( iInstallParams )
            {
            delete iInstallParams;
            iInstallParams = NULL;
            }
        iInstallParams = aTask.iInstallParams->CopyL();
        }
    else if ( iMethod == EListInstalledApps )
        {
        if ( iListAppsParams )
            {
            delete iListAppsParams;
            iListAppsParams = NULL;
            }
            
        iListAppsParams = aTask.iListAppsParams->CopyL();
        }
    else if ( iMethod == EUninstall )
        {
        if ( iUninstallParams )
            {
            delete iUninstallParams;
            iUninstallParams = NULL;
            }
            
        iUninstallParams = aTask.iUninstallParams->CopyL();
        }
    else if ( iMethod == ESetBURMode )
        {
        if ( iBURModeParams )
            {
            delete iBURModeParams;
            iBURModeParams = NULL;
            }
            
        iBURModeParams = aTask.iBURModeParams->CopyL();
        }
    else if ( iMethod == EListPublicFiles )
        {
        if ( iPubFilesParams )
            {
            delete iPubFilesParams;
            iPubFilesParams = NULL;
            }
        
        iPubFilesParams = aTask.iPubFilesParams->CopyL();
        }
    else if ( iMethod == EListDataOwners )
        {
        if ( iListDataOwnersParams )
            {
            delete iListDataOwnersParams;
            iListDataOwnersParams = NULL;
            }
        
        iListDataOwnersParams = aTask.iListDataOwnersParams->CopyL();
        }
    else if ( iMethod == EGetDataSize )
        {
        if ( iGetDataSizeParams )
            {
            delete iGetDataSizeParams;
            iGetDataSizeParams = NULL;
            }
        
        iGetDataSizeParams = aTask.iGetDataSizeParams->CopyL();
        }
    else if ( iMethod == EReboot )
        {
        if ( iRebootParams )
            {
            delete iRebootParams;
            iRebootParams = NULL;
            }
        }
    else if ( iMethod == ERequestData )
        {
        if ( iRequestDataParams )
            {
            delete iRequestDataParams;
            iRequestDataParams = NULL;
            }
        
        iRequestDataParams = aTask.iRequestDataParams->CopyL();
        }
    else if ( iMethod == EGetDataOwnerStatus )
        {
        if ( iGetDataOwnerParams )
            {
            delete iGetDataOwnerParams;
            iGetDataOwnerParams = NULL;
            }
            
        iGetDataOwnerParams = aTask.iGetDataOwnerParams->CopyL();
        }
    else if ( iMethod == ESupplyData )
        {
        if ( iSupplyDataParams )
            {
            delete iSupplyDataParams;
            iSupplyDataParams = NULL;
            }
        
        iSupplyDataParams = aTask.iSupplyDataParams->CopyL();
        }
    else if ( iMethod == EGetMetadata )
        {
        if ( iGetMetadataParams )
            {
            delete iGetMetadataParams;
            iGetMetadataParams = NULL;
            }
        
        iGetMetadataParams = aTask.iGetMetadataParams->CopyL();
        }
    }
 
// -----------------------------------------------------------------------------
// CSConTaskReply::Initialize( TSConMethodName aMethod, 
//              TInt aProgress, TBool aComplete )
// 
// -----------------------------------------------------------------------------
//              
void CSConTaskReply::InitializeL( TSConMethodName aMethod, 
                TInt aProgress, TBool aComplete )
    {
    iMethod = aMethod;
    if ( aMethod == EUpdateDeviceInfo )
        {
        if ( !iDevInfoParams )
            {
            iDevInfoParams = new (ELeave) CSConUpdateDeviceInfo();
            }
            
        iDevInfoParams->iComplete = aComplete;
        iDevInfoParams->iProgress = aProgress;
        }
    else if ( aMethod == EReboot )
        {
        if ( !iRebootParams )
            {
            iRebootParams = new (ELeave) CSConReboot();
            }
            
        iRebootParams->iComplete = aComplete;
        iRebootParams->iProgress = aProgress;
        }
    else if ( aMethod == EGetMetadata )
        {
        if ( !iGetMetadataParams )
            {
            iGetMetadataParams = new (ELeave) CSConGetMetadata();
            }
            
        iGetMetadataParams->iComplete = aComplete;
        iGetMetadataParams->iProgress = aProgress;
        }
    }

// -----------------------------------------------------------------------------
// CSConTaskReply::CopyAndFree()
// 
// -----------------------------------------------------------------------------
// 
CSConTaskReply* CSConTaskReply::CopyAndFreeL()  
    {
    CSConTaskReply* copy = new (ELeave) CSConTaskReply();
    CleanupStack::PushL( copy );
    copy->iTaskId = iTaskId;
    copy->iMethod = iMethod;
    
    if ( iDevInfoParams )
        {
        copy->iDevInfoParams = iDevInfoParams->CopyL();
        
        //free allocated memory
        delete iDevInfoParams;
        iDevInfoParams = NULL;
        }
    if ( iListAppsParams )
        {
        copy->iListAppsParams = iListAppsParams->CopyL();
        
        //free allocated memory
        delete iListAppsParams;
        iListAppsParams = NULL;
        }
    if ( iInstallParams )
        {
        copy->iInstallParams = iInstallParams->CopyL();
        
        //free allocated memory
        delete iInstallParams;
        iInstallParams = NULL;
        }
    if ( iUninstallParams )
        {
        copy->iUninstallParams = iUninstallParams->CopyL();
        
        //free allocated memory
        delete iUninstallParams;
        iUninstallParams = NULL;
        }
    if ( iBURModeParams )
        {
        copy->iBURModeParams = iBURModeParams->CopyL();
        
        //free allocated memory
        delete iBURModeParams;
        iBURModeParams = NULL;
        }
    if ( iGetDataSizeParams )
        {
        copy->iGetDataSizeParams = iGetDataSizeParams->CopyL();
        
        //free allocated memory
        delete iGetDataSizeParams;
        iGetDataSizeParams = NULL;
        }
    if ( iRequestDataParams )
        {
        copy->iRequestDataParams = iRequestDataParams->CopyL();
        
        //free allocated memory
        delete iRequestDataParams;
        iRequestDataParams = NULL;
        }
    if ( iGetDataOwnerParams )
        {
        copy->iGetDataOwnerParams = iGetDataOwnerParams->CopyL();
        
        //free allocated memory
        delete iGetDataOwnerParams;
        iGetDataOwnerParams = NULL;
        }
    if ( iSupplyDataParams )
        {
        copy->iSupplyDataParams = iSupplyDataParams->CopyL();
        
        //free allocated memory
        delete iSupplyDataParams;
        iSupplyDataParams = NULL;
        }
    if ( iPubFilesParams )
        {
        copy->iPubFilesParams = iPubFilesParams->CopyL();
        
        //free allocated memory
        delete iPubFilesParams;
        iPubFilesParams = NULL;
        }
    if ( iListDataOwnersParams )
        {
        copy->iListDataOwnersParams = iListDataOwnersParams->CopyL();
        
        //free allocated memory
        delete iListDataOwnersParams;
        iListDataOwnersParams = NULL;
        }
    if ( iRebootParams )
        {
        copy->iRebootParams = iRebootParams->CopyL();
        
        //free allocated memory
        delete iRebootParams;
        iRebootParams = NULL;
        }
    if ( iGetMetadataParams )
        {
        copy->iGetMetadataParams = iGetMetadataParams->CopyL();
        
        //free allocated memory
        delete iGetMetadataParams;
        iGetMetadataParams = NULL;
        }
    CleanupStack::Pop( copy );
    return copy;
    }

// -----------------------------------------------------------------------------
// CSConTaskReply::CleanTaskData()
// 
// -----------------------------------------------------------------------------
// 
void CSConTaskReply::CleanTaskData()
    {
    switch( iMethod )   
        {
        case EGetDataSize :
            if ( iGetDataSizeParams )
                {
                this->iGetDataSizeParams->DeleteDataOwners();
                }
            break;
        case EGetDataOwnerStatus :
            if ( iGetDataOwnerParams )
                {
                this->iGetDataOwnerParams->DeleteDataOwners();
                }
            break;
        case EListDataOwners :
            if ( iListDataOwnersParams )
                {
                this->iListDataOwnersParams->DeleteDataOwners();
                }
            break;
        case ERequestData :
            if ( iRequestDataParams )
                {
                this->iRequestDataParams->DeleteDataAndDataOwner();
                }
            break;
        default :
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConStatusReply::CSConStatusReply()
// 
// -----------------------------------------------------------------------------
// 
CSConStatusReply::CSConStatusReply()
    {
    }
   
// -----------------------------------------------------------------------------
// CSConStatusReply::~CSConStatusReply()
// 
// -----------------------------------------------------------------------------
//      
CSConStatusReply::~CSConStatusReply() 
    {
    iTasks.ResetAndDestroy();
    iTasks.Close();
    };

// End of file
