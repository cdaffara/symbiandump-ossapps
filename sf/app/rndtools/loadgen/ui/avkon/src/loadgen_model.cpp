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
#include "loadgen_utils.h"

#include "loadgen_traces.h"
#include "loadgen_editors.h"
#include "loadgen_loadattributes.h"
#include "loadgen_model.h"
#include "loadgen_app.h"
#include "loadgen_settingsviewdlg.h"
#include "loadgen_maincontainer.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <coeutils.h>
#include <bautils.h>
#include <eikenv.h>
#include <AknQueryDialog.h>
#include <e32math.h> 
#include <u32hal.h>
#include <hal.h> 
#include <hal_data.h>

#include <ecam.h>
#include <aknnotewrappers.h>
#include <featdiscovery.h>

inline TInt64 INIT_SEED()
{
    TTime now; 
    now.HomeTime(); 
    return now.Int64();
}

TInt64 CLoadGenModel::iRandomNumberSeed = INIT_SEED();

// ===================================== MEMBER FUNCTIONS =====================================

CLoadGenModel* CLoadGenModel::NewL()
    {
    CLoadGenModel* self = new(ELeave) CLoadGenModel;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CLoadGenModel::CLoadGenModel() : CActive(EPriorityStandard)
    {
    }
    
// --------------------------------------------------------------------------------------------

void CLoadGenModel::ConstructL()
    {
    iDrawState = EDrawStateInvalid;
    iReferenceNumber = 0;
    
    iEnv = CEikonEnv::Static();
    User::LeaveIfError(iLs.Connect());
    
    iLoadItemList = new(ELeave) CLoadItemList(64);

    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::ActivateModelL()
    {
    // load settings
    //TRAP_IGNORE(LoadSettingsL());

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
    iMemoryEatAttributes.iAmount = 0;
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

void CLoadGenModel::DeActivateModelL()
    {
    Cancel();
    
    // for a faster exit, send the application to background
    TApaTask selfTask(iEnv->WsSession());
    selfTask.SetWgId(iEnv->RootWin().Identifier());
    selfTask.SendToBackground();
    }
    
// --------------------------------------------------------------------------------------------

CLoadGenModel::~CLoadGenModel()
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

void CLoadGenModel::DoCancel()
    {
    iTimer.Cancel();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::RunL()
    {
    // continue    
    //iTimer.After(iStatus, 400000);
    //SetActive();
    }
    
// --------------------------------------------------------------------------------------------

void CLoadGenModel::SetMainContainer(CLoadGenMainContainer* aContainer)
    {
    iMainContainer = aContainer;
    iDrawState = EDrawStateMain;
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::StartNewLoadL(TInt aCommand)
    {
    // show new load query
    CLoadGenLoadTypeEditorBase* dlg = NULL;

    switch ( aCommand )
        {
        case ELoadGenCmdNewLoadCPULoad:
            {
            dlg = CLoadGenCPULoadEditor::NewL(iCpuLoadAttributes);
            break;
            }
        case ELoadGenCmdNewLoadEatMemory:
            {
            dlg = CLoadGenMemoryEatEditor::NewL(iMemoryEatAttributes);
            break;
            }
        case ELoadGenCmdNewLoadPhoneCall:
            {
            dlg = CLoadGenPhoneCallEditor::NewL(iPhoneCallAttributes);
            break;
            }
        case ELoadGenCmdNewLoadNetConn:
            {
            dlg = CLoadGenNetConnEditor::NewL(iNetConnAttributes);
            break;
            }
        case ELoadGenCmdNewLoadKeyPress:
            {
            dlg = CLoadGenKeyPressEditor::NewL(iKeyPressAttributes);
            break;
            }               
        case ELoadGenCmdNewLoadMessages:
            {
            dlg = CLoadGenMessagesEditor::NewL( iMessageAttributes );
            break;
            }
        case ELoadGenCmdNewLoadApplications:
            {
            dlg = CLoadGenApplicationsEditor::NewL( iApplicationsAttributes );
            break;
            }
        case ELoadGenCmdNewLoadPhotoCaptures:
            {
            if( iPhotoCaptureAttributes.iCameraCount > 0 )
                {
                dlg = CLoadGenPhotoCaptureEditor::NewL( iPhotoCaptureAttributes );
                }
            else
                {
                _LIT(message, "Cameras not available");
                CAknErrorNote* errorNote = new(ELeave) CAknErrorNote;
                errorNote->ExecuteLD(message);
                return;
                }
            break;
            }
        case ELoadGenCmdNewLoadBluetooth:
            {
            if( iBluetoothAttributes.iBluetoothSupported )
                {
                dlg = CLoadGenBluetoothEditor::NewL(iBluetoothAttributes);                
                }
            else
                {
                _LIT(message, "Bluetooth device not available");
                CAknErrorNote* errorNote = new(ELeave) CAknErrorNote;
                errorNote->ExecuteLD(message);
                return;
                }
            break;
            }
        case ELoadGenCmdNewLoadPointerEvent:
            {
            dlg = CLoadGenPointerEventEditor::NewL(iPointerEventAttributes);
            break;
            } 

        default:
            {
            User::Panic(_L("Wrong new load"), 111);
            break;
            }
        }

    TBool dlgResult = dlg->RunQueryLD();

    // set back title
    if (iMainContainer)
        iMainContainer->SetDefaultTitlePaneTextL();

    // start creating new load if dialog accepted
    if (dlgResult)
        {
        DoStartNewLoadL(aCommand);
        }

    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::EditLoadL(CLoadBase* aItem)
    {
    // suspend current
    aItem->Suspend();
    
    // show new load query
    CLoadGenLoadTypeEditorBase* dlg = NULL;

    switch ( aItem->Type() )
        {
        case ELoadGenCmdNewLoadCPULoad:
            {
            dlg = CLoadGenCPULoadEditor::NewL(static_cast<CCPULoad*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadEatMemory:
            {
            dlg = CLoadGenMemoryEatEditor::NewL(static_cast<CMemoryEat*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadPhoneCall:
            {
            dlg = CLoadGenPhoneCallEditor::NewL(static_cast<CPhoneCall*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadNetConn:
            {
            dlg = CLoadGenNetConnEditor::NewL(static_cast<CNetConn*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadKeyPress:
            {
            dlg = CLoadGenKeyPressEditor::NewL(static_cast<CKeyPress*>(aItem)->Attributes(), ETrue);
            break;
            }       
        case ELoadGenCmdNewLoadMessages:
            {
            dlg = CLoadGenMessagesEditor::NewL(static_cast<CMessages*>(aItem)->Attributes(), ETrue);
            break;
            }
        case ELoadGenCmdNewLoadApplications:
            {
            dlg = CLoadGenApplicationsEditor::NewL( static_cast<CAppLauncher*>(aItem)->Attributes(), ETrue );
            break;
            }
        case ELoadGenCmdNewLoadPhotoCaptures:
            {
            dlg = CLoadGenPhotoCaptureEditor::NewL( static_cast<CPhotoCapture*>(aItem)->Attributes(), ETrue );
            break;
            }
        case ELoadGenCmdNewLoadBluetooth:
            {
            dlg = CLoadGenBluetoothEditor::NewL( static_cast<CBluetooth*>(aItem)->Attributes(), ETrue );
            break;
            }
        case ELoadGenCmdNewLoadPointerEvent:
            {
            dlg = CLoadGenPointerEventEditor::NewL(static_cast<CPointerEvent*>(aItem)->Attributes(), ETrue);
            break;
            }
        default:
            {
            User::Panic(_L("Wrong edit load"), 114);
            break;
            }
        }

    TBool dlgResult = dlg->RunQueryLD();

    // set back title
    if (iMainContainer)
        iMainContainer->SetDefaultTitlePaneTextL();
    
    // change priority of the load item
    aItem->SetPriority();

    // resume execution the item
    aItem->Resume();
    
    if (dlgResult)
        RefreshViewL(EFalse);
    }
    
// --------------------------------------------------------------------------------------------

void CLoadGenModel::DoStartNewLoadL(TInt aCommand)
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
    iMainContainer->ListBox()->SetCurrentItemIndexAndDraw( iMainContainer->ListBox()->Model()->NumberOfItems()-1 );

    // start the load
    item->Resume();
        
    // refresh again
    RefreshViewL(EFalse);    
    }

// --------------------------------------------------------------------------------------------


void CLoadGenModel::StopAllLoadItemsL()
    {
    DeleteAllLoadItems();
    
    RefreshViewL();
    }
    
// --------------------------------------------------------------------------------------------

void CLoadGenModel::SuspendAllLoadItemsL()
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

void CLoadGenModel::ResumeAllLoadItemsL()
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

void CLoadGenModel::RefreshViewL(TBool aClearSelection)
    {
    if (iMainContainer && iDrawState == EDrawStateMain && iMainContainer->ListBox())
        {
        // clear selections if any
        iMainContainer->ListBox()->ClearSelection();
        
        // set item index to 0
        if (aClearSelection)
            iMainContainer->ListBox()->SetCurrentItemIndex(0); 
        
        // set text items
        iMainContainer->SetListBoxTextArrayL(GenerateListBoxItemTextArrayL());
        }
    }
        
// --------------------------------------------------------------------------------------------

void CLoadGenModel::AppendToLoadItemListL(CLoadBase* aItem)
    {
    if (iLoadItemList)
        iLoadItemList->AppendL( aItem );
    else
        User::Leave(KErrNotReady);
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::DeleteFromLoadItemListL(TInt aIndex)
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

void CLoadGenModel::SuspendOrResumeFromLoadItemListL(TInt aIndex)
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

TInt CLoadGenModel::LoadItemCount() const
    {
    TInt count(0);
    
    if (iLoadItemList)
        count = iLoadItemList->Count();
    
    return count;
    }
    
// --------------------------------------------------------------------------------------------

void CLoadGenModel::DeleteAllLoadItems()
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

CDesCArray* CLoadGenModel::GenerateListBoxItemTextArrayL()
    {
    CDesCArray* textArray = new(ELeave) CDesCArrayFlat(64);
    CleanupStack::PushL(textArray);

    _LIT(KEntryTemplateRedIcon,   "1\t%S\t\t");
    _LIT(KEntryTemplateGreenIcon, "2\t%S\t\t");
    
    for (TInt i=0; i<LoadItemCount(); i++)
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

void CLoadGenModel::ShowItemActionMenuL()
    {
    TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
    
    if (LoadItemCount() > currentItemIndex && currentItemIndex >= 0)
        {
        // show a query dialog    
        TInt queryIndex(0);
        CAknListQueryDialog* listQueryDlg = new(ELeave) CAknListQueryDialog(&queryIndex);

        if (listQueryDlg->ExecuteLD(R_ITEM_ACTION_QUERY))
            {
            // stop
            if (queryIndex == EItemActionMenuTypeStop)
                {
                StopSelectedOrHighlightedItemsL();
                }
            
            // suspend or resume
            else if (queryIndex == EItemActionMenuTypeSuspendResume)
                {
                SuspendOrResumeSelectedOrHighlightedItemsL();
                }
                           
            // edit
            else if (queryIndex == EItemActionMenuTypeEdit)
                {
                EditLoadL(iLoadItemList->At(currentItemIndex));
                }
            }
        }
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::StopSelectedOrHighlightedItemsL()
    {
    const CArrayFix<TInt>* selectionIndexes = iMainContainer->ListBoxSelectionIndexes();

    TInt err(KErrNone);

    // by default use selected items
    if (selectionIndexes && selectionIndexes->Count() > 0)
        {
        CAknQueryDialog* query = CAknQueryDialog::NewL();
        _LIT(KQueryMessage, "Stop %d selections?");
        TFileName queryMsg;
        queryMsg.Format(KQueryMessage, selectionIndexes->Count());
        

        if (query->ExecuteLD(R_GENERAL_CONFIRMATION_QUERY, queryMsg))
            {
            TInt deleteIndexCounter(0);
            TInt ref(0);
            TKeyArrayFix key(0, ECmpTUint16);
            TInt index(0);
            
            TInt loadCount = LoadItemCount();

            for (TInt i=0; i<loadCount; i++)
                {
                ref = i;

                if (selectionIndexes->Find(ref, key, index) == 0)  
                    {
                    TRAP(err, DeleteFromLoadItemListL(i-deleteIndexCounter));
                    deleteIndexCounter++; // amount of indexes decreases after each delete
                    }
                }
            
            RefreshViewL();
            User::LeaveIfError(err);
            }
        }
    
    // or if none selected, use the current item index
    else
        {
        TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
        
        if (LoadItemCount() > currentItemIndex && currentItemIndex >= 0)
            {
            CAknQueryDialog* query = CAknQueryDialog::NewL();
           _LIT(KQueryMessage, "Stop hightlighted item?");

            if (query->ExecuteLD(R_GENERAL_CONFIRMATION_QUERY, KQueryMessage))
                {
                TRAP(err, DeleteFromLoadItemListL(currentItemIndex));
                
                RefreshViewL();
                User::LeaveIfError(err);
                }
            }
        }  
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::SuspendOrResumeSelectedOrHighlightedItemsL()
    {
    const CArrayFix<TInt>* selectionIndexes = iMainContainer->ListBoxSelectionIndexes();

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
    
    // or if none selected, use the current item index
    else
        {
        TInt currentItemIndex = iMainContainer->CurrentListBoxItemIndex();
        
        if (LoadItemCount() > currentItemIndex && currentItemIndex >= 0)
            {
            SuspendOrResumeFromLoadItemListL(currentItemIndex);
            
            RefreshViewL(EFalse);
            }
        }  
    }                                
// --------------------------------------------------------------------------------------------

void CLoadGenModel::LoadSettingsL()
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenModel::SaveSettingsL()
    {
    }
        
// --------------------------------------------------------------------------------------------

TInt CLoadGenModel::LaunchSettingsDialogL()
    {
    return 0;
    }

// --------------------------------------------------------------------------------------------

CDesCArray* CLoadGenModel::ListOfAllAppsL()
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

// End of File
