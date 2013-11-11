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
#include "loadgen_editors.h"
#include "loadgen_loadattributes.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <AknForm.h> 
#include <eikedwin.h> 
#include <eikmfne.h>
#include <AknPopupFieldText.h>
#include <AknQueryValueText.h>
#include <eikspane.h> 
#include <akntitle.h> 
#include <eikcapc.h>
#include <e32hal.h> 

// ===================================== MEMBER FUNCTIONS =====================================

// --------------------------------------------------------------------------------------------

CLoadGenLoadTypeEditorBase::CLoadGenLoadTypeEditorBase(TBool aEditingExisting):
    iEditingExisting(aEditingExisting)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenLoadTypeEditorBase::ConstructL(const TDesC& aTitleText)
    {
    CAknForm::ConstructL();

    // set title text
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( aTitleText );
    }

// --------------------------------------------------------------------------------------------

void CLoadGenLoadTypeEditorBase::PreLayoutDynInitL()
    {
    CAknForm::PreLayoutDynInitL();
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenLoadTypeEditorBase::OkToExitL(TInt /*aButtonId*/)
    {
    return ETrue;
    }

// --------------------------------------------------------------------------------------------

void CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(TInt aControlId)
    {
    CAknForm::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenLoadTypeEditorBase::RunQueryLD()
    {
    User::Panic(_L("Unk.form"), 222);
    
    return ETrue;
    }
               
// --------------------------------------------------------------------------------------------

TBool CLoadGenLoadTypeEditorBase::DoRunQueryLD(TInt aResource)
    {
    return ExecuteLD(aResource);
    }

// --------------------------------------------------------------------------------------------

void CLoadGenLoadTypeEditorBase::InsertFieldAfterL(TInt aResourceId, TInt aControlId, TInt aPrevControlId)
    {
    CCoeControl* prevControl = Control(aPrevControlId);
    __ASSERT_DEBUG(prevControl, User::Panic(_L("Form.Ctrl.Null"), 111));
    TInt pos = FindLineIndex(*prevControl);
    pos = pos + 1;
    InsertLineL(pos, aResourceId);
    Line(aControlId)->ActivateL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenLoadTypeEditorBase::UpdateFormL()
    {
    UpdatePageL(ETrue);
    }
               
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenCPULoadEditor* CLoadGenCPULoadEditor::NewL(TCPULoadAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenCPULoadEditor* self = new(ELeave) CLoadGenCPULoadEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenCPULoadEditor::~CLoadGenCPULoadEditor()
    {
    delete iCpuQueryValText;
    delete iCpuTextArray;
    delete iCPUsArray;
    }

// --------------------------------------------------------------------------------------------

CLoadGenCPULoadEditor::CLoadGenCPULoadEditor(TCPULoadAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenCPULoadEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("CPU load"));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenCPULoadEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();

    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenCPULoadQueryPriority));
    CAknPopupFieldText* edMode = static_cast<CAknPopupFieldText*>(Control(ELoadGenCPULoadQueryMode));
    CAknPopupFieldText* edType = static_cast<CAknPopupFieldText*>(Control(ELoadGenCPULoadQueryType));
    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    edMode->SetCurrentValueIndex(iAttributes.iMode);
    edType->SetCurrentValueIndex(iAttributes.iType);
    
    LoadCPUsL();

    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

void CLoadGenCPULoadEditor::LoadCPUsL()
    {
    // if we are running in SMP environment
    if (iAttributes.iCpuCount > 1)
        {
        // insert possibility to select which cpu the load is generated to
        InsertFieldAfterL(R_CPULOAD_DLG_LINE_CPU, ELoadGenCPULoadQueryCpu, ELoadGenCPULoadQueryType);

        iCPUsArray = new (ELeave) CDesCArrayFlat( 5 );
        
        // "Free scheduling" means that no specific cpu is selected. Scheduler desides
        // which CPUu runs load thread at which time.
        iCPUsArray->AppendL(_L("Free scheduling"));
        
        // add CPUs
        for (int i = 0; i < iAttributes.iCpuCount; i++) 
            {
            TBuf<10> cpu;
            _LIT(KCPU, "CPU%d");
            cpu.Format(KCPU, i);
            iCPUsArray->AppendL(cpu);
            }
        
        // "All CPUs" means that one thread is started for each CPU.
        iCPUsArray->AppendL(_L("All CPUs"));
    
        iCpuTextArray = CAknQueryValueTextArray::NewL();
        iCpuTextArray->SetArray( *iCPUsArray );  
        iCpuQueryValText = CAknQueryValueText::NewL();    
        iCpuQueryValText->SetArrayL( iCpuTextArray );
        // CPU is not defined, set "Free scheduling" as default selection in query list
        if (iAttributes.iCpu < 0)
            {
            iCpuQueryValText->SetCurrentValueIndex(0);
            }
        // "All CPUs" selected. It is last choise in the selection list.
        else if (iAttributes.iCpu == KCPUSelection_AllCPUs)
            {
            iCpuQueryValText->SetCurrentValueIndex(iCpuTextArray->MdcaCount()-1);
            }
        // CPU is defined, set the correct CPU as default selection in query list
        else
            {
            // iCpu starts from zero (i.e. CPU0 is first CPU), but index zero
            // in iCpuQueryValText is "Free scheduling", hence iCpu+1.
            iCpuQueryValText->SetCurrentValueIndex(iAttributes.iCpu+1);
            }

        CAknPopupField* popup = static_cast<CAknPopupField*>(Control( ELoadGenCPULoadQueryCpu)); 
        popup->SetQueryValueL( iCpuQueryValText );
        }
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenCPULoadEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenCPULoadQueryPriority));
        CAknPopupFieldText* edMode = static_cast<CAknPopupFieldText*>(Control(ELoadGenCPULoadQueryMode));
        CAknPopupFieldText* edType = static_cast<CAknPopupFieldText*>(Control(ELoadGenCPULoadQueryType));
        CEikNumberEditor* edPeak = static_cast<CEikNumberEditor*>(ControlOrNull(ELoadGenCPULoadQueryLength));
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(ControlOrNull(ELoadGenCPULoadQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(ControlOrNull(ELoadGenCPULoadQueryVariance));
        
        // we are running in SMP environment
        if (iAttributes.iCpuCount > 1)
            {
            TInt currentValueIndex = iCpuQueryValText->CurrentValueIndex();
            // user selected a specific CPU in which the load thread should be run in.
            if (currentValueIndex == 0)
                {
                // User selected "Free scheduling"
                iAttributes.iCpu = KCPUSelection_FreeScheduling;
                }            
            else if( currentValueIndex == iCpuTextArray->MdcaCount()-1 )
                {
                // User selected "All CPUs", which is the last item in the selection list
                iAttributes.iCpu = KCPUSelection_AllCPUs;
                }
            else
                {                
                // iCpu should start from zero (CPU0 is the first cpu)
                // but zero in currentValueIndex means "Free scheduling". CPU0 in
                // currentValueIndex is at index 1, hence -1.
                iAttributes.iCpu = currentValueIndex - 1;
                }
            }

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        iAttributes.iMode = edMode->CurrentValueIndex();
        iAttributes.iType = edType->CurrentValueIndex();
        if (edPeak) iAttributes.iLength = edPeak->Number();
        if (edIdle) iAttributes.iIdle = edIdle->Number();
        if (edVariance) iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenCPULoadEditor::UpdateVisibilitiesOfFieldsL(TBool aFormInit)
    {
    // filter based on selected cpu load type
    CAknPopupFieldText* edType = static_cast<CAknPopupFieldText*>( Control( ELoadGenCPULoadQueryType ) );

    if ( !aFormInit && edType->CurrentValueIndex() == ECpuLoadTypeContinuous )
        {
        // save values before deleting the items
        CCoeControl* control = ControlOrNull( ELoadGenCPULoadQueryLength );
        if ( control )
            {
            CEikNumberEditor* edPeak = static_cast<CEikNumberEditor*>( control );
            iAttributes.iLength = edPeak->Number();
            DeleteLine( ELoadGenCPULoadQueryLength );
            control = NULL;
            }
        control = ControlOrNull( ELoadGenCPULoadQueryIdle );
        if ( control )
            {
            CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>( control );
            iAttributes.iIdle = edIdle->Number();
            DeleteLine( ELoadGenCPULoadQueryIdle );
            control = NULL;
            }
        control = ControlOrNull( ELoadGenCPULoadQueryVariance );
        if ( control )
            {
            CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>( control );
            iAttributes.iRandomVariance = edVariance->Number();
            DeleteLine( ELoadGenCPULoadQueryVariance );
            }
        }
    else if ( edType->CurrentValueIndex() == ECpuLoadTypePeriodic )
        {
        // insert new fields
        InsertFieldAfterL(R_CPULOAD_DLG_LINE_PERIODIC_LENGTH, ELoadGenCPULoadQueryLength, ELoadGenCPULoadQueryType);
        InsertFieldAfterL(R_CPULOAD_DLG_LINE_PERIODIC_IDLE, ELoadGenCPULoadQueryIdle, ELoadGenCPULoadQueryLength);
        InsertFieldAfterL(R_CPULOAD_DLG_LINE_PERIODIC_VARIANCE, ELoadGenCPULoadQueryVariance, ELoadGenCPULoadQueryIdle);
        
        // load values to just created editors
        CEikNumberEditor* edPeak = static_cast<CEikNumberEditor*>(Control(ELoadGenCPULoadQueryLength));
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenCPULoadQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenCPULoadQueryVariance));
        edPeak->SetNumber(iAttributes.iLength);
        edIdle->SetNumber(iAttributes.iIdle);
        edVariance->SetNumber(iAttributes.iRandomVariance);
        }

    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenCPULoadEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);

    // update visibilities of fields    
    if (aControlId == ELoadGenCPULoadQueryType)
        {
        UpdateVisibilitiesOfFieldsL();
        }
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenCPULoadEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_CPULOAD_FORM_DIALOG);
    }
       
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenMemoryEatEditor* CLoadGenMemoryEatEditor::NewL(TMemoryEatAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenMemoryEatEditor* self = new(ELeave) CLoadGenMemoryEatEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenMemoryEatEditor::~CLoadGenMemoryEatEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenMemoryEatEditor::CLoadGenMemoryEatEditor(TMemoryEatAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMemoryEatEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Memory eat"));
    
    // convert int64 vals to descs
    iAttributes.iAmountDes.Copy(KNullDesC);
    iAttributes.iRandomMinDes.Copy(KNullDesC);
    iAttributes.iRandomMaxDes.Copy(KNullDesC);
    
    iAttributes.iAmountDes.AppendNum(iAttributes.iAmount);
    iAttributes.iRandomMinDes.AppendNum(iAttributes.iRandomMin);
    iAttributes.iRandomMaxDes.AppendNum(iAttributes.iRandomMax);
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMemoryEatEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();

    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQueryPriority));
    CAknPopupFieldText* edSource = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQuerySource));
    CAknPopupFieldText* edType = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQueryType));
    CEikNumberEditor* edBuffer = static_cast<CEikNumberEditor*>(Control(ELoadGenMemoryEatQueryBuffer));
    CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenMemoryEatQueryIdle));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenMemoryEatQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    edSource->SetCurrentValueIndex(iAttributes.iSource);
    edType->SetCurrentValueIndex(iAttributes.iType);
    edBuffer->SetNumber(iAttributes.iBuffer);
    edIdle->SetNumber(iAttributes.iIdle);
    edVariance->SetNumber(iAttributes.iRandomVariance);
            
    UpdateVisibilitiesOfFieldsL(ETrue);
    UpdateAvailableMemoryL();
    
    // set source as dimmed if editing existing item
    if (iEditingExisting)
        {
        SetLineDimmedNow(ELoadGenMemoryEatQuerySource, ETrue); 
        }
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenMemoryEatEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQueryPriority));
        CAknPopupFieldText* edSource = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQuerySource));
        CAknPopupFieldText* edType = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQueryType));
        CEikNumberEditor* edBuffer = static_cast<CEikNumberEditor*>(Control(ELoadGenMemoryEatQueryBuffer));
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenMemoryEatQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenMemoryEatQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        iAttributes.iSource = edSource->CurrentValueIndex();
        iAttributes.iType = edType->CurrentValueIndex();
        iAttributes.iBuffer = edBuffer->Number();
        iAttributes.iIdle = edIdle->Number();
        iAttributes.iRandomVariance = edVariance->Number();


        CEikEdwin* edAmount = static_cast<CEikEdwin*>(ControlOrNull(ELoadGenMemoryEatQueryAmount));
        CEikEdwin* edRandomMin = static_cast<CEikEdwin*>(ControlOrNull(ELoadGenMemoryEatQueryRandomMin));
        CEikEdwin* edRandomMax = static_cast<CEikEdwin*>(ControlOrNull(ELoadGenMemoryEatQueryRandomMax));

        // get desc values and convert to int64
        if (edAmount)
            {
            edAmount->GetText(iAttributes.iAmountDes);

            TLex converter;
            converter.Assign(iAttributes.iAmountDes);
            User::LeaveIfError(converter.Val(iAttributes.iAmount));
            }

        if (edRandomMin)
            {
            edRandomMin->GetText(iAttributes.iRandomMinDes);

            TLex converter;
            converter.Assign(iAttributes.iRandomMinDes);
            User::LeaveIfError(converter.Val(iAttributes.iRandomMin));
            }

        if (edRandomMax)
            {
            edRandomMax->GetText(iAttributes.iRandomMaxDes);

            TLex converter;
            converter.Assign(iAttributes.iRandomMaxDes);
            User::LeaveIfError(converter.Val(iAttributes.iRandomMax));
            
            // swap min max values if they in wrong order
            if (iAttributes.iRandomMin > iAttributes.iRandomMax)
                {
                TInt64 temp(iAttributes.iRandomMin);
                iAttributes.iRandomMin = iAttributes.iRandomMax;
                iAttributes.iRandomMax = temp; 
                }
            
            }                        
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenMemoryEatEditor::UpdateVisibilitiesOfFieldsL(TBool aFormInit)
    {
    // filter based on selected memory eat type
    CAknPopupFieldText* edType = static_cast<CAknPopupFieldText*>(Control(ELoadGenMemoryEatQueryType));

    if (!aFormInit && (edType->CurrentValueIndex() == EMemoryEatTypeMemoryToEat || edType->CurrentValueIndex() == EMemoryEatTypeMemoryToBeLeft))
        {
        // no need for changes if amount field is already active
        CEikNumberEditor* edAmount = static_cast<CEikNumberEditor*>(ControlOrNull(ELoadGenMemoryEatQueryAmount));
        if (edAmount) return;
        
        // save values before deleting the items
        CEikEdwin* edRandomMin = static_cast<CEikEdwin*>(Control(ELoadGenMemoryEatQueryRandomMin));
        CEikEdwin* edRandomMax = static_cast<CEikEdwin*>(Control(ELoadGenMemoryEatQueryRandomMax));
        edRandomMin->GetText(iAttributes.iRandomMinDes);
        edRandomMax->GetText(iAttributes.iRandomMaxDes);
        
        DeleteLine(ELoadGenMemoryEatQueryRandomMin);
        DeleteLine(ELoadGenMemoryEatQueryRandomMax);
        }
    else if (!aFormInit && edType->CurrentValueIndex() == EMemoryEatTypeWavy)
        {
        // save values before deleting the items
        CEikEdwin* edAmount = static_cast<CEikEdwin*>(Control(ELoadGenMemoryEatQueryAmount));
        edAmount->GetText(iAttributes.iAmountDes);
        
        DeleteLine(ELoadGenMemoryEatQueryAmount);
        }
    
    if (edType->CurrentValueIndex() == EMemoryEatTypeMemoryToEat || edType->CurrentValueIndex() == EMemoryEatTypeMemoryToBeLeft)
        {
        // insert new fields
        InsertFieldAfterL(R_MEMORYEAT_DLG_LINE_AMOUNT, ELoadGenMemoryEatQueryAmount, ELoadGenMemoryEatQueryType);
        
        // load values to just created editors
        CEikEdwin* edAmount = static_cast<CEikEdwin*>(Control(ELoadGenMemoryEatQueryAmount));
        edAmount->SetTextL(&iAttributes.iAmountDes);
        }
    else if (edType->CurrentValueIndex() == EMemoryEatTypeWavy)
        {
        // insert new fields
        InsertFieldAfterL(R_MEMORYEAT_DLG_LINE_RANDOM_MIN, ELoadGenMemoryEatQueryRandomMin, ELoadGenMemoryEatQueryType);
        InsertFieldAfterL(R_MEMORYEAT_DLG_LINE_RANDOM_MAX, ELoadGenMemoryEatQueryRandomMax, ELoadGenMemoryEatQueryRandomMin);
        
        // load values to just created editors
        CEikEdwin* edRandomMin = static_cast<CEikEdwin*>(Control(ELoadGenMemoryEatQueryRandomMin));
        CEikEdwin* edRandomMax = static_cast<CEikEdwin*>(Control(ELoadGenMemoryEatQueryRandomMax));
        edRandomMin->SetTextL(&iAttributes.iRandomMinDes);
        edRandomMax->SetTextL(&iAttributes.iRandomMaxDes);
        }
    
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMemoryEatEditor::UpdateAvailableMemoryL()
    {
    // this feature is used only when not editing existing load
    if (!iEditingExisting)
        {
        CAknPopupFieldText* edSource = static_cast<CAknPopupFieldText*>(ControlOrNull(ELoadGenMemoryEatQuerySource));
        CEikEdwin* edAmount = static_cast<CEikEdwin*>(ControlOrNull(ELoadGenMemoryEatQueryAmount));
        
        if (edAmount && edSource)
            {
            iAttributes.iAmountDes.Copy(KNullDesC);

            if (edSource->CurrentValueIndex() == EMemoryEatSourceTypeRAM)
                {
                // RAM
                TMemoryInfoV1Buf ramMemory;
                UserHal::MemoryInfo(ramMemory);
                
                iAttributes.iAmountDes.AppendNum(ramMemory().iFreeRamInBytes);
                }
            else
                {
                // get drive
                TVolumeInfo volumeInfo;
                if (CEikonEnv::Static()->FsSession().Volume(volumeInfo, edSource->CurrentValueIndex()+1) == KErrNone)
                    {
                    iAttributes.iAmountDes.AppendNum(volumeInfo.iFree);
                    }
                else
                    {
                    iAttributes.iAmountDes.AppendNum(0);
                    }
                }

            edAmount->SetTextL(&iAttributes.iAmountDes);
            }        
        }
    }
        
// --------------------------------------------------------------------------------------------

void CLoadGenMemoryEatEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    
    // update visibilities of fields    
    if (aControlId == ELoadGenMemoryEatQueryType)
        {
        UpdateVisibilitiesOfFieldsL();
        }
    
    // update memory when source changes
    else if (aControlId == ELoadGenMemoryEatQuerySource)
        {
        UpdateAvailableMemoryL();
        }
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenMemoryEatEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_MEMORYEAT_FORM_DIALOG);
    }


// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenPhoneCallEditor* CLoadGenPhoneCallEditor::NewL(TPhoneCallAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenPhoneCallEditor* self = new(ELeave) CLoadGenPhoneCallEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenPhoneCallEditor::~CLoadGenPhoneCallEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenPhoneCallEditor::CLoadGenPhoneCallEditor(TPhoneCallAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPhoneCallEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Phone calls"));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPhoneCallEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();
    
    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenPhoneCallQueryPriority));
    CEikEdwin* edDestination = static_cast<CEikEdwin*>(Control(ELoadGenPhoneCallQueryDestination));
    CEikNumberEditor* edLength = static_cast<CEikNumberEditor*>(Control(ELoadGenPhoneCallQueryLength));
    CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenPhoneCallQueryIdle));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenPhoneCallQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    edDestination->SetTextL(&iAttributes.iDestination);   
    edLength->SetNumber(iAttributes.iLength);   
    edIdle->SetNumber(iAttributes.iIdle);   
    edVariance->SetNumber(iAttributes.iRandomVariance);   
    
    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenPhoneCallEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenPhoneCallQueryPriority));
        CEikEdwin* edDestination = static_cast<CEikEdwin*>(Control(ELoadGenPhoneCallQueryDestination));
        CEikNumberEditor* edLength = static_cast<CEikNumberEditor*>(Control(ELoadGenPhoneCallQueryLength));
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenPhoneCallQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenPhoneCallQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        edDestination->GetText(iAttributes.iDestination);
        iAttributes.iLength = edLength->Number();
        iAttributes.iIdle = edIdle->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenPhoneCallEditor::UpdateVisibilitiesOfFieldsL(TBool /*aFormInit*/)
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPhoneCallEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenPhoneCallEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_PHONECALL_FORM_DIALOG);
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenNetConnEditor* CLoadGenNetConnEditor::NewL(TNetConnAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenNetConnEditor* self = new(ELeave) CLoadGenNetConnEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenNetConnEditor::~CLoadGenNetConnEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenNetConnEditor::CLoadGenNetConnEditor(TNetConnAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenNetConnEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Network conn."));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenNetConnEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();
    
    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenNetConnQueryPriority));
    CEikEdwin* edDestination = static_cast<CEikEdwin*>(Control(ELoadGenNetConnQueryDestination));
    CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenNetConnQueryIdle));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenNetConnQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    edDestination->SetTextL(&iAttributes.iDestination);   
    edIdle->SetNumber(iAttributes.iIdle);   
    edVariance->SetNumber(iAttributes.iRandomVariance);
       
    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenNetConnEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenNetConnQueryPriority));
        CEikEdwin* edDestination = static_cast<CEikEdwin*>(Control(ELoadGenNetConnQueryDestination));
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenNetConnQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenNetConnQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        edDestination->GetText(iAttributes.iDestination);
        iAttributes.iIdle = edIdle->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenNetConnEditor::UpdateVisibilitiesOfFieldsL(TBool /*aFormInit*/)
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenNetConnEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenNetConnEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_NETCONN_FORM_DIALOG);
    }

