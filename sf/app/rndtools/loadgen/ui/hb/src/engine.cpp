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


// INCLUDE FILES
#include "engine.h"
#include "enginewrapper.h"
#include "settingsview.h"
#include "loadgen_utils.h"
#include "loadgen_cpuload.h"
#include "loadgen_memoryeat.h"
#include "loadgen_phonecall.h"
#include "loadgen_netconn.h"
#include "loadgen_keypress.h"
#include "loadgen_pointerevent.h"
#include "loadgen_messages.h"
#include "loadgen_applications.h"
#include "loadgen_photocapture.h"
#include "loadgen_bluetooth.h"
#include "loadgen_traces.h"
#include "loadgen_loadattributes.h"
#include "loadgen.hrh"

//#include "loadgen_editors.h"
//#include "loadgen_app.h"
//#include "loadgen_settingsviewdlg.h"
//#include "loadgen_maincontainer.h"

//#include <loadgen.rsg>

#include <coeutils.h>
#include <bautils.h>
#include <eikenv.h>
//#include <AknQueryDialog.h>
#include <e32math.h> 
#include <u32hal.h>
#include <hal.h> 
#include <hal_data.h>

#include <ecam.h>
//#include <aknnotewrappers.h>
#include <featdiscovery.h>

const TInt KZeroIndex = 0;

// ===================================== MEMBER FUNCTIONS =====================================

