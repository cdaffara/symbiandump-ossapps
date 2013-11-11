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

#include "cntserviceeditview.h"
#include <cntservicescontact.h>
#include <QCoreApplication>
#include "cntdebug.h"

CntServiceEditView::CntServiceEditView( CntAbstractServiceProvider& aServiceProvider )
: CntEditView(),
mProvider( aServiceProvider )
{   
    connect( this, SIGNAL(contactUpdated(int)), this, SLOT(doContactUpdated(int)) );
    connect( this, SIGNAL(contactRemoved(bool)), this, SLOT(doContactRemoved(bool)) );
    connect( this, SIGNAL(changesDiscarded()), this, SLOT(doChangesDiscarded()) );
}

CntServiceEditView::~CntServiceEditView()
{
}

void CntServiceEditView::doContactUpdated(int aSuccess)
{
    CNT_ENTRY_ARGS(aSuccess)
    
    int retValue = aSuccess ? KCntServicesReturnValueContactSaved : KCntServicesReturnValueContactNotModified;
    QVariant variant;
    variant.setValue(retValue);
    mProvider.CompleteServiceAndCloseApp(variant);
    
    CNT_EXIT
}

void CntServiceEditView::doContactRemoved(bool aSuccess)
{
    CNT_ENTRY_ARGS(aSuccess)
            
    int retValue = aSuccess ? KCntServicesReturnValueContactDeleted : KCntServicesReturnValueContactNotModified;
    QVariant variant;
    variant.setValue(retValue);
    mProvider.CompleteServiceAndCloseApp(variant);
    
    CNT_EXIT
}

void CntServiceEditView::doChangesDiscarded()
{
    CNT_ENTRY
    
    QVariant variant;
    variant.setValue(KCntServicesReturnValueContactNotModified);
    mProvider.CompleteServiceAndCloseApp(variant);
    
    CNT_EXIT
}

// EOF