// --------------------------------------------------------------------------------------------
       
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenKeyPressEditor* CLoadGenKeyPressEditor::NewL(TKeyPressAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenKeyPressEditor* self = new(ELeave) CLoadGenKeyPressEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------------------------

CLoadGenKeyPressEditor::~CLoadGenKeyPressEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenKeyPressEditor::CLoadGenKeyPressEditor(TKeyPressAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenKeyPressEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Key presses"));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenKeyPressEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();

    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenKeyPressQueryPriority));
    CEikNumberEditor* edHeartBeat = static_cast<CEikNumberEditor*>(Control(ELoadGenKeyPressQueryHeartBeat));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenKeyPressQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    edHeartBeat->SetNumber(iAttributes.iHeartBeat);
    edVariance->SetNumber(iAttributes.iRandomVariance);

    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenKeyPressEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenKeyPressQueryPriority));
        CEikNumberEditor* edHeartBeat = static_cast<CEikNumberEditor*>(Control(ELoadGenKeyPressQueryHeartBeat));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenKeyPressQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        iAttributes.iHeartBeat = edHeartBeat->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenKeyPressEditor::UpdateVisibilitiesOfFieldsL(TBool /*aFormInit*/)
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenKeyPressEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenKeyPressEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_KEYPRESS_FORM_DIALOG);
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenMessagesEditor* CLoadGenMessagesEditor::NewL( TMessageAttributes& aAttributes, 
                                                      TBool aEditingExisting )
    {
    CLoadGenMessagesEditor* self = new(ELeave) CLoadGenMessagesEditor( aAttributes, 
                                                                        aEditingExisting );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenMessagesEditor::~CLoadGenMessagesEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenMessagesEditor::CLoadGenMessagesEditor( TMessageAttributes& aAttributes, 
                                                TBool aEditingExisting ) :
                                            CLoadGenLoadTypeEditorBase( aEditingExisting ), 
                                            iAttributes( aAttributes )
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMessagesEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL( _L("Messages") );
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMessagesEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();
    
    // load values to static editors
    CAknPopupFieldText* edPriority = 
                    static_cast<CAknPopupFieldText*>( Control( ELoadGenMessagesQueryPriority ) );
    CAknPopupFieldText* edType = 
                    static_cast<CAknPopupFieldText*>( Control( ELoadGenMessagesQueryType ) );                    
    CEikEdwin* edDestination = 
                    static_cast<CEikEdwin*>( Control( ELoadGenMessagesQueryDestination ) );
    CEikNumberEditor* edAmount = 
                    static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryAmount ) );                
    CEikNumberEditor* edLength = 
                    static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryLength ) );
    CEikNumberEditor* edIdle = 
                    static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryIdle ) );
    CEikNumberEditor* edVariance = 
                    static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryVariance ) );

    edPriority->SetCurrentValueIndex( iAttributes.iPriority );
    edType->SetCurrentValueIndex( iAttributes.iMessageType );
    edDestination->SetTextL( &iAttributes.iDestination );
    edAmount->SetNumber( iAttributes.iAmount );
    edLength->SetNumber( iAttributes.iLength );   
    edIdle->SetNumber( iAttributes.iIdle );   
    edVariance->SetNumber( iAttributes.iRandomVariance );   
       
    UpdateVisibilitiesOfFieldsL( ETrue );
    // set type selection as dimmed if editing existing item
    if (iEditingExisting)
        {
        SetLineDimmedNow(ELoadGenMessagesQueryType, ETrue); 
        }
    
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenMessagesEditor::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOk )
        {
        // store values from editors
        CAknPopupFieldText* edPriority = 
                static_cast<CAknPopupFieldText*>( Control( ELoadGenMessagesQueryPriority ) );
        CAknPopupFieldText* edType = 
                static_cast<CAknPopupFieldText*>( Control( ELoadGenMessagesQueryType ) );                
        CEikEdwin* edDestination = 
                static_cast<CEikEdwin*>( Control( ELoadGenMessagesQueryDestination ) );
        CEikNumberEditor* edLength = 
                static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryLength ) );
        CEikNumberEditor* edAmount = 
                static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryAmount ) );                
        CEikNumberEditor* edIdle = 
                static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryIdle ) );
        CEikNumberEditor* edVariance =
                static_cast<CEikNumberEditor*>( Control( ELoadGenMessagesQueryVariance ) );

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        iAttributes.iMessageType = edType->CurrentValueIndex();
        edDestination->GetText( iAttributes.iDestination );
        iAttributes.iAmount = edAmount->Number();
        iAttributes.iLength = edLength->Number();
        iAttributes.iIdle = edIdle->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenMessagesEditor::UpdateVisibilitiesOfFieldsL( TBool /*aFormInit*/ )
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMessagesEditor::HandleControlStateChangeL( TInt aControlId )
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL( aControlId );
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenMessagesEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD( R_MESSAGES_FORM_DIALOG );
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenApplicationsEditor* CLoadGenApplicationsEditor::NewL( TApplicationsAttributes& aAttributes, 
                                                      TBool aEditingExisting )
    {
    CLoadGenApplicationsEditor* self = new(ELeave) CLoadGenApplicationsEditor( aAttributes, 
                                                                        aEditingExisting );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenApplicationsEditor::~CLoadGenApplicationsEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenApplicationsEditor::CLoadGenApplicationsEditor( TApplicationsAttributes& aAttributes, 
                                                TBool aEditingExisting ) :
                                            CLoadGenLoadTypeEditorBase( aEditingExisting ), 
                                            iAttributes( aAttributes )
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenApplicationsEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL( _L("Applications") );
    }

