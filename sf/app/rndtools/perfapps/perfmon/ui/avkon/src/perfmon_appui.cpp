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


// INCLUDE FILES
#include "perfmon_appui.h"
#include "perfmon_valuesview.h"
#include "perfmon_graphsview.h"
#include "perfmon_datapopupcontainer.h"
#include "perfmon.hrh"
#include "perfmon_model.h"
#include "perfmon_document.h"
#include <perfmon.rsg>

#include <avkon.hrh>
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h> 


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------------------------

void CPerfMonAppUi::ConstructL()
    {
    // disable window server priority control for this application
    iEikonEnv->WsSession().ComputeMode( RWsSession::EPriorityControlDisabled );

    // set as system application to prevent getting shut down events 
    iEikonEnv->SetSystem(ETrue);
    
    BaseConstructL(EAknEnableSkin);

    iModel = static_cast<CPerfMonDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();

    // Show tabs for main views from resources
    CEikStatusPane* sp = StatusPane();

    // Fetch pointer to the default navi pane control
    iNaviPane = (CAknNavigationControlContainer*)sp->ControlL( 
        TUid::Uid(EEikStatusPaneUidNavi));

    // Tabgroup has been read from resource and it were pushed to the navi pane. 
    // Get pointer to the navigation decorator with the ResourceDecorator() function. 
    // Application owns the decorator and it has responsibility to delete the object.
    iDecoratedTabGroup = iNaviPane->ResourceDecorator();
    if (iDecoratedTabGroup)
        {
        iTabGroup = (CAknTabGroup*) iDecoratedTabGroup->DecoratedControl();
        }
        
    CPerfMonValuesView* valuesView = new(ELeave) CPerfMonValuesView;
    CleanupStack::PushL(valuesView);
    valuesView->ConstructL();
    AddViewL(valuesView);           // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();            // valuesView

    CPerfMonGraphsView* graphsView = new(ELeave) CPerfMonGraphsView;
    CleanupStack::PushL(graphsView);
    graphsView->ConstructL();
    AddViewL(graphsView);           // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();            // graphsView
    
    // set the default view
    SetDefaultViewL(*valuesView);

    // notify the model that everything has been constructed
    iModel->ActivateModelL();
    }

// --------------------------------------------------------------------------------------------

CPerfMonAppUi::~CPerfMonAppUi()
    {
    // notify the model that the application is closing
    if (iModel)    
        TRAP_IGNORE(iModel->DeActivateModelL());
    
    delete iDecoratedTabGroup;
    }

// --------------------------------------------------------------------------------------------

void CPerfMonAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if (aResourceId == R_PERFMON_APP_MENU)
        {
        aMenuPane->SetItemDimmed(EPerfMonCmdEnableLogging, iModel->Settings().iLoggingEnabled);
        aMenuPane->SetItemDimmed(EPerfMonCmdDisableLogging, !iModel->Settings().iLoggingEnabled);
        }
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CPerfMonAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode /*aType*/)
    {
    if ( iTabGroup == NULL )
        {
        return EKeyWasNotConsumed;
        }

    TInt active = iTabGroup->ActiveTabIndex();
    TInt count = iTabGroup->TabCount();

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            if (active > 0)
                {
                active--;
                iTabGroup->SetActiveTabByIndex( active );
                ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
                }
            break;
        case EKeyRightArrow:
            if((active + 1) < count)
                {
                active++;
                iTabGroup->SetActiveTabByIndex( active );
                ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
                }
            break;
        default:
            return EKeyWasNotConsumed;
        }

    return EKeyWasConsumed;
    }


// --------------------------------------------------------------------------------------------

void CPerfMonAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EPerfMonCmdEnableLogging:
            {
            iModel->EnableLogging(ETrue);
            break;
            }

        case EPerfMonCmdDisableLogging:
            {
            iModel->EnableLogging(EFalse);
            break;
            }
                        
        case EPerfMonCmdSettings:
            {
            if (iModel->LaunchSettingsDialogL() == EAknCmdExit)
                Exit();
            break;
            }

        case EPerfMonCmdAbout:
            {
	        CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog;
            dialog->ExecuteLD(R_PERFMON_ABOUT_DIALOG);
            }
            break;
            
        // a normal way to close an application
        case EAknCmdExit:
        case EEikCmdExit:
        case EAknSoftkeyExit: 
			{
            Exit();
			}
            break;

        default:
            break;      
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonAppUi::HandleForegroundEventL(TBool aForeground)
    {
    // handle visibility of the data popup container
    if (iModel && iModel->DataPopupContainer())
        {
        iModel->DataPopupContainer()->UpdateVisibility(aForeground);
        }
    
    // call the base class
    CAknAppUi::HandleForegroundEventL(aForeground); 
    }

// --------------------------------------------------------------------------------------------

void CPerfMonAppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUi::HandleResourceChangeL(aType);

    // update size of the data popup container (implemented here because data popup container
    // does not get HandleResourceChangeL events)
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        if (iModel)
            {
            if (iModel->DataPopupContainer())
                {
                iModel->DataPopupContainer()->SetPositionAndSize();
                }
            
            }
        }
    }

// --------------------------------------------------------------------------------------------

// End of File  