CEngine* CEngine::NewL(EngineWrapper *aEngineWrapper)
    {
    CEngine* self = new(ELeave) CEngine;
    CleanupStack::PushL(self);
    self->ConstructL(aEngineWrapper);
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CEngine::CEngine() : CActive(EPriorityStandard)
    {
    }
    
// --------------------------------------------------------------------------------------------

void CEngine::ConstructL(EngineWrapper *aEngineWrapper)
    {
	iEngineWrapper = aEngineWrapper;

    iReferenceNumber = 0;
	iCurrentItemIndex = 0;
	iEditExistingLoad = EFalse;
    
    iEnv = CEikonEnv::Static();
    User::LeaveIfError(iLs.Connect());
    
    iLoadItemList = new(ELeave) CLoadItemList(64);

    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
  
  }

// --------------------------------------------------------------------------------------------

void CEngine::ActivateEngineL()
    {
    // load settings
    //TRAP_IGNORE(LoadSettingsL());

	// RAM
	TMemoryInfoV1Buf ramMemory;
	UserHal::MemoryInfo(ramMemory);
	                
	TInt freeRam = ramMemory().iFreeRamInBytes;

	
    // set defaults to attributes
    iCpuLoadAttributes.iId = 0;
    iCpuLoadAttributes.iPriority = EThreadPriorityTypeNormal;
    iCpuLoadAttributes.iMode = ECpuLoadModeYielding;
    iCpuLoadAttributes.iType = ECpuLoadTypePeriodic;
    iCpuLoadAttributes.iLength = 4000;
    iCpuLoadAttributes.iIdle = 2000;
    iCpuLoadAttributes.iRandomVariance = 0;
    iCpuLoadAttributes.iCpu = KCPUSelection_FreeScheduling; // set CPU as "undefined" (can be >= 0 only in SMP environments)
    iCpuLoadAttributes.iCpuCount = UserSvr::HalFunction(EHalGroupKernel, KHalFunction_EKernelHalNumLogicalCpus, 0, 0);
    if( iCpuLoadAttributes.iCpuCount < 1 || iCpuLoadAttributes.iCpuCount > KMaxCPUs )
        {
        // HAL may not support this function, so let's use CPU count 1:
        iCpuLoadAttributes.iCpuCount = 1;
        }
    iMemoryEatAttributes.iId = 0;
    iMemoryEatAttributes.iPriority = EThreadPriorityTypeNormal;
    iMemoryEatAttributes.iSource = 0;
    iMemoryEatAttributes.iType = EMemoryEatTypeMemoryToBeLeft;
    iMemoryEatAttributes.iBuffer = 16*1024;
    iMemoryEatAttributes.iIdle = 0;
    iMemoryEatAttributes.iAmount = freeRam;//0;
    iMemoryEatAttributes.iRandomMin = 1000000;
    iMemoryEatAttributes.iRandomMax = 1500000;
    iMemoryEatAttributes.iAmountDes.Copy(KNullDesC);
    iMemoryEatAttributes.iRandomMinDes.Copy(KNullDesC);
    iMemoryEatAttributes.iRandomMaxDes.Copy(KNullDesC);
    iMemoryEatAttributes.iRandomVariance = 0;

    iPhoneCallAttributes.iId = 0;
    iPhoneCallAttributes.iPriority = EThreadPriorityTypeNormal;
    iPhoneCallAttributes.iDestination.Copy(_L("+358"));
    iPhoneCallAttributes.iLength = 60000;
    iPhoneCallAttributes.iIdle = 10000;
    iPhoneCallAttributes.iRandomVariance = 0;

    iNetConnAttributes.iId = 0;
    iNetConnAttributes.iPriority = EThreadPriorityTypeNormal;
    iNetConnAttributes.iDestination.Copy(_L("http://www.nokia.com"));
    iNetConnAttributes.iIdle = 10000;
    iNetConnAttributes.iRandomVariance = 0;

    iKeyPressAttributes.iId = 0;
    iKeyPressAttributes.iPriority = EThreadPriorityTypeNormal;
    iKeyPressAttributes.iHeartBeat = 100;
    iKeyPressAttributes.iRandomVariance = 0;
    
    iPointerEventAttributes.iId = 0;
    iPointerEventAttributes.iPriority = EThreadPriorityTypeNormal;
    iPointerEventAttributes.iHeartBeat = 1000;
    iPointerEventAttributes.iRandomVariance = 0;

    iMessageAttributes.iId = 0;
    iMessageAttributes.iPriority = EThreadPriorityTypeNormal;
    iMessageAttributes.iMessageType = EMessageTypeSMS;
    iMessageAttributes.iDestination.Copy(_L("+358"));
    iMessageAttributes.iAmount = 5;
    iMessageAttributes.iLength = 160;
    iMessageAttributes.iIdle = 15000;
    iMessageAttributes.iRandomVariance = 0;
    
    iApplicationsAttributes.iId = 0;
    iApplicationsAttributes.iLaunchingInterval = 2000;
    iApplicationsAttributes.iPriority = EThreadPriorityTypeNormal;
    iApplicationsAttributes.iKeyPressType = EApplicationsKeyPressTypeNone;
    iApplicationsAttributes.iMaxOpen = 20;
    iApplicationsAttributes.iHeartBeat = 50;
    iApplicationsAttributes.iRandomVariance = 0;
    
    iPhotoCaptureAttributes.iId = 0;
    iPhotoCaptureAttributes.iPriority = EThreadPriorityTypeNormal;
    iPhotoCaptureAttributes.iIdle = 10000;
    iPhotoCaptureAttributes.iRandomVariance = 0;        
    iPhotoCaptureAttributes.iCameraCount = CCamera::CamerasAvailable();
    if(iPhotoCaptureAttributes.iCameraCount > 0)
        {
        iPhotoCaptureAttributes.iCamera = 0; // Main camera
        }
    else
        {
        iPhotoCaptureAttributes.iCamera = -1;
        }
    
    iBluetoothAttributes.iId = 0;
    iBluetoothAttributes.iPriority = EThreadPriorityTypeNormal;
    iBluetoothAttributes.iIdle = 10000;
    iBluetoothAttributes.iRandomVariance = 0;
    iBluetoothAttributes.iBluetoothSupported = CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdBt);
    }

// --------------------------------------------------------------------------------------------

void CEngine::DeActivateEngineL()
    {
    Cancel();
    
    // for a faster exit, send the application to background
    TApaTask selfTask(iEnv->WsSession());
    selfTask.SetWgId(iEnv->RootWin().Identifier());
    selfTask.SendToBackground();
    }
    
// --------------------------------------------------------------------------------------------

CEngine::~CEngine()
    {
    if (iLoadItemList)
        {
        DeleteAllLoadItems();
        delete iLoadItemList;
        }
    if ( iApplicationsAttributes.iAppsArray )
        {
        delete iApplicationsAttributes.iAppsArray;
        }
    iTimer.Close();
    iLs.Close();
    }

// --------------------------------------------------------------------------------------------

