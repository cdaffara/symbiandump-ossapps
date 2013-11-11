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
* Description:  Header file for ConML tasks
*
*/


#ifndef _SCONCONMLTASK_H_
#define _SCONCONMLTASK_H_

#include <e32base.h>
#include <f32file.h>

const TInt KSConTaskPartiallyCompleted( 206 );
const TInt KMaxPackageNameLength = 128;

enum TSConMethodName
    {
    ECancel = 0,
    EGetDataOwnerStatus,
    EGetDataSize,
    EGetStatus,
    EInstall,
    EListDataOwners,
    EListInstalledApps,
    EListPublicFiles,
    ERequestData,
    ESetBURMode,
    ESetInstParams,
    ESupplyData,
    EUninstall,
    EUpdateDeviceInfo,
    EReboot,
    EGetMetadata
    };

enum TSConInstallMode
    {
    ESilentInstall = 0,
    EUnsilentInstall,
    EUnknown
    };
    
enum TSConUserPerm
    {
    EPermReadOnly = 0,
    EPermNormal
    };
    
enum TSConDOType
    {
    EActiveDataOwner = 1,
    EPassiveDataOwner,
    EHybridDataOwner
    };
    
enum TSConHasFiles
    {
    ENoFiles = 0,
    EPublicFiles,
    ESystemFiles,
    EPublicSystemFiles
    };
    
enum TSConAppType
    {
    ESisApplication = 0,
    ESisAugmentation,
    EJavaApplication,
    EWidgetApplication
    };

enum TSConBurMode
    {
    ESConBurNormal = 1,     // 1 BUR mode - normal
    ESConBurBackupFull,     // 2 BUR mode – backup full
    ESConBurBackupPartial,  // 3 BUR mode – backup partial
    ESConBurRestoreFull,    // 4 BUR mode – restore full
    ESConBurRestorePartial  // 5 BUR mode – restore partial
    };

enum TSConIncType
    {
    ESConNoBackup = 0,          // 0   No backup
    ESConBackupBase = 100,      // 100 Backup – base
    ESConBackupIncrement = 200  // 200 Backup - increment
    };

enum TSConTransferDataType
    {
    ESConRegistrationData = 1,  // 1 RegistrationData
    ESConPassiveSnapshotData,   // 2 Passive snapshot data
    ESConPassiveBaseData,       // 3 Passive base data
    ESConPassiveIncrementalData,// 4 Passive incremental data 
    ESConActiveSnapshotData,    // 5 Active snapshot data
    ESConActiveBaseData,        // 6 Active base data
    ESConActiveIncrementalData, // 7 Active incremental data
    ESConSystemData,            // 8 System data
    ESConSystemSnapshotData     // 9 System snapshot data
    };

enum TSConDataOwnerStatus
    {
    ESConUnset = 0,         // 0 Unset
    ESConNotFound,          // 1 Not found
    ESConReady,             // 2 Ready
    ESConNotReady,          // 3 Not ready
    ESConFailed,            // 4 Failed
    ESConNotConnected,      // 5 Not connected
    ESConNotImplemented     // 6 Not implemented
    };




//============================================================
// class CSConReboot declaration
//============================================================  
NONSHARABLE_CLASS( CSConReboot ) : public CBase
    {
    public:
        CSConReboot();      
        ~CSConReboot();
        CSConReboot* CopyL();
                    
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress     
    };

//============================================================
// class CSConDataOwner declaration
//============================================================  
NONSHARABLE_CLASS( CSConDataOwner ) : public CBase
    {
    public:
        CSConDataOwner();
        ~CSConDataOwner();
        CSConDataOwner* CopyL();
    
    public:
        //dataowner type
        TSConDOType                     iType;
        //unique identifier
        TUid                            iUid;
        //drive list
        TDriveList                      iDriveList;
        //package name
        TBuf<KMaxPackageNameLength>     iPackageName;
        //requires reboot
        TBool                           iReqReboot;
        //has files 
        TSConHasFiles                   iHasFiles;
        //supports incremental backup
        TBool                           iSupportsInc;
        //supports selective backup
        TBool                           iSupportsSel;
        //delay to prepare data
        TBool                           iDelayToPrep;
        //size
        TUint                           iSize;
        //data owner status
        TSConDataOwnerStatus            iDataOwnStatus;
        //transfer data type
        TSConTransferDataType           iTransDataType;
        //Java hash
        HBufC*                          iJavaHash;
    };
        
