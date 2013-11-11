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
* Description:  
*
*/


#include <e32base.h>
#include <pathinfo.h> // PathInfo
#include <driveinfo.h> // TDriveInfo
#include "cfileplugin.h"
#include "harvesterserverlogger.h"
#include "cfileharvester.h"
#include "cfileplugin.h"
#include "cfilemonitor.h"
#include "cmmcmonitor.h"
#include <rsearchserversession.h>
#include <csearchdocument.h>
#include <common.h>

#include <ccpixindexer.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cfilepluginTraces.h"
#endif



// local declarations and functions
namespace {

_LIT(KCPixSearchServerPrivateDirectory, "\\Private\\2001f6f7\\");
_LIT(KIndexingDBPath,"indexing\\indexdb");
_LIT(KPathIndexDbPath, CPIX_INDEVICE_INDEXDB);
_LIT(KfileDBPath, "\\root\\file");
_LIT(KPathFolder, "\\root\\file\\folder");
_LIT(KPathFileContent, "\\root\\file\\content");
_LIT(KFileBaseAppClassContent, "root file content");
_LIT(KFileBaseAppClassFolder, "root file folder");
_LIT(KFilePluginAtSign, "@");
_LIT(KFilePluginColon, ":");
_LIT(KNameField, "Name");
_LIT(KExtensionField, "Extension");
_LIT(KIsFolderField, "IsFolder");
_LIT(KMimeTypeFile, FILE_MIMETYPE);
_LIT(KMimeTypeFolder , FOLDER_MIMETYPE);
_LIT(KMimeTypeField , CPIX_MIMETYPE_FIELD);

#define CONSTANT_TO_PTR16(ptr, c) TPtrC16 ptr; ptr.Set((const TUint16*)c,User::StringLength(c) );

/**
* MapFileToDrive - returns the TDriveNumber that the file is located on.
* @aFilename filename
* @aDrive returns the TDriveNumber
* returns KErrNone on success or a standard error code
*/
TInt MapFileToDrive(const TDesC& aFilename, TDriveNumber& aDrive)
    {
    TParse file;
    file.Set(aFilename, NULL, NULL);
    TPtrC drvChar = file.Drive().Left(1);
    TChar d(drvChar[0]);

    TInt drive;
    const TInt ret = RFs::CharToDrive(d, drive);
    if (!ret)
        {
        aDrive = TDriveNumber(drive);
        }
    return ret;
    }

/**
* MapBaseAppClassToDrive - gets the TDriveNumber associated with the aBaseAppClass.
* @aBaseAppClass e.g. "@c:root file"
* @aDrive returns the TDriveNumber for the aBaseAppClass
* returns KErrNone on success or a standard error code
*/
TInt MapBaseAppClassToDrive(const TDesC& aBaseAppClass, TDriveNumber& aDrive)
    {
    if (KErrNone != aBaseAppClass.Left(1).Compare(KFilePluginAtSign))
        {
        return KErrGeneral;
        }

    TPtrC drvChar = aBaseAppClass.Left(2).Right(1);
    TChar d(drvChar[0]);
    TInt drive;
    const TInt ret = RFs::CharToDrive(d, drive);
    if (!ret)
        {
        aDrive = TDriveNumber(drive);
        }

    return ret;
    }

} // anonymous namespace


TBool CFilePlugin::IsFileTypeMedia(const TDesC& aExt)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_ISFILETYPEMEDIA_ENTRY );
    
    const TText* KMediaExt[] = {_S("txt"),_S("pdf"), _S("jpg"),_S("mp3"), _S("jpeg"), _S("wma"), _S("3gp"), _S("mpg4"),
                                _S("avi"), _S("jpf"), _S("mbm"), _S("png"), _S("gif"), _S("bmp"),  _S("mp4"), _S("mpeg4"),
                                _S("m4v"), _S("m4a"), _S("3gpp"), _S("3g2"), _S("aac"), _S("amr"), _S("wmv"), _S("divx"),
                                _S("awb"),_S("mid"), _S("midi"), _S("spmid"), _S("rng"), _S("mxmf"), _S("wav"),
                                _S("au"), _S("nrt"), _S("mka"),_S("jp2"), _S("j2k"), _S("jpx"),
                                _S("rm"), _S("rmvb"),_S("ota"), _S("wbmp"), _S("wmf"),_S("otb"),
                                _S("rv"),  _S("mkv"), _S("asf"),_S("ra"),_S("tif"), _S("tiff")};
    
    const TInt count = sizeof( KMediaExt ) / sizeof( TText* );
    
    TBool isValid = false;
    
    for (TInt i = 0; i < count; i++)
        {
        CONSTANT_TO_PTR16(type, KMediaExt[i]);
        if(!aExt.Compare(type))
            {
            isValid = true;
            break;
            }
        }    
    OstTraceFunctionExit0( CFILEPLUGIN_ISFILETYPEMEDIA_EXIT );
    return isValid;
    }

