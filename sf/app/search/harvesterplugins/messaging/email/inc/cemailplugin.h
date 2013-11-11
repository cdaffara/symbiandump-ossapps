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


#ifndef CEMAILPLUGIN_H
#define CEMAILPLUGIN_H

#include <e32base.h>
#include <cindexingplugin.h>
#include <common.h>
#include <memailitemobserver.h>

class CSearchDocument;
class CCPixIndexer;
class QEmailFetcher;

class CEmailPlugin : public CIndexingPlugin, public MEmailItemObserver
{
public:
	static CEmailPlugin* NewL();
	static CEmailPlugin* NewLC();
	virtual ~CEmailPlugin();
	
	/**
	 * From CIndexingPlugin
	 */
	 void StartPluginL();
	 void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
	 
	/**
	 * From MEmailItemObserver
	 */
	 void HandleDocumentL(const CSearchDocument* aSearchDocument, TCPixActionType aActionType);
	
	 /**
	  * From MEmailItemObserver
	  */ 
	 void HarvestingCompleted();
	 
     void PausePluginL();
     
     void ResumePluginL();
	 
	 /**
     * Gets the database observer class
     */
    CCPixIndexer* GetIndexer(); //SPB { return iIndexer; }
	
	 /**
     * MountL - Mount an IndexDB.
     * @aMedia drive to mount.
     * @aForceReharvesting to reharvest or not
     */
    void MountL(TDriveNumber aMedia, TBool aForceReharvesting=EFalse);

    /**
     * UnMount - Dismount a IndexDB for drive aMedia.
     * @aMedia drive to unmount.
     * @aUndefineAsWell if ETrue then undefine the volume as well
     * 
     */
    void UnMount(TDriveNumber aMedia, TBool aUndefineAsWell = ETrue);
    
    /**
     * FormBaseAppClass - constructs a baseAppClass for the given drive.
     * @aMedia drive to form the baseAppClass for.
     * @aBaseAppClass return descriptor containing the baseAppClass
     * returns KErrNone on success or a standard error code
     */
    static TInt FormBaseAppClass(TDriveNumber aMedia, TDes& aBaseAppClass);
    
    /**
     * DatabasePathL - Forms IndexDb path.
     * @param aMedia - The drive for which the path should be constructed.
     * returns pointer to the IndexDb path.
     */
    HBufC* DatabasePathLC(TDriveNumber aMedia);
    
public:
	/*
	 * Notifies the indexing manager of completed harvesting, called by CMessageHarvester
	 */
	void HarvestingCompleted(TInt aError);

protected:
	CEmailPlugin();
	void ConstructL();

private:
    // CPix indexer 
    CCPixIndexer* iIndexer[EDriveZ+1]; // EDriveZ enum value is 25, so add 1.
    //Currently used Drive by messaging application it is Default drive
    TInt     iCurrentDrive;
    // File system session
    RFs iFs;
    //Email fecther from Qt Module
    QEmailFetcher* iQEmailFetcher;
    //for unit testing.
    #ifdef HARVESTERPLUGINTESTER_FRIEND
        friend class CHarvesterPluginTester;
    #endif
#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
    void UpdatePerformaceDataL(TMsvSessionEvent);
#endif
};

#endif // CEMAILPLUGIN_H
