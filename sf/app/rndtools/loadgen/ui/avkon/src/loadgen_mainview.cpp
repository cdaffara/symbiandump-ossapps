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
#include "loadgen.hrh"
#include "loadgen_mainview.h"
#include "loadgen_maincontainer.h"
#include "loadgen_document.h" 
#include "loadgen_model.h"
#include "loadgen_traces.h"
#include <loadgen.rsg>

#include <eikenv.h>
#include <aknViewAppUi.h> 
#include <aknnotewrappers.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLoadGenMainView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CLoadGenMainView::ConstructL()
    {
    BaseConstructL( R_LOADGEN_VIEW_MAIN );
    iModel = static_cast<CLoadGenDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    }

// ---------------------------------------------------------
// CLoadGenMainView::~CLoadGenMainView()
// ---------------------------------------------------------
//
CLoadGenMainView::~CLoadGenMainView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------
// TUid CLoadGenMainView::Id()
// ---------------------------------------------------------
//
TUid CLoadGenMainView::Id() const
    {
    return KMainViewUID;
    }

// ---------------------------------------------------------
// TUid CLoadGenMainView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
// ---------------------------------------------------------
//
void CLoadGenMainView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    AppUi()->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

  
// ---------------------------------------------------------
// CLoadGenMainView::HandleCommandL(TInt aCommand)
// ---------------------------------------------------------
//
void CLoadGenMainView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case ELoadGenCmdNewLoadCPULoad:
        case ELoadGenCmdNewLoadEatMemory:
        case ELoadGenCmdNewLoadPhoneCall:
        case ELoadGenCmdNewLoadNetConn:
        case ELoadGenCmdNewLoadKeyPress:
        case ELoadGenCmdNewLoadMessages:
        case ELoadGenCmdNewLoadApplications:
        case ELoadGenCmdNewLoadPhotoCaptures:
        case ELoadGenCmdNewLoadBluetooth:
        case ELoadGenCmdNewLoadPointerEvent:
            {            
            TRAPD(err, iModel->StartNewLoadL(aCommand));
            if( err != KErrNone )
                {
                LOGSTRING2("LoadGen: Load (command: %d) start failed!", aCommand);
                _LIT(message, "Load start failed!");
                CAknErrorNote* errorNote = new(ELeave) CAknErrorNote;
                errorNote->ExecuteLD(message);                
                }
            break;
            }
        case ELoadGenStopAll:
            {
            iModel->StopAllLoadItemsL();
            break;
            }
        case ELoadGenSuspendAll:
            {
            iModel->SuspendAllLoadItemsL();
            break;
            }
        case ELoadGenResumeAll:
            {
            iModel->ResumeAllLoadItemsL();
            break;
            }            
        default:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }

// ---------------------------------------------------------
// CLoadGenMainView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CLoadGenMainView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CLoadGenMainView::DoActivateL(...)
// ---------------------------------------------------------
//
void CLoadGenMainView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CLoadGenMainContainer;
        iModel->SetMainContainer(iContainer);
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        } 
   }

// ---------------------------------------------------------
// CLoadGenMainView::DoDeactivate()
// ---------------------------------------------------------
//
void CLoadGenMainView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// End of File