CFilePlugin* CFilePlugin::NewL()
    {
    CFilePlugin* instance = CFilePlugin::NewLC();
    CleanupStack::Pop(instance);
    return instance;
    }

CFilePlugin* CFilePlugin::NewLC()
    {
    CFilePlugin* instance = new (ELeave)CFilePlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
    }

CFilePlugin::CFilePlugin()
    {
    for (TInt i=EDriveA; i<=EDriveZ; i++)
        {
        iIndexer[i] = NULL; //Initialize to NULL
        iFolderIndexer[i] = NULL;
        }
    }

CFilePlugin::~CFilePlugin()
	{
	CPIXLOGSTRING("CFilePlugin::~CFilePlugin()");

	// remove notification paths before destroying iMonitor
	for (TInt i=EDriveA; i<=EDriveZ; i++)
		{
		UnMount(TDriveNumber(i), EFalse);
		}
	delete iHarvester;
	delete iMonitor;
	delete iMmcMonitor;	
	iMountDrives.Close();
	iJobQueue.Reset();
	iJobQueue.Close();
	TInt err = iFs.DismountPlugin(KFastFindFSPluginName);
	CPIXLOGSTRING2("CFilePlugin::~CFilePlugin(), iFs.DismountPlugin: %i", err);
	err = iFs.RemovePlugin(KFastFindFSPluginName);
	CPIXLOGSTRING2("CFilePlugin::~CFilePlugin(), iFs.RemovePlugin: %i", err);
	iFs.Close();
	}

void CFilePlugin::ConstructL()
	{
    iIndexState = ETrue;
    User::LeaveIfError( iFs.Connect() );
    TInt err = iFs.AddPlugin(KFastFindFSPluginFile);
    OstTrace1( TRACE_NORMAL, CFILEPLUGIN_CONSTRUCTL, "CFilePlugin::ConstructL;iFs.AddPlugin=%d", err );
    CPIXLOGSTRING2("CFilePlugin::ConstructL, iFs.AddPlugin: %i", err);
    if ( err != KErrAlreadyExists )
    	{
    	err = iFs.MountPlugin(KFastFindFSPluginName);
    	OstTrace1( TRACE_NORMAL, DUP1_CFILEPLUGIN_CONSTRUCTL, "CFilePlugin::ConstructL;iFs.MountPlugin=%d", err );
    	CPIXLOGSTRING2("CFilePlugin::ConstructL, iFs.MountPlugin: %i", err);
    	}
    // check if already up, unload and reload
    else if ( err == KErrAlreadyExists )
    	{
    	// dismount
    	TInt err = iFs.DismountPlugin(KFastFindFSPluginName);
	    OstTrace1( TRACE_NORMAL, DUP2_CFILEPLUGIN_CONSTRUCTL, "CFilePlugin::ConstructL;iFs.DismountPlugin=%d", err );
	    CPIXLOGSTRING2("CFilePlugin::ConstructL(), iFs.DismountPlugin: %i", err);
		err = iFs.RemovePlugin(KFastFindFSPluginName);
		OstTrace1( TRACE_NORMAL, DUP3_CFILEPLUGIN_CONSTRUCTL, "CFilePlugin::ConstructL;iFs.RemovePlugin=%d", err );
		CPIXLOGSTRING2("CFilePlugin::ConstructL(), iFs.RemovePlugin: %i", err);
		// if no error reload
		if ( err == KErrNone )
			{
			err = iFs.AddPlugin(KFastFindFSPluginFile);
			OstTrace1( TRACE_NORMAL, DUP4_CFILEPLUGIN_CONSTRUCTL, "CFilePlugin::ConstructL;iFs.AddPlugin=%d", err );
			CPIXLOGSTRING2("CFilePlugin::ConstructL, iFs.AddPlugin: %i", err);
			err = iFs.MountPlugin(KFastFindFSPluginName);
			OstTrace1( TRACE_NORMAL, DUP5_CFILEPLUGIN_CONSTRUCTL, "CFilePlugin::ConstructL;iFs.MountPlugin=%d", err );
			CPIXLOGSTRING2("CFilePlugin::ConstructL, iFs.MountPlugin: %i", err);
			}
    	}

    iHarvester = CFileHarvester::NewL(*this, iFs);
    iMonitor = CFileMonitor::NewL(*this, &iFs);
    iMmcMonitor = CMMCMonitor::NewL(*this, &iFs);
    }

