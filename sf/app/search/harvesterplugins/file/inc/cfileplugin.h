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


#ifndef CFILEPLUGIN_H
#define CFILEPLUGIN_H

#include <f32file.h> // TDriveNumber

#include <cindexingplugin.h>
#include <common.h>
#include <rsearchserversession.h>

class CFileHarvester;
class CFileMonitor;
class CMMCMonitor;
class CCPixIndexer;

// CONSTANTS
_LIT( KExcludePathSystem, ":\\System\\" );
_LIT( KExcludePathPrivate, ":\\Private\\" );
_LIT( KExcludePathMapsCities, ":\\cities\\" );

_LIT( KFastFindFSPluginFile, "cpixfileserverplugin" );
_LIT( KFastFindFSPluginName, "CPixFileServerPlugin" );

// maximum length that the fully qualified File Plugin base app class descriptor can be
// e.g. "@c:root file"
const TInt KFilePluginBaseAppClassMaxLen = 64;

class CFilePlugin : public CIndexingPlugin
{
public:    
    
    struct TRecord
        {
        TFileName iUri;
        TCPixActionType iActionType;
        TBool isFolder;
        };
    
	static CFilePlugin* NewL();
	static CFilePlugin* NewLC();
	virtual ~CFilePlugin();
	
	/**
	 * From CIndexingPlugin
	 */
	void StartPluginL();
	void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
	void PausePluginL();
	void ResumePluginL();
	/**
	 * CreateContentIndexItemL sends a file for indexing contents
	 * @aFilename full path and filename of the file to be indexed
	 * @aActionType action to be taken on the file
	 */
	void CreateContentIndexItemL(const TDesC& aFilename, TCPixActionType aActionType);
	/**
     * CreateFolderIndexItemL sends a folder/file for indexing
     * @aFilename full path and filename of the file to be indexed
     * @aActionType action to be taken on the file
     */
	void CreateFolderFileIndexItemL(const TDesC& aFilename, TCPixActionType aActionType, TBool aIsDir = ETrue);
	/**
     * Get the current file plugin harvesting state 
     * ETrue on pause, EFalse on resume  
     */
	TBool GetHarvesterState();
private:
	/**
     * CreateCpixDocumentL creates a document file 
     * @aFilePath full path of the file/folder to be indexed     
     */
	CSearchDocument* CreateCpixDocumentL(const TDesC& aFilePath, TBool aIsDir);	
	
	/**
     * IsFileTypeMedia checks for vaild extention type to be indexed 
     * @aExt file extention
     */
	TBool IsFileTypeMedia(const TDesC& aExt);
	
	/**
     * RemoveFileDatabaseL deletes the database file from the requested drive 
     * @aDrive drive number
     */
	void RemoveFileDatabaseL(TDriveNumber aDrive);

public:
	// 
	// New Functions
	//

	/*
	 * Notifies the indexing manager of completed harvesting, called by CMessageHarvester
	 */
	void HarvestingCompleted(TDriveNumber aDriveNumber, TInt aError);

    /**
     * Register notification paths to file observer
     * @aDriveNumber drive to add notification paths
     */
    void AddNotificationPathsL(const TDriveNumber aDriveNumber);

    /**
     * Unregister notification paths to file observer
     * @aDriveNumber drive to remove notification paths from
     */
    void RemoveNotificationPaths(const TDriveNumber aDriveNumber);

	/**
	 * MountL - Mount an IndexDB.
	 * @aMedia drive to mount.
	 * @aForceReharvest force a re-harvest of aMedia
	 */
	void MountL(TDriveNumber aMedia, TBool aForceReharvest = EFalse);

	/**
	 * UnMount - Dismount a IndexDB for drive aMedia.
	 * @aMedia drive to unmount.
	 * @aUndefineAsWell if ETrue then undefine the volume as well
	 * 
	 */
	void UnMount(TDriveNumber aMedia, TBool aUndefineAsWell,TBool aRemoveQueuePersist = EFalse);

	/**
	 * FormBaseAppClass - constructs a baseAppClass for the given drive.
	 * @aMedia drive to form the baseAppClass for.
	 * @aBaseAppClass return descriptor containing the baseAppClass
	 * returns KErrNone on success or a standard error code
	 */
	static TInt FormBaseAppClass(TDriveNumber aMedia,const TDesC& aGenericAppClass, TDes& aBaseAppClass);
	
	
	/**
	 * DatabasePathL - Forms IndexDb path.
	 * @param aMedia - The drive for which the path should be constructed.
	 * returns pointer to the IndexDb path.
	 */
	HBufC* DatabasePathLC(TDriveNumber aMedia,const TDesC& aPath);
	
	void MountAvailableDrivesInQueue();
	
	void AddToQueueL(const TDesC& aFilename, TCPixActionType aActionType, TBool aIsFolder);
	            
	void IndexQueuedItems();

protected:
	CFilePlugin();
	void ConstructL();

private:
    // CPix database 
    CCPixIndexer* iIndexer[EDriveZ+1]; // EDriveZ enum value is 25, so add 1.
    
    CCPixIndexer* iFolderIndexer[EDriveZ+1];
    
    RArray<TDriveNumber>  iMountDrives;
    
    RArray<TRecord> iJobQueue;

    // File system session
    RFs iFs;

    // Harvester
    CFileHarvester* iHarvester;

    // Monitor
    //TBool iIsMonitorInit;  unused variable
    CFileMonitor* iMonitor;

    // MMC monitor
    CMMCMonitor* iMmcMonitor;
    
    TBool iIndexState;
   
    //for unit testing.
    #ifdef HARVESTERPLUGINTESTER_FRIEND
        friend class CHarvesterPluginTester;
    #endif
#ifdef __PERFORMANCE_DATA
    TTime iStartTime[26];// for the all drives
    TTime iCompleteTime;
    void UpdatePerformaceDataL(TDriveNumber aMedia);
	TInt i;
#endif
};

#endif // CFILEPLUGIN_H
