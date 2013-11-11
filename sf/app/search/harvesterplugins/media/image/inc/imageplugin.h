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
* Description:  Media Audio harvester plugin header
*
*/


#ifndef CIMAGEPLUGIN_H
#define CIMAGEPLUGIN_H

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
class CCPIXMediaImageDoc;
class CCPixIndexerUtils;
class CMMCMonitorUtil;
class CCPIXMDEDbManager;
class CMDSEntity;

// to get perf data
//#ifndef __PERFORMANCE_DATA
//#define __PERFORMANCE_DATA
//#endif //__PERFORMANCE_DATA

class CImagePlugin : public CIndexingPlugin, 
                     public MdeMEdiaObserver,
                     public MMediaObjectHandler,
                     public MMMCEventObserver
{
public:
    // Constructor and Destructors.
    /*
     * NewL
     * @return instance of Media plugin
     */
    static CImagePlugin *NewL();
    /*
     * NewLC
     * @return instance of Media plugin
     */
    static CImagePlugin *NewLC();
    /*
     * Destructor
     */
    virtual ~CImagePlugin();
    
public:
    /* 
     * Prepares the plugin for StartHarvesterL and also  starts
     * the favourites db observer.  
     */
    void StartPluginL();
    
    /*
     * @description Start the harvester.
     * @param aQualifiedBaseAppClass the base app class. In this case
     *        use BOOKMARK_QBASEAPPCLASS
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
    CImagePlugin();
    /*
     * Second phase constructor
     */
    void ConstructL();
    
private:
    CCPixIndexer *iIndexer; //Indexer
    CMdeHarvester* iMdeHarvester; //Mde harvester owned
    CMdeObjectQueueManager* iObjectJobQueueManager; //Objectqueue manager owned
    CMdsMediaMonitor*       iMdsMonitor;//MDS monitor owned
    CMdsSessionObjectUtils* iMdsUtils;//Mds utils owned
    CCPIXMediaImageDoc*     iImageDocument;//MDS audio doc owned
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

#endif // CImagePlugin_H