void CFilePlugin::StartPluginL()
	{
    TInt error = KErrNone;    
    error = iMonitor->Initialize();
    OstTrace1( TRACE_NORMAL, CFILEPLUGIN_STARTPLUGINL, "CFilePlugin::StartPluginL;Monitor Error=%d", error );
    CPIXLOGSTRING2("CFilePlugin::StartMonitoring, error: %i", error );        

    // Start the monitoring
    if (error == KErrNone )
        {
        iMonitor->StartMonitoring();
        OstTrace0( TRACE_NORMAL, DUP1_CFILEPLUGIN_STARTPLUGINL, "CFilePlugin::StartMonitoring - iFileMonitor->StartMonitoring " );
        CPIXLOGSTRING("CFilePlugin::StartMonitoring - iFileMonitor->StartMonitoring ");
        }

    iMmcMonitor->StartMonitoring();

    OstTrace0( TRACE_NORMAL, DUP2_CFILEPLUGIN_STARTPLUGINL, "END CFilePlugin::StartMonitoring" );
    CPIXLOGSTRING("END CFilePlugin::StartMonitoring");

    // Add harvesters for each non removable drive
    for ( TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ )
        {
        if ( iFs.IsValidDrive( driveNumber ) )
            {
    		TUint drvStatus( 0 );
    		TInt err = DriveInfo::GetDriveStatus(iFs, TDriveNumber(driveNumber), drvStatus);
    		if ( err != KErrNone )
    			{
    			continue;
    			}

    		// Harvest drive that are internal, non-removable, user visible, present
    		if ( ( drvStatus & DriveInfo::EDriveInternal ) &&
    			!( drvStatus & DriveInfo::EDriveRemovable ) && // NOT removable
    			 ( drvStatus & DriveInfo::EDriveUserVisible ) &&
    			 ( drvStatus & DriveInfo::EDrivePresent ) )
    			{
    		    MountL(TDriveNumber(driveNumber));
    			}
            }
        }
}

void CFilePlugin::MountL(TDriveNumber aMedia, TBool aForceReharvest)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_MOUNTL_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::MountL");
    // Check if already exists
    if (iIndexer[aMedia] && iFolderIndexer[aMedia])
        return;
    //remove the database incase of memory card insertion before harvesting
    if (aForceReharvest)
        {        
          RemoveFileDatabaseL(aMedia);
        }
        
    // Add Notifications paths prior to opening IndexDB.
    AddNotificationPathsL(aMedia);

    // Form the baseappclass for folder
    TBuf<KFilePluginBaseAppClassMaxLen> baseFolderAppClass;
    FormBaseAppClass(aMedia,KFileBaseAppClassFolder, baseFolderAppClass);
    //Form the baseappclass for content
    TBuf<KFilePluginBaseAppClassMaxLen> baseContentAppClass;
    FormBaseAppClass(aMedia,KFileBaseAppClassContent, baseContentAppClass);
    
    // Define volume for folder and content index database
    HBufC* path = DatabasePathLC(aMedia,KPathFolder);
    User::LeaveIfError(iSearchSession.DefineVolume(baseFolderAppClass, *path));
    CleanupStack::PopAndDestroy(path);
    
    HBufC* contentpath = DatabasePathLC(aMedia,KPathFileContent);
    User::LeaveIfError(iSearchSession.DefineVolume(baseContentAppClass, *contentpath));
    CleanupStack::PopAndDestroy(contentpath);
    
    // construct and open the database
    iIndexer[aMedia] = CCPixIndexer::NewL(iSearchSession);
    iIndexer[aMedia]->OpenDatabaseL(baseContentAppClass);
    
    iFolderIndexer[aMedia]= CCPixIndexer::NewL(iSearchSession);
    iFolderIndexer[aMedia]->OpenDatabaseL(baseFolderAppClass);

    // Add to harvesting queue
    iObserver->AddHarvestingQueue(this, baseContentAppClass, aForceReharvest);
    
    CPIXLOGSTRING("END CFilePlugin::MountL");
    OstTraceFunctionExit0( CFILEPLUGIN_MOUNTL_EXIT );
    }