void CEngine::DoCancel()
    {
    iTimer.Cancel();
    }

// --------------------------------------------------------------------------------------------

void CEngine::RunL()
    {
    // continue    
    //iTimer.After(iStatus, 400000);
    //SetActive();
    }
    
// --------------------------------------------------------------------------------------------

void CEngine::StartNewLoadL(TInt aCommand)
    {
    // show new load query
	//CLoadGenLoadTypeEditorBase* dlg = NULL;
	
	if(aCommand == ELoadGenCmdNewLoadPhotoCaptures && 
	   iPhotoCaptureAttributes.iCameraCount == 0)
		{
		_LIT(message, "Cameras not available");
		iEngineWrapper->ShowNote(message);
		return;
		}
	if(aCommand == ELoadGenCmdNewLoadBluetooth &&
	   iBluetoothAttributes.iBluetoothSupported == EFalse)
		{
		_LIT(message, "Bluetooth device not available");
		iEngineWrapper->ShowNote(message); 
		return;
		}
	/*
    switch ( aCommand )
        {
        case ELoadGenCmdNewLoadCPULoad:
            {
            //dlg = CLoadGenCPULoadEditor::NewL(iCpuLoadAttributes);
            break;
            }
        case ELoadGenCmdNewLoadEatMemory:
            {
            //dlg = CLoadGenMemoryEatEditor::NewL(iMemoryEatAttributes);
            break;
            }
        case ELoadGenCmdNewLoadPhoneCall:
            {
            //dlg = CLoadGenPhoneCallEditor::NewL(iPhoneCallAttributes);
            break;
            }
        case ELoadGenCmdNewLoadNetConn:
            {
            //dlg = CLoadGenNetConnEditor::NewL(iNetConnAttributes);
            break;
            }
        case ELoadGenCmdNewLoadKeyPress:
            {
            //dlg = CLoadGenKeyPressEditor::NewL(iKeyPressAttributes);
            break;
            }               
        case ELoadGenCmdNewLoadMessages:
            {
            //dlg = CLoadGenMessagesEditor::NewL( iMessageAttributes );
            break;
            }
        case ELoadGenCmdNewLoadApplications:
            {
            //dlg = CLoadGenApplicationsEditor::NewL( iApplicationsAttributes );
            break;
            }
        case ELoadGenCmdNewLoadPhotoCaptures:
            {
            if( iPhotoCaptureAttributes.iCameraCount > 0 )
                {
                //dlg = CLoadGenPhotoCaptureEditor::NewL( iPhotoCaptureAttributes );
                }
            else
                {
                _LIT(message, "Cameras not available");
				iEngineWrapper->ShowNote(message); 
                //CAknErrorNote* errorNote = new(ELeave) CAknErrorNote;
                //errorNote->ExecuteLD(message);
				return;
                }
            break;
            }
        case ELoadGenCmdNewLoadBluetooth:
            {
            if( iBluetoothAttributes.iBluetoothSupported )
                {
                //dlg = CLoadGenBluetoothEditor::NewL(iBluetoothAttributes);                
                }
            else
                {
                _LIT(message, "Bluetooth device not available");
				iEngineWrapper->ShowNote(message); 
                //CAknErrorNote* errorNote = new(ELeave) CAknErrorNote;
                //errorNote->ExecuteLD(message);
                return;
                }
            break;
            }
        case ELoadGenCmdNewLoadPointerEvent:
            {
            //dlg = CLoadGenPointerEventEditor::NewL(iPointerEventAttributes);
            break;
            } 

        default:
            {
            User::Panic(_L("Wrong new load"), 111);
            break;
            }
        }
	*/
	iEditExistingLoad = EFalse;
    iEngineWrapper->OpenLoadSettings(aCommand, EFalse); // = dlg->RunQueryLD();

    // set back title
    
	//if (iMainContainer) 
    //    iMainContainer->SetDefaultTitlePaneTextL();

    // start creating new load if dialog accepted
    //if (dlgResult)
    //    {
    //    DoStartNewLoadL(aCommand);
    //    }
	// DoStartNewLoadL() called from engine wrapper slot function newLoadAdded() after
	// load settings closed by user select: ok.
    }

// --------------------------------------------------------------------------------------------