// --------------------------------------------------------------------------------------------

void CLoadGenApplicationsEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();

    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenApplicationsQueryPriority));
    CEikNumberEditor* edLaunchingInterval = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsLaunchingInterval));
    CAknPopupFieldText* edKeyPressType = static_cast<CAknPopupFieldText*>(Control(ELoadGenApplicationsKeyPressType));
    CEikNumberEditor* edMaxOpen = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsMaxOpen));
    CEikNumberEditor* edHeartBeat = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsKeyPressQueryHeartBeat));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsQueryVariance));

    edPriority->SetCurrentValueIndex( iAttributes.iPriority );
    edLaunchingInterval->SetNumber( iAttributes.iLaunchingInterval );
    edKeyPressType->SetCurrentValueIndex( iAttributes.iKeyPressType );
    edMaxOpen->SetNumber( iAttributes.iMaxOpen );
    edHeartBeat->SetNumber( iAttributes.iHeartBeat );
    edVariance->SetNumber( iAttributes.iRandomVariance );

    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenApplicationsEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenApplicationsQueryPriority));
        CEikNumberEditor* edLaunchingInterval = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsLaunchingInterval));
        CAknPopupFieldText* edKeyPressType = static_cast<CAknPopupFieldText*>(Control(ELoadGenApplicationsKeyPressType));
        CEikNumberEditor* edMaxOpen = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsMaxOpen));
        CEikNumberEditor* edHeartBeat = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsKeyPressQueryHeartBeat));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenApplicationsQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        iAttributes.iLaunchingInterval = edLaunchingInterval->Number();
        iAttributes.iKeyPressType = edKeyPressType->CurrentValueIndex();
        iAttributes.iMaxOpen = edMaxOpen->Number();
        iAttributes.iHeartBeat = edHeartBeat->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       
    
