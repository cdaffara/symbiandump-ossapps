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

#include "cntservicegroupmemberview.h"
#include "cntabstractserviceprovider.h"
#include "cntdebug.h"
#include <cntservicescontact.h>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceGroupMemberView::CntServiceGroupMemberView( CntAbstractServiceProvider& aServiceProvider ) : 
    CntGroupMemberView(),
    mProvider( aServiceProvider )
{
    CNT_ENTRY
    connect(this, SIGNAL(backPressed()), this, SLOT(closeGroupMemberView()));
    CNT_EXIT
}

/*!
Destructor
*/
CntServiceGroupMemberView::~CntServiceGroupMemberView()
{
    CNT_ENTRY
    CNT_EXIT
}

/*!
Close the view (quits the service as well)
*/
void CntServiceGroupMemberView::closeGroupMemberView()
{
    CNT_ENTRY
    QVariant variant;
    variant.setValue(KCntServicesTerminated);
    mProvider.CompleteServiceAndCloseApp(variant);
    CNT_EXIT
}

// end of file
