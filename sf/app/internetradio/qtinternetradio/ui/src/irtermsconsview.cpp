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

#include <hbpushbutton.h>

#include "irtermsconsview.h"
#include "irapplication.h"
#include "iruidefines.h"
#include "irviewmanager.h"

IRTermsConsView::IRTermsConsView(IRApplication* aApplication, TIRViewId aViewId) 
    : IRBaseView(aApplication, aViewId)

{
    // This view need not to be stacked.
    setFlag(EViewFlag_UnStackable);
    initViewContents();            
}

IRTermsConsView::~IRTermsConsView()
{   
}

void IRTermsConsView::initViewContents()
{               
    setObjectName(TERMS_CONS_VIEW_BASE_OBJECT_VIEW);
    QObjectList roots;
    roots.append(this);
    iLoader.setObjectTree(roots);
    iLoader.load(TERMS_CONS_VIEW_LAYOUT_FILENAME);        
    
	// load correct orientation
    connect( getViewManager(), SIGNAL( orientationChanged(Qt::Orientation) ),
             this, SLOT( handleOrientationChanged(Qt::Orientation) ) );
	handleOrientationChanged(getViewManager()->orientation());
	        
    HbPushButton *acceptButton = qobject_cast<HbPushButton *>(iLoader.findObject(TERMS_CONS_VIEW_ACCEPT_BTN));
    HbPushButton *declineButton = qobject_cast<HbPushButton *>(iLoader.findObject(TERMS_CONS_VIEW_DECLINE_BTN));
    
    connect( acceptButton, SIGNAL(released()),
             iApplication, SLOT(handleTermsConsAccepted()) );    
    connect( declineButton, SIGNAL(released()),
             iApplication, SIGNAL(quit()) );      
}

void IRTermsConsView::handleOrientationChanged(Qt::Orientation aOrientation)
{
    if (aOrientation == Qt::Vertical)
    {
        iLoader.load(TERMS_CONS_VIEW_LAYOUT_FILENAME, PORTRAIT_SEC);
    }
    else
    {
        iLoader.load(TERMS_CONS_VIEW_LAYOUT_FILENAME, LANDSCAPE_SEC);
    }
}	
