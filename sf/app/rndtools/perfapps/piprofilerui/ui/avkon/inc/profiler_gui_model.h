/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef PROFILER_GUI_MODEL_H
#define PROFILER_GUI_MODEL_H

// SYSTEM INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <apgcli.h>
#include <gdi.h>
#include <utf.h>
#include <e32property.h>    // RProperty

// LOCAL INCLUDES
#include "profiler_gui_mainview.h"
#include "profiler_gui.hrh"

// COMMON INCLUDES
#include <piprofiler/ProfilerConfig.h>
#include <piprofiler/ProfilerAttributes.h>
#include <piprofiler/ProfilerEngineStatusChecker.h>

// setting keys (do not change uids of existing keys to maintain compatibility to older versions!)
const TUid KPSettingPluginNameMode                         = { 0x00 };
const TUid KPSettingPluginSaveFileDrive                    = { 0x01 };
const TUid KPSettingPluginTraceOutput                      = { 0x02 };
const TUid KPSettingPluginSaveFilePrefix                   = { 0x03 };
const TUid KPSettingPluginTraceMode                        = { 0x04 };


// FORWARD DECLARATIONS
class CProfilerGuiMainView;
class CProfilerGuiMainContainer;
class CEikonEnv;
class TSamplerAttributes;
class CProfilerEngineStatusChecker;
class MProfilerStatusObserver;


typedef CArrayFixSeg<TSamplerAttributes> CSamplerItemList;



class CProfilerGuiModel : public CActive, MProfilerStatusObserver
	{
private:
    enum TContainerDrawState
    	{
    	EDrawStateInvalid = -1,
    	EDrawStateMain
    	};
 public:
	static CProfilerGuiModel* NewL();
	~CProfilerGuiModel();
	void ActivateModelL();
	void DeActivateModelL();
	void SetMainView(CProfilerGuiMainView* aMainView);
	void UpdateState(TInt aState);
	TBool GetSelectedItemHasSettings();
    TBool GetSelectedItemEnabled();
    TBool GetSelectedItemHidden();
    TInt EditSelectedSamplerL(TInt index);
    void SelectedSamplerInfoL(TInt index);
    void TerminateProfilerL();
    TBool CheckTraceLocationSanityL(TGeneralAttributes& aAttr, TBool aQuietCheck);
 private:
    void RunL();
	void DoCancel();
	void LoadPluginsL();	
    TInt EditSamplerL(TSamplerAttributes& aItem);
    void SamplerInfoL(TSamplerAttributes& aItem);
private:
	CProfilerGuiModel();
	void ConstructL();
    TInt LoadGeneralSettingsL();

    void AppendToSamplerItemListL(TSamplerAttributes& aItem);
    void DisableOrEnableFromSamplerItemListL(TInt aIndex);
    void DeleteAllSamplerItems();
    void RefreshViewL(TBool aClearSelection=ETrue);
    void LaunchProfilerEngineL();
    void UpdateUIRunningStateL();
    TInt FindProcessL(RProcess& aProc);
    
public:
    
    enum TProfilingMode
        {
        EProfilingModeNormal = 0,
        EProfilingModeTimed
        };
    
 	void StartAllSamplerItemsL(TProfilingMode aProfilingMode = EProfilingModeNormal);
    void StopAllSamplerItemsL();
    void DeleteAllSamplerItemsL();
    void DisableAllSamplerItemsL();
    void EnableAllSamplerItemsL();
            
    TInt SamplerItemCount() const;
    void ShowItemActionMenuL();
    void StopSelectedOrHighlightedItemsL();
    void DisableOrEnableSelectedOrHighlightedItemsL();

    CDesCArray* GenerateListBoxItemTextArrayL();
	void StartNewSamplerL(TInt aCommand);
    void AddNewSamplersL(CArrayFixFlat<TSamplerAttributes>& iSamplerAttributes);

    void SaveGeneralSettingsL();
    void SetMainContainer(CProfilerGuiMainContainer* aContainer);
    TInt LaunchSettingsDialogL();
    inline CEikonEnv* EikonEnv() { return iEnv; }
    inline CProfilerGuiMainContainer* MainContainer() { return iMainContainer; }
    inline TBool SamplerItemsExists() { return iSamplerItemList->Count() > 0; } 
    inline TGeneralAttributes const& GeneralSettings() const { return iGeneralAttributes; }
    
    TPtrC GetWriterInfoNoteL(const TDesC& aNote);
    void AttachClient();
    void RemoveClient();

public:
    // from MProfilerStatusObserver
    void NotifyContainerReadyL();
    void HandleProfilerStatusChange( KProfilerStatus aStatus );
    void HandleProfilerErrorL( TInt aError );
        
private:
	static const TInt                   iSamplerNameMode = ESamplerNameLong;
    CProfilerGuiMainContainer*          iMainContainer;
    CEikonEnv*                          iEnv;
    TInt                                iDrawState;
    CSamplerItemList*                   iSamplerItemList;
    TInt                                iReferenceNumber;
    CArrayFixFlat<TSamplerAttributes>*	iSamplerAttributes;
    TGeneralAttributes                  iGeneralAttributes;
    TBool                               iProfilerStatus;   // Status of Engine before it was launched by this application
    CProfilerGuiMainView*               iMainView;

    // checkers
    CProfilerEngineStatusChecker*       iStatusChecker;
public:
    TInt							     iState;
    };

#endif