void CEngine::EditLoadL(TInt aIndex)
    {
	CLoadBase* aItem = iLoadItemList->At(aIndex);
	iCurrentItemIndex = aIndex;
	iEditExistingLoad = ETrue;
    // suspend current
	if (aItem->State() == CLoadBase::ELoadStateRunning)
		{
		iLoadStatusBeforeEdit = CLoadBase::ELoadStateRunning;
		aItem->Suspend();
		}
    
    // show new load query

	/* CLoadGenLoadTypeEditorBase* dlg = NULL;
	

    switch ( aItem->Type() )
        {
        case ELoadGenCmdNewLoadCPULoad:
            {
            //dlg = CLoadGenCPULoadEditor::NewL(static_cast<CCPULoad*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadEatMemory:
            {
            //dlg = CLoadGenMemoryEatEditor::NewL(static_cast<CMemoryEat*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadPhoneCall:
            {
            //dlg = CLoadGenPhoneCallEditor::NewL(static_cast<CPhoneCall*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadNetConn:
            {
            //dlg = CLoadGenNetConnEditor::NewL(static_cast<CNetConn*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadKeyPress:
            {
            //dlg = CLoadGenKeyPressEditor::NewL(static_cast<CKeyPress*>(aItem)->Attributes(), ETrue);
            break;
            }       
        case ELoadGenCmdNewLoadMessages:
            {
            //dlg = CLoadGenMessagesEditor::NewL(static_cast<CMessages*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadApplications:
            {
            //dlg = CLoadGenApplicationsEditor::NewL( static_cast<CAppLauncher*>(aItem)->Attributes(), ETrue );
            break;
            }
        case ELoadGenCmdNewLoadPhotoCaptures:
            {
            //dlg = CLoadGenPhotoCaptureEditor::NewL( static_cast<CPhotoCapture*>(aItem)->Attributes(), ETrue );
            break;
            }
        case ELoadGenCmdNewLoadBluetooth:
            {
            //dlg = CLoadGenBluetoothEditor::NewL( static_cast<CBluetooth*>(aItem)->Attributes(), ETrue );
            break;
            }
        case ELoadGenCmdNewLoadPointerEvent:
            {
            //dlg = CLoadGenPointerEventEditor::NewL(static_cast<CPointerEvent*>(aItem)->Attributes(), ETrue);
            break;
            }
        default:
            {
            User::Panic(_L("Wrong edit load"), 114);
            break;
            }
        }
		*/
	iEngineWrapper->OpenLoadSettings(aItem->Type(), iEditExistingLoad);
    // ExistingLoadEditedL method called from engine wrapper when load settings view close signal received.
    
	//TBool dlgResult = ETrue; //dlg->RunQueryLD();

    // set back title
	// TODO: check this.
    //if (iMainContainer)
    //    iMainContainer->SetDefaultTitlePaneTextL();
    
    // change priority of the load item
    //aItem->SetPriority();

    // resume execution the item
    //aItem->Resume();
    
    //if (dlgResult)
    //    RefreshViewL(EFalse);
    }

// --------------------------------------------------------------------------------------------    
	
void CEngine::ExistingLoadEditedL()
	{
	CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
	// change priority of the load item
    aItem->SetPriority();
	if (iLoadStatusBeforeEdit == CLoadBase::ELoadStateRunning)
		{
		// resume execution the item
		aItem->Resume();
		RefreshViewL(EFalse);
		}
	// cancel:
	
	}
	
// --------------------------------------------------------------------------------------------    
	
void CEngine::ExistingLoadEditCancelled()
	{
	if (iLoadStatusBeforeEdit == CLoadBase::ELoadStateRunning)
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		// resume execution due to suspend action before edit started:
		aItem->Resume();
		}	
	RefreshViewL(EFalse);
	}
	
// --------------------------------------------------------------------------------------------