void CFilePlugin::UnMount(TDriveNumber aMedia, TBool aUndefineAsWell,TBool aRemoveQueuePersist)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_UNMOUNT_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::UnMount ");
    // Check if already exists
    if (!iIndexer[aMedia] && !iFolderIndexer[aMedia])
        {
        OstTraceFunctionExit0( CFILEPLUGIN_UNMOUNT_EXIT );
        return;
        }

    // Form the baseappclass for folder
    TBuf<KFilePluginBaseAppClassMaxLen> baseFolderAppClass;
    FormBaseAppClass(aMedia,KFileBaseAppClassFolder, baseFolderAppClass);
    //Form the baseappclass for content
    TBuf<KFilePluginBaseAppClassMaxLen> baseContentAppClass;
    FormBaseAppClass(aMedia,KFileBaseAppClassContent, baseContentAppClass);

    // Remove from harvesting queue aRemovePersist will remove from config file as well
    iObserver->RemoveHarvestingQueue(this, baseContentAppClass,aRemoveQueuePersist);
    
    // Delete the index object
    if (iIndexer[aMedia])
        {
        delete iIndexer[aMedia];
        iIndexer[aMedia] = NULL;
        }
    
    if (iFolderIndexer[aMedia])
        {
        delete iFolderIndexer[aMedia];
        iFolderIndexer[aMedia] = NULL;
        }
    RemoveNotificationPaths(aMedia);

    if (aUndefineAsWell)
        {
        iSearchSession.UnDefineVolume(baseFolderAppClass);
        iSearchSession.UnDefineVolume(baseContentAppClass);
        }
    CPIXLOGSTRING("END CFilePlugin::UnMount ");
    OstTraceFunctionExit0( DUP1_CFILEPLUGIN_UNMOUNT_EXIT );
    }

void CFilePlugin::StartHarvestingL(const TDesC& aQualifiedBaseAppClass)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_STARTHARVESTINGL_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::StartHarvestingL ");
    // Map base app class to a drive number
    TDriveNumber drive(EDriveA); //Initialize to get rid of compiler warning.
    if (KErrNone != MapBaseAppClassToDrive(aQualifiedBaseAppClass, drive))
        {
        User::Leave(KErrGeneral);
        }

    // Leave if no indexer for this drive
    if (!iIndexer[drive] && !iFolderIndexer[drive])
        {
        User::Leave(KErrGeneral);
        }

#ifdef __PERFORMANCE_DATA
    	iStartTime[drive].UniversalTime();
#endif
    // Reset the database
    iIndexer[drive]->ResetL();
    iFolderIndexer[drive]->ResetL();
    
    // Start the actual harvest
    if( iIndexState )
        {    
        iHarvester->StartL(drive);
        }
    else
        {
        iMountDrives.Append(TDriveNumber(drive));
        }
    CPIXLOGSTRING("END CFilePlugin::StartHarvestingL ");
    OstTraceFunctionExit0( CFILEPLUGIN_STARTHARVESTINGL_EXIT );
    }