// --------------------------------------------------------------------------------------------

void CLoadGenApplicationsEditor::UpdateVisibilitiesOfFieldsL( TBool /*aFormInit*/ )
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenApplicationsEditor::HandleControlStateChangeL( TInt aControlId )
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL( aControlId );
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenApplicationsEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD( R_APPLICATIONS_FORM_DIALOG );
    }

// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------

CLoadGenPhotoCaptureEditor* CLoadGenPhotoCaptureEditor::NewL(TPhotoCaptureAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenPhotoCaptureEditor* self = new(ELeave) CLoadGenPhotoCaptureEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenPhotoCaptureEditor::~CLoadGenPhotoCaptureEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenPhotoCaptureEditor::CLoadGenPhotoCaptureEditor(TPhotoCaptureAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPhotoCaptureEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Photo captures"));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPhotoCaptureEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();
    
    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenPhotoCaptureQueryPriority));
    //CAknPopupFieldText* edDevice = static_cast<CAknPopupFieldText*>(Control(ELoadGenPhotoCaptureQueryDevice));    
    CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenPhotoCaptureQueryIdle));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenPhotoCaptureQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    //edDevice->SetCurrentValueIndex(0);
    edIdle->SetNumber(iAttributes.iIdle);   
    edVariance->SetNumber(iAttributes.iRandomVariance);
    
    LoadCamerasL();
    
    UpdateVisibilitiesOfFieldsL(ETrue);
    }

