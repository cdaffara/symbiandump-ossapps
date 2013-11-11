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

#ifndef CAPPLICATIONSPLUGIN_H
#define CAPPLICATIONSPLUGIN_H

#include <e32base.h>
#include <cindexingplugin.h>
#include <common.h>
#include <apgcli.h> //RApaLsSession
#include <apgnotif.h> //Notification
//#include <WidgetRegistryClient.h>
#include "delayedcallback.h"
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include <widgetregistryclient.h>
#else
#include <usif/scr/scr.h>
#endif
#include <javaregistryincludes.h>

class CCPixIndexer;
class CRepository;
class TAppRegInfo;
/**
 * Applications plugin class. Harvests applictions(exes), widgets and java apps.
 * 
 * Implements CIndexingPlugin, DelayedCallbackObserver & AppListServerObserver.
 * 
 */
class CApplicationsPlugin : public CIndexingPlugin, public MDelayedCallbackObserver, public MApaAppListServObserver
{
public:
    enum THarvesterState
        {
        EHarvesterIdleState,
        EHarvesterStartHarvest        
        };
    
    // Constructors and destructor
    static CApplicationsPlugin* NewL();
    static CApplicationsPlugin* NewLC();
    virtual ~CApplicationsPlugin();

public: // From CIndexingPlugin
	virtual void StartPluginL();
	virtual void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
	void PausePluginL();
	void ResumePluginL();
	
public: // From MDelayedCallbackObserver
    void DelayedCallbackL(TInt aCode);
    void DelayedError(TInt aErrorCode);

public: // From MApaAppListServObserver
    void HandleAppListEvent(TInt aEvent);

private: // Constructors
	CApplicationsPlugin();
	void ConstructL();
	
    /*
     * @description Add, update or delete application document with info in TApaAppInfo based on aActionType.
     * @param aAppInfo: got via GetNextApp().
     * @param aActionType: add, update or delete.
     * @return void
     * Leaves in case of error.
     */
    void CreateApplicationsIndexItemL(RPointerArray<Usif::TAppRegInfo>& aAppInfo, TCPixActionType aActionType);

    /*
     * @description Adds necessary document fields to aDocument for widget with Uid aUid.
     * @param aDocument search document. Not owned.
     * @param aUid: Uid of the widget.
     * @return void
     * Leaves in case of error.
     */
	//void AddWidgetInfoL( CSearchDocument* aDocument, TUid aUid );

	/*
	 * @description Returns true if applicaiton with uid aUid hidden, false otherwise
	 * @param aUid The UID of the application to be checked for hidden property.
	 * @return ETrue if hidden, EFalse otherwise.    
	 */
	TBool IsAppHiddenL(TUid aUid);
	
private:
	CDelayedCallback* iAsynchronizer;  //Owned.	
    CCPixIndexer* iIndexer; // CPix database. Owned.
    //RApaLsSession iApplicationServerSession; //to get application info.
    Usif::RSoftwareComponentRegistry iScrSession;
    Usif::RApplicationInfoView iScrView;
    CApaAppListNotifier* iNotifier; //Owned.
    //State of harvester either to pause/resume
    TBool iIndexState;
    //harvesting state
    THarvesterState iHarvestState;    

#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
#endif
};

#endif // CAPPLICATIONSPLUGIN_H
