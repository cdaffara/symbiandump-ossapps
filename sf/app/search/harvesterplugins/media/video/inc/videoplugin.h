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
* Description:  Media Video harvester plugin header
*
*/


#ifndef CVIDEOPLUGIN_H
#define CVIDEOPLUGIN_H

#include <cindexingplugin.h>
#include <e32base.h>
#include "common.h"
#include "mediaobjecthandler.h"
#include "mmceventobserver.h"

//Forward declaration
class CSearchDocument;
class CCPixIndexer;
class CMdeHarvester;
class CMdeObjectQueueManager;
class CMdsMediaMonitor;
class CMdsSessionObjectUtils;
class CCPIXMediaVideoDoc;
class CCPixIndexerUtils;
class CMMCMonitorUtil;
class CCPIXMDEDbManager;
class CMDSEntity;

class CVideoPlugin : public CIndexingPlugin, public MdeMEdiaObserver,
                     public MMediaObjectHandler, public MMMCEventObserver
    {
public:
    // Constructor and Destructors.
    /*
    * NewL
    * @return instance of Media video plugin
    */
    static CVideoPlugin* NewL();    
    /*
     * NewLC
     * @return instance of Media plugin
     */
    static CVideoPlugin* NewLC();
    /*
    * Destructor
    */
    ~CVideoPlugin();

public:
    /* 
    * Prepares the plugin for StartHarvesterL
    */
    void StartPluginL();
    
    /*
    * @description Start the harvester.
    * @param aQualifiedBaseAppClass the base app class. In this case
    *        use MEDIA_QBASEAPPCLASS
    * @return void
    * Leaves in case of errors.
    */
    void StartHarvestingL(const TDesC & aQualifiedBaseAppClass);
    
    void PausePluginL();
    void ResumePluginL();
    
    /*
    * HandleMdeItemL callback from MMediaObjectHandler
    * @param TItemId aObjId object Id
    * @param TCPixActionType aActionType cpix action type
    */
    void HandleMdeItemL( TItemId aObjId, TCPixActionType aActionType);
#ifdef __PERFORMANCE_DATA
    void UpdateLogL();
#endif
    /*
    * HarvestingCompletedL callback from MdeEventHandler
    */
    void HarvestingCompletedL();
    
    /*
    * HandleMMCEventL callback from MMMCEventObserver
    * @param TDriveNumber aDrive drive number of MMC
    * @param TBool aMMCInsertRemoval ETrue if inserted else EFalse
    */
    void HandleMMCEventL(const TDriveNumber aDrive,const TBool aMMCInsertRemoval);
private:
    //private constructors.
    /*
    * Default constructor
    */
    CVideoPlugin();
    /*
     * Second phase constructor
     */
    void ConstructL();

private:
    // data
    CCPixIndexer*           iIndexer; //Indexer
    CMdeHarvester*          iMdeHarvester; //Mde harvester owned
    CMdeObjectQueueManager* iObjectJobQueueManager; //Objectqueue manager owned
    CMdsMediaMonitor*       iMdsMonitor;//MDS monitor owned
    CMdsSessionObjectUtils* iMdsUtils;//Mds utils owned
    CCPIXMediaVideoDoc*     iVideoDocument;//MDS video doc owned
    CCPixIndexerUtils*      iIndexerUtil;//Indexer util owned
    CMMCMonitorUtil*        iMMcMonitor;//MMC monitor owned
    CCPIXMDEDbManager*      iDBManager;
    CMDSEntity*             iMdsItem;

//for helping with testing.
#ifdef HARVESTERPLUGINTESTER_FRIEND
    friend class CHarvesterPluginTester;
#endif
        
#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
	TInt count;
#endif	
    };

#endif  // CVIDEOPLUGIN_H