//============================================================
// class CSConUpdateDeviceInfo declaration
//============================================================      
NONSHARABLE_CLASS( CSConUpdateDeviceInfo ) : public CBase
    {
    public:
        CSConUpdateDeviceInfo();
        ~CSConUpdateDeviceInfo();
        CSConUpdateDeviceInfo* CopyL();

        TBuf8<3>            iVersion;       //version number
        TBool               iInstallSupp;
        TBool               iUninstallSupp;
        TBool               iInstParamsSupp;
        TBool               iInstAppsSupp;
        TBool               iDataOwnersSupp;
        TBool               iSetBURModeSupp;
        TBool               iGetSizeSupp;
        TBool               iReqDataSupp;
        TBool               iSupplyDataSupp;
        TBool               iRebootSupp;
        TInt                iMaxObjectSize; //kBytes
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress     
    };
//============================================================
// class CSConInstApp declaration
//============================================================      
NONSHARABLE_CLASS( CSConInstApp ) : public CBase 
    {
    public:
        CSConInstApp() {};
        ~CSConInstApp();
        CSConInstApp* CopyL();
        
        TFileName       iName;
        TFileName       iParentName;
        TFileName       iVendor;
        TFileName       iVersion;
        TInt64          iSize;
        TSConAppType    iType;
        TUid            iUid;
        HBufC*          iWidgetBundleId; // Widget bundle identifier like "com.nokia.widget.HelloWorld"
    };
    
//============================================================
// class CSConListInstApps declaration
//============================================================      
NONSHARABLE_CLASS( CSConListInstApps ) : public CBase
    {
    public:
        CSConListInstApps();
        ~CSConListInstApps();
        CSConListInstApps* CopyL();
            
        TBool                   iAllApps;       //list all apps
        TDriveList              iDriveList;     //drive list

        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
                
        RPointerArray<CSConInstApp>     iApps; //installed apps
    };
    
//============================================================
// class CSConGetStatus declaration
//============================================================      
NONSHARABLE_CLASS( CSConGetStatus ) : public CBase
    {
    public:
        TBool               iAll;           //list all statuses
        TInt                iTaskId;        //task id
    };  
    
//============================================================
// class CSConFile declaration
//============================================================      
NONSHARABLE_CLASS( CSConFile ) : public CBase
    {
    public:
        CSConFile();
        ~CSConFile();
        CSConFile* CopyL();     
            
        TFileName                   iPath;      //file path + name
        TBuf<KMaxTimeFormatSpec>    iModified;  //modified date
        TUint                       iSize;      //file size
        TSConUserPerm               iUserPerm;  //user permission
    };  
    
//============================================================
// class CSConInstall declaration
//============================================================      
NONSHARABLE_CLASS( CSConInstall ) : public CBase
    {
    public:
        CSConInstall();
        ~CSConInstall();
            
        CSConInstall* CopyL();
            
        TFileName           iPath;          //path to file
        TSConInstallMode    iMode;          //install mode
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
        HBufC8*             iData;
    };  
    
//============================================================
// class CSConUninstall declaration
//============================================================      
NONSHARABLE_CLASS( CSConUninstall ) : public CBase
    {
    public:
        CSConUninstall();
        ~CSConUninstall();
            
        CSConUninstall* CopyL();
            
        TFileName           iName;          // application name
        TFileName           iVendor;        // application vendor
        TUid                iUid;           // application uid
        TSConAppType    	iType;			// application type
        TSConInstallMode    iMode;          // install mode
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
        HBufC8*             iData;
    };  
    