void CEngine::DoStartNewLoadL(TInt aCommand)
    {
    CLoadBase* item = NULL;
    
    switch ( aCommand )
        {
        case ELoadGenCmdNewLoadCPULoad:
            {
            if( iCpuLoadAttributes.iCpu == KCPUSelection_AllCPUs )
                {
                // Call this function recursively for all CPUs:
                for( TUint i = 0; i < iCpuLoadAttributes.iCpuCount; ++i )
                    {                    
                    iCpuLoadAttributes.iCpu = i;
                    DoStartNewLoadL(aCommand);
                    }
                // And finally set user selection back to "All CPUs"
                iCpuLoadAttributes.iCpu = KCPUSelection_AllCPUs;
                return;
                }
            else
                {
                item = CCPULoad::NewL(iCpuLoadAttributes, iReferenceNumber);
                }
            break;
            }
        case ELoadGenCmdNewLoadEatMemory:
            {
            item = CMemoryEat::NewL(iMemoryEatAttributes, iReferenceNumber);
            break;
            }
        case ELoadGenCmdNewLoadPhoneCall:
            {
            item = CPhoneCall::NewL(iPhoneCallAttributes, iReferenceNumber);
            break;
            }
        case ELoadGenCmdNewLoadNetConn:
            {
            item = CNetConn::NewL(iNetConnAttributes, iReferenceNumber);
            break;
            }
            
        case ELoadGenCmdNewLoadKeyPress:
            {
            item = CKeyPress::NewL(iKeyPressAttributes, iReferenceNumber);
            break;
            }        
        case ELoadGenCmdNewLoadMessages:
            {
            item = CMessages::NewL( iMessageAttributes, iReferenceNumber );
            break;
            }
        case ELoadGenCmdNewLoadApplications:
            {
            iApplicationsAttributes.iAppsArray = ListOfAllAppsL();
            item = CAppLauncher::NewL( iApplicationsAttributes, iReferenceNumber );            
            break;
            }
        case ELoadGenCmdNewLoadPhotoCaptures:
            {
            item = CPhotoCapture::NewL( iPhotoCaptureAttributes, iReferenceNumber);
            break;
            }
        case ELoadGenCmdNewLoadBluetooth:
            {
            item = CBluetooth::NewL( iBluetoothAttributes, iReferenceNumber);            
            break;
            }
        case ELoadGenCmdNewLoadPointerEvent:
            {
            item = CPointerEvent::NewL(iPointerEventAttributes, iReferenceNumber);
            break;
            }
            
        default:
            {
            User::Panic(_L("Wrong new load"), 112);
            break;
            }
        }
    
    iReferenceNumber++;

    // add item to the array
    AppendToLoadItemListL(item);

    // update the listbox
    RefreshViewL(EFalse);

    // set item index to the end
	// Check this feature to be done in Orbit UI.
    // iMainContainer->ListBox()->SetCurrentItemIndexAndDraw( iMainContainer->ListBox()->Model()->NumberOfItems()-1 );

    // start the load
    item->Resume();
        
    // refresh again
    RefreshViewL(EFalse);    
    }

// --------------------------------------------------------------------------------------------


void CEngine::StopAllLoadItemsL()
    {
    DeleteAllLoadItems();
    
    RefreshViewL();
    }
    
// --------------------------------------------------------------------------------------------

void CEngine::SuspendAllLoadItemsL()
    {
    for (TInt i=0; i<LoadItemCount(); i++)
        {
        if ( iLoadItemList->At(i) )
            {
            iLoadItemList->At(i)->Suspend();
            }
        }
        
    RefreshViewL();    
    }
// --------------------------------------------------------------------------------------------

void CEngine::ResumeAllLoadItemsL()
    {
    for (TInt i=0; i<LoadItemCount(); i++)
        {
        if ( iLoadItemList->At(i) )
            {
            iLoadItemList->At(i)->Resume();
            }
        }

    RefreshViewL();    
    }    
    
// --------------------------------------------------------------------------------------------

void CEngine::RefreshViewL(TBool aClearSelection)
    {
	// clear selections if any
	iEngineWrapper->ClearSelection();
	if (aClearSelection) 
		{
		iEngineWrapper->SetCurrentItemIndex(KZeroIndex);
		}
	iEngineWrapper->ReFreshView(GenerateListBoxItemTextArrayL());
    }

// --------------------------------------------------------------------------------------------