void CLoadGenPhotoCaptureEditor::LoadCamerasL()
    {    
    if (iAttributes.iCameraCount > 0)
        {
        // insert possibility to select which cpu the load is generated to
        InsertFieldAfterL(R_PHOTOCAPTURE_DLG_LINE_DEVICE, ELoadGenPhotoCaptureQueryDevice, ELoadGenPhotoCaptureQueryPriority);
        
        iCamerasArray = new (ELeave) CDesCArrayFlat( iAttributes.iCameraCount );   
        
        // add Cameras
        for (int i = 0; i < iAttributes.iCameraCount; i++) 
            {
            TBuf<16> cam;
            _LIT(KCam, "Camera %d");
            cam.Format(KCam, i);
            iCamerasArray->AppendL(cam);
            }    
       
        iCameraTextArray = CAknQueryValueTextArray::NewL();
        iCameraTextArray->SetArray( *iCamerasArray );  
        iCameraQueryValText = CAknQueryValueText::NewL();    
        iCameraQueryValText->SetArrayL( iCameraTextArray );
        if( iAttributes.iCameraCount > 0 )
            {
            iCameraQueryValText->SetCurrentValueIndex(iAttributes.iCamera);
            }
        CAknPopupField* popup = static_cast<CAknPopupField*>(Control( ELoadGenPhotoCaptureQueryDevice)); 
        popup->SetQueryValueL( iCameraQueryValText );        
        }    
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenPhotoCaptureEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenPhotoCaptureQueryPriority));        
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenPhotoCaptureQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenPhotoCaptureQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();        
        iAttributes.iIdle = edIdle->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        
        if (iAttributes.iCameraCount > 1)
            {
            iAttributes.iCamera = iCameraQueryValText->CurrentValueIndex();
            }
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenPhotoCaptureEditor::UpdateVisibilitiesOfFieldsL(TBool /*aFormInit*/)
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPhotoCaptureEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenPhotoCaptureEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_PHOTOCAPTURE_FORM_DIALOG);
    }