void CFilePlugin::CreateContentIndexItemL(const TDesC& aFilename, TCPixActionType aActionType)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_CREATECONTENTINDEXITEML_ENTRY );
    TFileName lowerCaseFilename(aFilename);
    lowerCaseFilename.LowerCase();

    OstTraceExt2( TRACE_NORMAL, DUP1_CFILEPLUGIN_CREATECONTENTINDEXITEML, "CFilePlugin::CreateContentIndexItemL;lowerCaseFilename=%S;aActionType=%d", lowerCaseFilename, aActionType );
    CPIXLOGSTRING3("CFilePlugin::CreateFileIndexItemL lowerCaseFilename = %S aActionType = %d ", 
					&lowerCaseFilename, aActionType);

	TDriveNumber drive(EDriveA);
	User::LeaveIfError( MapFileToDrive( lowerCaseFilename, drive ) );

	CCPixIndexer* indexer = iIndexer[drive];
    
	if (!indexer)
		{
		OstTrace0( TRACE_NORMAL, DUP2_CFILEPLUGIN_CREATECONTENTINDEXITEML, "CFilePlugin::CreateFileIndexItemL(): Could not map file to drive." );
		CPIXLOGSTRING("CFilePlugin::CreateFileIndexItemL(): Could not map file to drive.");
		OstTraceFunctionExit0( CFILEPLUGIN_CREATECONTENTINDEXITEML_EXIT );
		return;
		}
	OstTrace0( TRACE_NORMAL, DUP5_CFILEPLUGIN_CREATECONTENTINDEXITEML, "CFilePlugin::Indexing File content" );
	
	// Index an empty item if removal action
	if (aActionType == ECPixRemoveAction)
		{
#ifdef _DEBUG
		TRAPD(err, indexer->DeleteL(lowerCaseFilename));
		OstTrace1( TRACE_NORMAL, CFILEPLUGIN_CREATECONTENTINDEXITEML, "CFilePlugin::CreateContentIndexItemL;DeleteL returned=%d", err );
		CPIXLOGSTRING2("CFilePlugin::CreateFileIndexItemL(): DeleteL returned %d.", err);
#else   
		TRAP_IGNORE( indexer->DeleteL(lowerCaseFilename) );
#endif
		}
	else
		{
		// creating CSearchDocument object with unique ID for this application
		CSearchDocument* index_item = CSearchDocument::NewLC(lowerCaseFilename, KNullDesC, KNullDesC, CSearchDocument::EFileParser);

		// Send for indexing
		if (aActionType == ECPixAddAction)
			{
#ifdef _DEBUG
			TRAPD(err, indexer->AddL(*index_item));
	        OstTrace1( TRACE_NORMAL, DUP3_CFILEPLUGIN_CREATECONTENTINDEXITEML, "CFilePlugin::CreateContentIndexItemL;AddL returned=%d", err );
	        CPIXLOGSTRING2("CFilePlugin::CreateFileIndexItemL(): AddL returned %d.", err);
#else
			TRAP_IGNORE( indexer->AddL(*index_item) );
#endif
			}
		else if (aActionType == ECPixUpdateAction)
			{
#ifdef _DEBUG		
			TRAPD(err, indexer->UpdateL(*index_item));
	        OstTrace1( TRACE_NORMAL, DUP4_CFILEPLUGIN_CREATECONTENTINDEXITEML, "CFilePlugin::CreateContentIndexItemL;UpdateL returned=%d", err );
	        CPIXLOGSTRING2("CFilePlugin::CreateFileIndexItemL(): UpdateL returned %d.", err);
#else
			TRAP_IGNORE( indexer->UpdateL(*index_item) );
#endif
			}
		CleanupStack::PopAndDestroy(index_item);
		}    
    OstTraceFunctionExit0( DUP1_CFILEPLUGIN_CREATECONTENTINDEXITEML_EXIT );
    }

