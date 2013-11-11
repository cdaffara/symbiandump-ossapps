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

#include <hbtoolbar.h>
#include <hbnotificationdialog.h>
#include <hbaction.h>

#include "HelpBaseView.h"

//////////////////////////////////////////////////////////////////////////////////////////////

HelpBaseView::HelpBaseView()
{
}

HelpBaseView::~HelpBaseView()
{
}


////////////////////////////////////////////////////////////////////////////////////////////

void HelpBaseView::initBaseDocMl()
{
 // Create widget hierarchy
    setObjectName( DOCML_VIEW_NAME );

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this );

	mBuilder.setObjectTree(roots);

	mBuilder.load(QRC_DOCML_BASE);
}

void HelpBaseView::onToolbarOnlineSupport()
{
    HbNotificationDialog *notificationDialog = new HbNotificationDialog();
    notificationDialog->setParent(this);
    notificationDialog->setTitle(URL_LINK_SUPPORT);
    notificationDialog->show();
}


// end of file
