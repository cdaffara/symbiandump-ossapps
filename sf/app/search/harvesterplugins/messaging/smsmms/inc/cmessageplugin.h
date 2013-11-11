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


#ifndef CMESSAGEPLUGIN_H
#define CMESSAGEPLUGIN_H

#include <e32base.h>
#include <msvapi.h>
#include <cindexingplugin.h>
#include <common.h>

class CMessageMonitor;
class CMessageHarvester;
class CMessageDataHandler;
class CCPixIndexer;

enum TMsgType
{
	EMsgTypeInvalid,
	EMsgTypeSms,
	EMsgTypeMms,
	EMsgTypeDraft,
	EMsgTypeEmailPop3,
	EMsgTypeEmailSmtp,
	EMsgTypeEmailImap4
};

class CMessagePlugin : public CIndexingPlugin, public MMsvSessionObserver
{
public:
	static CMessagePlugin* NewL();
	static CMessagePlugin* NewLC();
	virtual ~CMessagePlugin();
	
	/**
	 * From CIndexingPlugin
	 */
	void StartPluginL();
	void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
    void PausePluginL();
    void ResumePluginL();
    
	/**
	* callback from MMsvSessionObserver
	*/
	virtual void HandleSessionEventL(TMsvSessionEvent aEvent, 
									 TAny* aArg1, 
									 TAny* aArg2, 
									 TAny* aArg3);

	/**
	* Called by Message monitor or harvester when new item is found.
	*
	* @param aMsgId Message item identifier
	* @param TCPixActionType aActionType action type
	* @param aFolderId identifier of the folder where message is stored 
	*/
    void MessageItemL( TMsvId aMsgId, TCPixActionType aActionType, TMsvId aFolderId );
    	
	/**
	* Indentify the message type from TMsvEntry
	*
	* @param TMsvEntry& aEntry a entry item to identify
	*/
    TMsgType CalculateMessageType (const TMsvEntry& aEntry );
    
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
    
    /**
     * IsMediaRemovableL - Check media is removable or not.
     * @param aDrive - The drive for which it is checked.
     * returns ETrue if it is removable else EFalse.
     */
    TBool IsMediaRemovableL(TDriveNumber& aDrive);
    
    TBool GetHarvesterState();
    
    void MountAvailableDrivesInQueue();    

public:
	/*
	 * Notifies the indexing manager of completed harvesting, called by CMessageHarvester
	 */
	void HarvestingCompleted(TInt aError);

protected:
	CMessagePlugin();
	void ConstructL();

private:
    // Session to Messge server session
    CMsvSession* iMsvSession;
    CMessageMonitor* iMessageMonitor;    
    CMessageHarvester* iMessageHarvester;
    CMessageDataHandler* iMessageDataHandler;

    // File system session
    RFs iFs;
    //Currently used Drive by messaging application
    TInt     iCurrentDrive;
    // CPix indexer 
    CCPixIndexer* iIndexer[EDriveZ+1]; // EDriveZ enum value is 25, so add 1.
    
    TBool iIndexState;
    
    RArray<TDriveNumber>  iMountDrives;    
    
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

#endif // CMESSAGEPLUGIN_H