void CFilePlugin::CreateFolderFileIndexItemL(const TDesC& aFilename, TCPixActionType aActionType, TBool aIsDir)
    {    
    OstTraceFunctionEntry0( CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML_ENTRY );
    CSearchDocument* index_item = NULL;
    TBool isMediaFile = false;
    TFileName lowerCaseFilename(aFilename);    
    lowerCaseFilename.LowerCase();        
    
    // Adding the file entry to folder index,check for file or folder entry,
    //if file, extract the extension
    if (!aIsDir)
        {
        TFileName extension; 
        TInt pos = lowerCaseFilename.LocateReverse('.');
        if (pos > 0)
            {
            extension.Copy(lowerCaseFilename.Mid(pos+1));
            isMediaFile = IsFileTypeMedia(extension);
            }
        }
    //Check for extension type, if media items do not index them
    if(!isMediaFile)
        {
        OstTrace0( TRACE_NORMAL, DUP5_CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML, "CFilePlugin::Indexing File Folder" );
        
        OstTraceExt2( TRACE_NORMAL, CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML, "CFilePlugin::CreateFolderFileIndexItemL;lowerCaseFilename=%S;aActionType=%d", lowerCaseFilename, aActionType );
        CPIXLOGSTRING3("CFilePlugin::CreateFolderFileIndexItemL lowerCaseFilename = %S aActionType = %d ", 
                                                    &lowerCaseFilename, aActionType);

        TDriveNumber drive(EDriveA);
        User::LeaveIfError( MapFileToDrive( lowerCaseFilename, drive ) );    

        CCPixIndexer* indexer = iFolderIndexer[drive];
        if (!indexer)
            {
            OstTrace0( TRACE_NORMAL, DUP1_CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML, "CFilePlugin::CreateFolderFileIndexItemL(): Could not map file to drive." );
            CPIXLOGSTRING("CFilePlugin::CreateFolderFileIndexItemL(): Could not map file to drive.");
            OstTraceFunctionExit0( CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML_EXIT );
            return;
            }
        
        if (aActionType == ECPixRemoveAction)
            {
    #ifdef _DEBUG
            TRAPD(err, indexer->DeleteL(lowerCaseFilename));
            OstTrace1( TRACE_NORMAL, DUP2_CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML, "CFilePlugin::CreateFolderFileIndexItemL;DeleteL returned=%d", err );
            CPIXLOGSTRING2("CFilePlugin::CreateFolderFileIndexItemL(): DeleteL returned %d.", err);
    #else
            TRAP_IGNORE( indexer->DeleteL(lowerCaseFilename) );
    #endif
            }
        else
            {
            index_item = CreateCpixDocumentL(lowerCaseFilename, aIsDir);
            
            if( aActionType == ECPixAddAction )
                {
    #ifdef _DEBUG
                TRAPD(err, indexer->AddL(*index_item));
                OstTrace1( TRACE_NORMAL, DUP3_CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML, "CFilePlugin::CreateFolderFileIndexItemL;AddL returned=%d", err );
                CPIXLOGSTRING2("CFilePlugin::CreateFolderFileIndexItemL(): AddL returned %d.", err);
    #else
                TRAP_IGNORE( indexer->AddL(*index_item) );
    #endif
                }
            else if( aActionType == ECPixUpdateAction )
                {
    #ifdef _DEBUG
                TRAPD(err, indexer->UpdateL(*index_item));
                OstTrace1( TRACE_NORMAL, DUP4_CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML, "CFilePlugin::CreateFolderFileIndexItemL;UpdateL returned=%d", err );
                CPIXLOGSTRING2("CFilePlugin::CreateFolderFileIndexItemL(): UpdateL returned %d.", err);
    #else
                TRAP_IGNORE( indexer->UpdateL(*index_item) );
    #endif
                }
            delete index_item;
            }
        }    
    OstTraceFunctionExit0( DUP1_CFILEPLUGIN_CREATEFOLDERFILEINDEXITEML_EXIT );
    }

void CFilePlugin::HarvestingCompleted(TDriveNumber aDriveNumber, TInt aError)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_HARVESTINGCOMPLETED_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::HarvestingCompleted ");

    if (iIndexer[aDriveNumber])
        {
        Flush(*iIndexer[aDriveNumber]);
        }
    if (iFolderIndexer[aDriveNumber])
        {
        Flush(*iFolderIndexer[aDriveNumber]);
        }
    
    //Form baseapp class for folder and content database
    TBuf<KFilePluginBaseAppClassMaxLen> baseContentAppClass;
    FormBaseAppClass(TDriveNumber(aDriveNumber),KFileBaseAppClassContent, baseContentAppClass);    
    
    iObserver->HarvestingCompleted(this, baseContentAppClass, aError);    
    
#ifdef __PERFORMANCE_DATA
    TRAP_IGNORE( UpdatePerformaceDataL(aDriveNumber) );
#endif


    CPIXLOGSTRING("END CFilePlugin::HarvestingCompleted ");
    OstTraceFunctionExit0( CFILEPLUGIN_HARVESTINGCOMPLETED_EXIT );
    }

void CFilePlugin::AddNotificationPathsL(const TDriveNumber aDriveNumber)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_ADDNOTIFICATIONPATHSL_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::AddNotificationPathsL ");
    iMonitor->AddNotificationPathsL(aDriveNumber);
    iHarvester->AddIgnorePathsL(aDriveNumber);
    CPIXLOGSTRING("END CFilePlugin::AddNotificationPathsL ");
    OstTraceFunctionExit0( CFILEPLUGIN_ADDNOTIFICATIONPATHSL_EXIT );
    }

void CFilePlugin::RemoveNotificationPaths(const TDriveNumber aDriveNumber)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_REMOVENOTIFICATIONPATHS_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::RemoveNotificationPaths");
    iMonitor->RemoveNotificationPaths(aDriveNumber);
    iHarvester->RemoveIgnorePaths(aDriveNumber);
    CPIXLOGSTRING("END CFilePlugin::RemoveNotificationPaths");
    OstTraceFunctionExit0( CFILEPLUGIN_REMOVENOTIFICATIONPATHS_EXIT );
    }