//============================================================
// class CSConListDataOwners declaration
//============================================================      
NONSHARABLE_CLASS( CSConListDataOwners ) : public CBase
    {
    public:
        CSConListDataOwners();
        ~CSConListDataOwners();         
        CSConListDataOwners* CopyL();           
        void DeleteDataOwners();
            
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
        
        RPointerArray<CSConDataOwner>   iDataOwners;    //data owners
    };
    
//============================================================
// class CSConSetBURMode declaration
//============================================================      
NONSHARABLE_CLASS( CSConSetBURMode ) : public CBase
    {
    public:
        TDriveList          iDriveList;     //drive list
        TSConBurMode        iPartialType;   //partial backup type
        TSConIncType        iIncType;       //incremental backup type
        
        CSConSetBURMode();
        ~CSConSetBURMode();     
        CSConSetBURMode* CopyL();
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };

//============================================================
// class CSConGetDataSize declaration
//============================================================  
NONSHARABLE_CLASS( CSConGetDataSize ) : public CBase
    {
    public:
        CSConGetDataSize();
        ~CSConGetDataSize();
        
        CSConGetDataSize* CopyL();          
        void DeleteDataOwners();
            
    public:
        RPointerArray<CSConDataOwner> iDataOwners;  //data owners
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };
    
//============================================================
// class CSConListPublicFiles declaration
//============================================================  
NONSHARABLE_CLASS( CSConListPublicFiles ) : public CBase
    {
    public:
        CSConListPublicFiles();
        ~CSConListPublicFiles();            
        CSConListPublicFiles* CopyL();
        
    public:
        RPointerArray<CSConFile>        iFiles;         //data owners
        RPointerArray<CSConDataOwner>   iDataOwners;    //data owners
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };
    
//============================================================
// class CSConRequestData declaration
//============================================================  
NONSHARABLE_CLASS( CSConRequestData ) : public CBase
    {
    public:
        CSConRequestData();         
        ~CSConRequestData();            
        CSConRequestData* CopyL();          
        void DeleteDataAndDataOwner();
            
        CSConDataOwner*     iDataOwner;     //data owner
        HBufC8*             iBackupData;    //backup data
        TBool               iMoreData;      //more data
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };
    
//============================================================
// class CSConGetDataOwnerStatus declaration
//============================================================  
NONSHARABLE_CLASS( CSConGetDataOwnerStatus ) : public CBase
    {
    public:
        CSConGetDataOwnerStatus();
        ~CSConGetDataOwnerStatus();         
        CSConGetDataOwnerStatus* CopyL();
        void DeleteDataOwners();
            
    public:
        RPointerArray<CSConDataOwner> iDataOwners;  //data owners
        
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };

//============================================================
// class CSConSupplyData declaration
//============================================================  
NONSHARABLE_CLASS( CSConSupplyData ) : public CBase
    {
    public:
        CSConSupplyData();
        ~CSConSupplyData();         
        CSConSupplyData* CopyL();
    public:
        CSConDataOwner*     iDataOwner;
        HBufC8*             iRestoreData;   //restore data
        TBool               iMoreData;      //more data     
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };

//============================================================
// class CSConGetMetadata declaration
//============================================================
NONSHARABLE_CLASS( CSConGetMetadata ) : public CBase
    {
    public:
        CSConGetMetadata();
        ~CSConGetMetadata();            
        CSConGetMetadata* CopyL();
    public:
        TFileName           iFilename;
        HBufC8*             iData;          //metadata
        TBool               iMoreData;      //more data     
        //used only with reply
        TBool               iComplete;      //task complete
        TInt                iProgress;      //task progress 
    };