CDesCArray* CEngine::GenerateListBoxItemTextArrayL()
    {
    CDesCArray* textArray = new(ELeave) CDesCArrayFlat(64);
    CleanupStack::PushL(textArray);

    _LIT(KEntryTemplateRedIcon,   "1\t%S\t\t");
    _LIT(KEntryTemplateGreenIcon, "2\t%S\t\t");
    TInt cnt = LoadItemCount();
    for (TInt i=0; i< cnt; i++)
        {
        // add description from each entry
        TBuf<256> textEntry;
        TBuf<256> description = iLoadItemList->At(i)->Description();
        
        if (iLoadItemList->At(i)->State() == CLoadBase::ELoadStateRunning)
            textEntry.Format(KEntryTemplateGreenIcon, &description);
        else
            textEntry.Format(KEntryTemplateRedIcon, &description);
        
        textArray->AppendL(textEntry);
        }    
    
    CleanupStack::Pop(); //textArray
    return textArray;
    }
	
// --------------------------------------------------------------------------------------------

void CEngine::AppendToLoadItemListL(CLoadBase* aItem)
    {
    if (iLoadItemList)
        iLoadItemList->AppendL( aItem );
    else
        User::Leave(KErrNotReady);
    }

// --------------------------------------------------------------------------------------------

void CEngine::DeleteFromLoadItemListL(TInt aIndex)
    {
    if (iLoadItemList)
        {
        if (iLoadItemList->Count() > aIndex && aIndex >= 0)
            {
            // first call delete on the item
            if (iLoadItemList->At(aIndex))
                {                                
                delete iLoadItemList->At(aIndex);
                iLoadItemList->At(aIndex) = NULL;
                }
            
            // and them remove the array entry
            iLoadItemList->Delete(aIndex);
            iLoadItemList->Compress();
            }
        else
            User::Leave(KErrNotFound);
        }
    else
        User::Leave(KErrNotReady);
    }

// --------------------------------------------------------------------------------------------

void CEngine::SuspendOrResumeFromLoadItemListL(TInt aIndex)
    {
    if (iLoadItemList)
        {
        if (iLoadItemList->Count() > aIndex && aIndex >= 0)
            {
            if (iLoadItemList->At(aIndex))
                {
                if (iLoadItemList->At(aIndex)->State() == CLoadBase::ELoadStateRunning)
                    {
                    iLoadItemList->At(aIndex)->Suspend();
                    }
                else if (iLoadItemList->At(aIndex)->State() == CLoadBase::ELoadStateSuspended)
                    {
                    iLoadItemList->At(aIndex)->Resume();
                    }
                }
            }
        else
            User::Leave(KErrNotFound);
        }
    else
        User::Leave(KErrNotReady);
    }
            
// --------------------------------------------------------------------------------------------

TInt CEngine::LoadItemCount() const
    {
    TInt count(0);
    
    if (iLoadItemList)
        count = iLoadItemList->Count();
    
    return count;
    }
    
// --------------------------------------------------------------------------------------------

void CEngine::DeleteAllLoadItems()
    {
    // call delete for each entry and free memory allocated for the array
    for (TInt i=0; i<LoadItemCount(); i++)
        {
        if ( iLoadItemList->At(i) )
            {            
            delete iLoadItemList->At(i);
            iLoadItemList->At(i) = NULL;
            }
        }
        
    iLoadItemList->Reset();    
    }  
          
// --------------------------------------------------------------------------------------------

void CEngine::StopSelectedOrHighlightedItemsL(const CArrayFix<TInt>* aSelectionIndexes)
    {
    TInt err(KErrNone);

    // by default use selected items
    if (aSelectionIndexes && aSelectionIndexes->Count() > 0)
        {
        TInt deleteIndexCounter(0);
        TInt ref(0);
        TKeyArrayFix key(0, ECmpTUint16);
        TInt index(0);
        
        TInt loadCount = LoadItemCount();

        for (TInt i=0; i<loadCount; i++)
            {
            ref = i;

            if (aSelectionIndexes->Find(ref, key, index) == 0)  
                {
                TRAP(err, DeleteFromLoadItemListL(i-deleteIndexCounter));
                deleteIndexCounter++; // amount of indexes decreases after each delete
                }
            }
            
            RefreshViewL();
            User::LeaveIfError(err);
        }
    }

// --------------------------------------------------------------------------------------------