TInt CFilePlugin::FormBaseAppClass(TDriveNumber aMedia, const TDesC& aGenericAppClass, TDes& aBaseAppClass)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_FORMBASEAPPCLASS_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::FormBaseAppClass");
    TChar chr;
    const TInt ret = RFs::DriveToChar(aMedia, chr);
    if (KErrNone == ret)
        {
        aBaseAppClass.Copy(KFilePluginAtSign);
        aBaseAppClass.Append(chr);
        aBaseAppClass.LowerCase();
        aBaseAppClass.Append(KFilePluginColon);
        aBaseAppClass.Append(aGenericAppClass);
        }

    CPIXLOGSTRING("END CFilePlugin::FormBaseAppClass");
    OstTraceFunctionExit0( CFILEPLUGIN_FORMBASEAPPCLASS_EXIT );
    return ret;
    }

HBufC* CFilePlugin::DatabasePathLC(TDriveNumber aMedia,const TDesC& aPath)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_DATABASEPATHLC_ENTRY );
    CPIXLOGSTRING("ENTER CFilePlugin::DatabasePathLC");
    // Allocate extra space for root path e.g. "C:\\Private\\2001f6f7\\"
    const TInt KRootPathMaxLength = 30;
    HBufC* indexDbPath = HBufC::NewLC(KRootPathMaxLength + KPathIndexDbPath().Length() + aPath.Length());
    TPtr indexDbPathPtr = indexDbPath->Des();

    // Data caging implementation
    iFs.CreatePrivatePath(aMedia);

    TChar chr;
    RFs::DriveToChar(aMedia, chr);
    indexDbPathPtr.Append(chr);
    indexDbPathPtr.Append(KFilePluginColon);

    TFileName pathWithoutDrive;
    iFs.PrivatePath(pathWithoutDrive);
    indexDbPathPtr.Append(KCPixSearchServerPrivateDirectory);

    indexDbPathPtr.Append(KPathIndexDbPath);
    indexDbPathPtr.Append(aPath);

    CPIXLOGSTRING("END CFilePlugin::DatabasePathLC");
    OstTraceFunctionExit0( CFILEPLUGIN_DATABASEPATHLC_EXIT );
    return indexDbPath;
    }

CSearchDocument* CFilePlugin::CreateCpixDocumentL(const TDesC& aFilePath, TBool aIsDir)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_CREATECPIXDOCUMENTL_ENTRY );
    
    CSearchDocument* index_item = CSearchDocument::NewLC(aFilePath, KFileBaseAppClassFolder);
        
    TParse file;
    file.Set(aFilePath, NULL, NULL);
    
    TFileName fileFoldername; 
    TInt pos = aFilePath.LocateReverse('\\');
    if (pos > 0)
        {
        fileFoldername.Copy(aFilePath.Mid(pos+1));
        index_item->AddFieldL(KNameField, fileFoldername, 
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText );
        }
    
    //check for the extension, for folders names store the extension field as NULL
    //adding Mimetype and Extension field
    if(!aIsDir)
       {
        if( file.ExtPresent())
            {
            TPtrC extension = file.Ext();
            index_item->AddFieldL(KExtensionField, extension, CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
            }
        index_item->AddFieldL(KMimeTypeField, KMimeTypeFile, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
       }
    else
        {
        index_item->AddFieldL(KExtensionField, KNullDesC);
        index_item->AddFieldL(KMimeTypeField, KMimeTypeFolder, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
        }
    TBuf<2> Isfolder;
    Isfolder.AppendNum(aIsDir);
    index_item->AddFieldL(KIsFolderField, Isfolder, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
    //Only content to be added to exceprt field. See appclass-hierarchy.txt
    //Add excerpt field
    index_item->AddExcerptL(aFilePath);
    
    CleanupStack::Pop(index_item);
    OstTraceFunctionExit0( CFILEPLUGIN_CREATECPIXDOCUMENTL_EXIT );
    return index_item;
    }

void CFilePlugin::RemoveFileDatabaseL(TDriveNumber aDrive)
    {
    RFs aFs;
    User::LeaveIfError( aFs.Connect() );
    TChar drive;
    TInt err = aFs.DriveToChar((TDriveNumber)aDrive,drive);
    if ( err == KErrNone )
        {
        TBuf<KMaxFileName> folderpath;
        folderpath.Append(drive);
        folderpath.Append(KFilePluginColon);
        folderpath.Append(KCPixSearchServerPrivateDirectory);
        folderpath.Append(KIndexingDBPath);
        folderpath.Append(KfileDBPath);
        CFileMan* FileMan = CFileMan::NewL(aFs);
        if ( FileMan )
            FileMan->Delete( folderpath );
        delete FileMan;
        }
    aFs.Close();
    }

void CFilePlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_PAUSEPLUGINL_ENTRY );
    iIndexState = EFalse;
    //iHarvester->SetHarvesterState(iIndexState);
    OstTraceFunctionExit0( CFILEPLUGIN_PAUSEPLUGINL_EXIT );
    }

void CFilePlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_RESUMEPLUGINL_ENTRY );
    iIndexState = ETrue;
    IndexQueuedItems();
    iHarvester->ResumeRequest();
    MountAvailableDrivesInQueue();
    //iHarvester->SetHarvesterState(iIndexState);
    OstTraceFunctionExit0( CFILEPLUGIN_RESUMEPLUGINL_EXIT );
    }

