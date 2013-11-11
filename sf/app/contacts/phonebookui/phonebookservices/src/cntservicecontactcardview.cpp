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

#include "cntservicecontactcardview.h"
#include "cntabstractserviceprovider.h"
#include "cntdebug.h"
#include <cntservicescontact.h>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceContactCardView::CntServiceContactCardView( CntAbstractServiceProvider& aServiceProvider ) : 
    CntContactCardView(),
    mProvider( aServiceProvider )
{
    CNT_ENTRY
    connect(this, SIGNAL(backPressed(int)), this, SLOT(closeContactCard(int)));
    CNT_EXIT
}

/*!
Destructor
*/
CntServiceContactCardView::~CntServiceContactCardView()
{
    CNT_ENTRY
    CNT_EXIT
}

/*!
Close the view (quits the service as well)
*/
void CntServiceContactCardView::closeContactCard(int value)
{
    CNT_ENTRY
    CNT_LOG_ARGS(value);
    QVariant variant;
    variant.setValue(value);
    mProvider.CompleteServiceAndCloseApp(variant);
    CNT_EXIT
}

// end of file