void CEngine::SuspendOrResumeSelectedOrHighlightedItemsL()
    {
    const CArrayFix<TInt>* selectionIndexes = iEngineWrapper->QueryListSelectedIndexesOrCurrentItemL(); //iMainContainer->ListBoxSelectionIndexes();

    // by default use selected items
    if (selectionIndexes && selectionIndexes->Count() > 0)
        {
        TInt ref(0);
        TKeyArrayFix key(0, ECmpTUint16);
        TInt index(0);

        for (TInt i=0; i<LoadItemCount(); i++)
            {
            ref = i;

            if (selectionIndexes->Find(ref, key, index) == 0)  
                {
                SuspendOrResumeFromLoadItemListL(i);
                }
            }
        
        RefreshViewL(EFalse);
        }  
    }                                
// --------------------------------------------------------------------------------------------

void CEngine::LoadSettingsL()
    {
    }

// --------------------------------------------------------------------------------------------

void CEngine::SaveSettingsL()
    {
    }
        
// --------------------------------------------------------------------------------------------

TInt CEngine::LaunchSettingsDialogL()
    {
    return 0;
    }
    
// --------------------------------------------------------------------------------------------
CDesCArray* CEngine::ListOfAllAppsL()
    {
    const TInt KMaxAppsArraySize=250;
    RApaLsSession ls;
    CleanupClosePushL(ls);

    User::LeaveIfError( ls.Connect() );
    CDesCArray* allAppsArray = new(ELeave) CDesCArrayFlat( KMaxAppsArraySize );
    CleanupStack::PushL( allAppsArray );
    // reset the apps list
    allAppsArray->Reset();

    // search all apps
    TApaAppInfo appInfo;
    User::LeaveIfError(ls.GetAllApps());


    while ( ls.GetNextApp( appInfo ) == KErrNone )
        {
        allAppsArray->AppendL( appInfo.iFullName );      
        }
    CleanupStack::Pop( allAppsArray );
    CleanupStack::PopAndDestroy( &ls );
   

    // remove loadgen.* from the list
    for ( TInt i = 0; i < allAppsArray->MdcaCount(); i++ )
        {
        if ( allAppsArray->MdcaPoint(i).FindF( _L("\\loadgen.") ) != KErrNotFound )
            {
            allAppsArray->Delete(i);
            allAppsArray->Compress();
            break;
            }
        }

    // sort the elements
    allAppsArray->Sort();

    return allAppsArray; // ownership transferred
    }

// --------------------------------------------------------------------------------------------

void CEngine::LaunchPerfMonL()
	{
	// launch Performance Monitor
	const TUid KUidPerfMon = { 0x20011385 };
	
	RWsSession ws;
	User::LeaveIfError( ws.Connect() );
	CleanupClosePushL(ws);
	
	RApaLsSession ls;
	User::LeaveIfError( ls.Connect() );
	CleanupClosePushL(ls);
	
	
	// try to find the task of PerfMon
	TApaTaskList tasklist(ws);
	TApaTask task = tasklist.FindApp(KUidPerfMon);

	if (task.Exists())
		{
		// task exists, bring it to foreground
		task.BringToForeground();
		}
	else
		{
		TApaAppInfo appInfo;
		User::LeaveIfError(ls.GetAppInfo(appInfo, KUidPerfMon));
		
		CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
		cmdLine->SetExecutableNameL(appInfo.iFullName);
		cmdLine->SetCommandL(EApaCommandBackground);

		// start the app
		User::LeaveIfError(ls.StartApp(*cmdLine));
		
		CleanupStack::PopAndDestroy(); //cmdLine                
		}
	
	CleanupStack::PopAndDestroy(2); //ws,ls
	}

// --------------------------------------------------------------------------------------------
	
TCPULoadAttributes CEngine::GetCPULoadAttributes()
	{
	if (iEditExistingLoad == EFalse) 
		{
		return iCpuLoadAttributes;
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CCPULoad*>(aItem)->Attributes());
		}
	}

// --------------------------------------------------------------------------------------------

void CEngine::ChangeCPULoadAttributes(const TCPULoadAttributes& aAttributes) 
	{ 
	if (iEditExistingLoad == EFalse) 
		{
		iCpuLoadAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CCPULoad*>(aItem)->SetAttributes(aAttributes);
		}
	}

// --------------------------------------------------------------------------------------------