//============================================================
// class CSConTask declaration
//============================================================  
NONSHARABLE_CLASS( CSConTask ): public CBase
    {
    public:
        /**
        * Two-phase constructor
        * @return CSConTask instance
        */
        static CSConTask* NewL( TSConMethodName aMethod );
        static CSConTask* NewLC( TSConMethodName aMethod );

        CSConTask();
        ~CSConTask();
        TSConMethodName GetServiceId() const;
        CSConTask* CopyL() const;
        TBool GetComplete();
        void SetCompleteValue( TBool aValue );
        TBool GetCompleteValue();
        void SetProgressValue( TInt aValue );
    private:
        
        void ConstructL( TSConMethodName aMethod );                
    public:
        //Task
        //service id
        TSConMethodName             iMethod;
        //task number
        TInt                        iTaskId;            
        
        //DeviceInfo -params
        CSConUpdateDeviceInfo*      iDevInfoParams;
        //ListInstalledTasks -params
        CSConListInstApps*          iListAppsParams;
        //GetStatus -params
        CSConGetStatus*             iGetStatusParams;
        //Install -params
        CSConInstall*               iInstallParams; 
        //Uninstall -params 
        CSConUninstall*             iUninstallParams;
        //SetBURMode -params
        CSConSetBURMode*            iBURModeParams;
        //GetDataSize -params
        CSConGetDataSize*           iGetDataSizeParams;
        //RequestData -params
        CSConRequestData*           iRequestDataParams; 
        //GetDataOwnerStatus -params
        CSConGetDataOwnerStatus*    iGetDataOwnerParams;
        //SupplyData -params
        CSConSupplyData*            iSupplyDataParams;
        //ListPublicFiles -params
        CSConListPublicFiles*       iPubFilesParams;
        //ListDataOwners -params
        CSConListDataOwners*        iListDataOwnersParams;
        //Reboot -params
        CSConReboot*                iRebootParams;
        //GetMetadata -params
        CSConGetMetadata*           iGetMetadataParams;
        
        TInt                        iCancelTaskId;
        TBool                       iCancelTaskAll;
    };
    
//============================================================
// class CSConTaskReply declaration
//============================================================
NONSHARABLE_CLASS( CSConTaskReply ) : public CBase
    {
    public:
        CSConTaskReply();
        CSConTaskReply( TSConMethodName aMethod );
        ~CSConTaskReply();
        void InitializeL( const CSConTask& aTask );   
        void InitializeL( TSConMethodName aMethod, 
            TInt aProgress, TBool aComplete );           
        CSConTaskReply* CopyAndFreeL();
        void CleanTaskData();
            
    public:
        //task id number
        TInt                        iTaskId;
        //method type
        TSConMethodName             iMethod;
        
        //DeviceInfo -params
        CSConUpdateDeviceInfo*      iDevInfoParams;
        //ListInstalledTasks -params
        CSConListInstApps*          iListAppsParams;
        //GetStatus -params
        CSConGetStatus*             iGetStatusParams;
        //Install -params
        CSConInstall*               iInstallParams;
        //Uninstall -params
        CSConUninstall*             iUninstallParams;
        //SetBURMode -params
        CSConSetBURMode*            iBURModeParams;
        //GetDataSize -params
        CSConGetDataSize*           iGetDataSizeParams;
        //RequestData -params
        CSConRequestData*           iRequestDataParams;
        //GetDataOwnerStatus -params
        CSConGetDataOwnerStatus*    iGetDataOwnerParams;
        //SupplyData -params
        CSConSupplyData*            iSupplyDataParams;
        //ListPublicFiles -params
        CSConListPublicFiles*       iPubFilesParams;
        //ListDataOwners -params
        CSConListDataOwners*        iListDataOwnersParams;
        //Reboot -params
        CSConReboot*                iRebootParams;
        //GetMetadata -params
        CSConGetMetadata*           iGetMetadataParams;
    };      
    
//============================================================
// class CSConStatusReply declaration
//============================================================
NONSHARABLE_CLASS( CSConStatusReply ): public CBase
    {
    public:
        CSConStatusReply();
        ~CSConStatusReply();
            
    public:
        TBool                           iNoTasks;       //no active tasks
        RPointerArray<CSConTaskReply>   iTasks;
    };

#endif

// End of file