// --------------------------------------------------------------------------------------------

CLoadGenBluetoothEditor* CLoadGenBluetoothEditor::NewL(TBluetoothAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenBluetoothEditor* self = new(ELeave) CLoadGenBluetoothEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenBluetoothEditor::~CLoadGenBluetoothEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenBluetoothEditor::CLoadGenBluetoothEditor(TBluetoothAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenBluetoothEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Bluetooth"));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenBluetoothEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();
    
    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenBluetoothQueryPriority));        
    CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenBluetoothQueryIdle));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenBluetoothQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);    
    edIdle->SetNumber(iAttributes.iIdle);   
    edVariance->SetNumber(iAttributes.iRandomVariance);
        
    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenBluetoothEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenBluetoothQueryPriority));        
        CEikNumberEditor* edIdle = static_cast<CEikNumberEditor*>(Control(ELoadGenBluetoothQueryIdle));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenBluetoothQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();        
        iAttributes.iIdle = edIdle->Number();
        iAttributes.iRandomVariance = edVariance->Number();                
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenBluetoothEditor::UpdateVisibilitiesOfFieldsL(TBool /*aFormInit*/)
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenBluetoothEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenBluetoothEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_BLUETOOTH_FORM_DIALOG);
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CLoadGenPointerEventEditor* CLoadGenPointerEventEditor::NewL(TPointerEventAttributes& aAttributes, TBool aEditingExisting)
    {
    CLoadGenPointerEventEditor* self = new(ELeave) CLoadGenPointerEventEditor(aAttributes, aEditingExisting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------------------------

CLoadGenPointerEventEditor::~CLoadGenPointerEventEditor()
    {
    }

// --------------------------------------------------------------------------------------------

CLoadGenPointerEventEditor::CLoadGenPointerEventEditor(TPointerEventAttributes& aAttributes, TBool aEditingExisting) :
    CLoadGenLoadTypeEditorBase(aEditingExisting), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPointerEventEditor::ConstructL()
    {
    CLoadGenLoadTypeEditorBase::ConstructL(_L("Pointer events"));
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPointerEventEditor::PreLayoutDynInitL()
    {
    CLoadGenLoadTypeEditorBase::PreLayoutDynInitL();

    // load values to static editors
    CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenPointerEventQueryPriority));
    CEikNumberEditor* edHeartBeat = static_cast<CEikNumberEditor*>(Control(ELoadGenPointerEventQueryHeartBeat));
    CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenPointerEventQueryVariance));

    edPriority->SetCurrentValueIndex(iAttributes.iPriority);
    edHeartBeat->SetNumber(iAttributes.iHeartBeat);
    edVariance->SetNumber(iAttributes.iRandomVariance);

    UpdateVisibilitiesOfFieldsL(ETrue);
    }