TMemoryEatAttributes CEngine::GetMemoryEatAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iMemoryEatAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CMemoryEat*>(aItem)->Attributes());
		}
	}

// --------------------------------------------------------------------------------------------

void CEngine::ChangeMemoryEatAttributes(const TMemoryEatAttributes& aAttributes) 
	{ 
	if (iEditExistingLoad == EFalse) 
		{
		iMemoryEatAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CMemoryEat*>(aItem)->SetAttributes(aAttributes);
		}
	}	
	
// --------------------------------------------------------------------------------------------

TPhoneCallAttributes CEngine::GetPhoneCallAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iPhoneCallAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CPhoneCall*>(aItem)->Attributes());
		}
	}

// --------------------------------------------------------------------------------------------	

void CEngine::ChangePhoneCallAttributes(const TPhoneCallAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iPhoneCallAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CPhoneCall*>(aItem)->SetAttributes(aAttributes);
		}	
	}
	
// --------------------------------------------------------------------------------------------	

TNetConnAttributes CEngine::GetNetConnAttributes()
	{ 
  
	if (iEditExistingLoad == EFalse)
		{	
		return iNetConnAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CNetConn*>(aItem)->Attributes());
		}
	
	}

// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangeNetConnAttributes(const TNetConnAttributes& aAttributes)
	{
    
	if (iEditExistingLoad == EFalse) 
		{
		iNetConnAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CNetConn*>(aItem)->SetAttributes(aAttributes);
		}
			
	}
	
// --------------------------------------------------------------------------------------------	

TKeyPressAttributes CEngine::GetKeyPressAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iKeyPressAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CKeyPress*>(aItem)->Attributes());
		}
	}	

	
// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangeKeyPressAttributes(const TKeyPressAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iKeyPressAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CKeyPress*>(aItem)->SetAttributes(aAttributes);
		}	
	}	
	
// --------------------------------------------------------------------------------------------	

TMessageAttributes CEngine::GetMessageAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iMessageAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CMessages*>(aItem)->Attributes());
		}
	}	
	
// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangeMessageAttributes(const TMessageAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iMessageAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CMessages*>(aItem)->SetAttributes(aAttributes);
		}	
	}

// --------------------------------------------------------------------------------------------	

TApplicationsAttributes CEngine::GetApplicationsAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iApplicationsAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CAppLauncher*>(aItem)->Attributes());
		}
	}	

// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangeApplicationsAttributes(const TApplicationsAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iApplicationsAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CAppLauncher*>(aItem)->SetAttributes(aAttributes);
		}	
	}
	
// --------------------------------------------------------------------------------------------	

TPhotoCaptureAttributes CEngine::GetPhotoCaptureAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iPhotoCaptureAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CPhotoCapture*>(aItem)->Attributes());
		}
	}	

// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangePhotoCaptureAttributes(const TPhotoCaptureAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iPhotoCaptureAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CPhotoCapture*>(aItem)->SetAttributes(aAttributes);
		}	
	}	
	
// --------------------------------------------------------------------------------------------	
	
TBluetoothAttributes CEngine::GetBluetoothAttributes()
	{ 
	if (iEditExistingLoad == EFalse)
		{	
		return iBluetoothAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CBluetooth*>(aItem)->Attributes());
		}
	}		

	
// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangeBluetoothAttributes(const TBluetoothAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iBluetoothAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CBluetooth*>(aItem)->SetAttributes(aAttributes);
		}	
	} 	
	
// --------------------------------------------------------------------------------------------	
	
TPointerEventAttributes CEngine::GetPointerEventAttributes()
	{
	if (iEditExistingLoad == EFalse)
		{	
		return iPointerEventAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		return (static_cast<CPointerEvent*>(aItem)->Attributes());
		}
	}

// --------------------------------------------------------------------------------------------		
	
void CEngine::ChangePointerEventAttributes(const TPointerEventAttributes& aAttributes)
	{
	if (iEditExistingLoad == EFalse) 
		{
		iPointerEventAttributes = aAttributes; 
		}
	else 
		{
		CLoadBase* aItem = iLoadItemList->At(iCurrentItemIndex);
		static_cast<CPointerEvent*>(aItem)->SetAttributes(aAttributes);
		}	
	} 
	
// --------------------------------------------------------------------------------------------
	
// End of File