void CFilePlugin::MountAvailableDrivesInQueue()
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_MOUNTAVAILABLEDRIVESINQUEUE_ENTRY );
    for(TInt i=0;i<iMountDrives.Count();i++)
        {
        TDriveNumber drive = iMountDrives[i];
        iMountDrives.Remove(i);
        iHarvester->StartL(drive);        
        }
    OstTraceFunctionExit0( CFILEPLUGIN_MOUNTAVAILABLEDRIVESINQUEUE_EXIT );
    }

TBool CFilePlugin::GetHarvesterState()
    {
    return iIndexState;
    }

void CFilePlugin::AddToQueueL(const TDesC& aFilename, TCPixActionType aActionType, TBool aIsFolder)
    {
    OstTraceFunctionEntry0( CFILEPLUGIN_ADDTOQUEUEL_ENTRY );
    TRecord entry;
    entry.isFolder = aIsFolder;
    entry.iUri.Copy(aFilename);
    entry.iActionType = aActionType;
    iJobQueue.AppendL(entry);
    OstTraceFunctionExit0( CFILEPLUGIN_ADDTOQUEUEL_EXIT );
    }

void CFilePlugin::IndexQueuedItems()
    {   
    OstTraceFunctionEntry0( CFILEPLUGIN_INDEXQUEUEDITEMS_ENTRY );
    while (iJobQueue.Count()>0 && GetHarvesterState())
        {
        TRecord entry = iJobQueue[0];        
        //Let the indexer handle this object TRAP it as it can leave
        if(entry.isFolder)            
            CreateFolderFileIndexItemL( entry.iUri, entry.iActionType );
        else
            {
            CreateContentIndexItemL( entry.iUri, entry.iActionType );
            CreateFolderFileIndexItemL( entry.iUri, entry.iActionType, entry.isFolder );
            }        
        iJobQueue.Remove(0);
        }
    OstTraceFunctionExit0( CFILEPLUGIN_INDEXQUEUEDITEMS_EXIT );
    }

#ifdef __PERFORMANCE_DATA
void CFilePlugin::UpdatePerformaceDataL(TDriveNumber aDriveNumber)
    {
    TTime now;
  	TChar aChar;
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime[aDriveNumber]);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
	RFs::DriveToChar((TInt)aDriveNumber, aChar);
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\FilePerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\FilePerf.txt"), EFileWrite));
    
    HBufC8 *heap = HBufC8::NewL(100);
    TPtr8 ptr = heap->Des();
    now.HomeTime();
    TBuf<50> timeString;             
                
    _LIT(KOwnTimeFormat,"%:0%H%:1%T%:2%S");
    now.FormatL(timeString,KOwnTimeFormat);
    ptr.AppendNum(now.DateTime().Day());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Month());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Year());
    ptr.Append(_L(":"));
    ptr.Append(timeString);
    ptr.Append(_L("Drive "));
    ptr.Append(aChar);
    ptr.Append( _L(" Ani: Time took for Harvesting File is : "));
    ptr.AppendNum(timeDiff.Int64()/1000) ;
    ptr.Append(_L(" MilliSeonds \n"));
    TInt myInt = 0;
    perfFile.Seek(ESeekEnd,myInt);
    perfFile.Write (ptr);
    perfFile.Close ();
    fileSession.Close ();
    delete heap;
    }
#endif

// End of file