// --------------------------------------------------------------------------------------------

TBool CLoadGenPointerEventEditor::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyOk)
        {
        // store values from editors
        CAknPopupFieldText* edPriority = static_cast<CAknPopupFieldText*>(Control(ELoadGenPointerEventQueryPriority));
        CEikNumberEditor* edHeartBeat = static_cast<CEikNumberEditor*>(Control(ELoadGenPointerEventQueryHeartBeat));
        CEikNumberEditor* edVariance = static_cast<CEikNumberEditor*>(Control(ELoadGenPointerEventQueryVariance));

        iAttributes.iPriority = edPriority->CurrentValueIndex();
        iAttributes.iHeartBeat = edHeartBeat->Number();
        iAttributes.iRandomVariance = edVariance->Number();
        }
    
    return ETrue;
    }       

// --------------------------------------------------------------------------------------------

void CLoadGenPointerEventEditor::UpdateVisibilitiesOfFieldsL(TBool /*aFormInit*/)
    {
    UpdateFormL();
    }

// --------------------------------------------------------------------------------------------

void CLoadGenPointerEventEditor::HandleControlStateChangeL(TInt aControlId)
    {
    CLoadGenLoadTypeEditorBase::HandleControlStateChangeL(aControlId);
    }
    
// --------------------------------------------------------------------------------------------

TBool CLoadGenPointerEventEditor::RunQueryLD()
    {
    return CLoadGenLoadTypeEditorBase::DoRunQueryLD(R_POINTEREVENT_FORM_DIALOG);
    }

// End of File
