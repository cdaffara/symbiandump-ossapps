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

#include <hbapplication.h>
#include <hbpushbutton.h>
#include <hbmainwindow.h>
#include <qapplication.h>
#include <hbview.h>
#include <hbscrollarea.h>
#include <QGraphicsGridLayout>

#include "testpbkservices.h"

int main(int argc, char *argv[])
{
    // Initialization
    HbApplication app(argc, argv);

    // Main window widget.
    // Includes decorators such as signal strength and battery life indicator.
    HbMainWindow mainWindow;

    testPbkServices *service = new testPbkServices(&mainWindow);

    HbView* view = new HbView();
    view->setTitle("QtPhonebook Test Application");

    QGraphicsGridLayout* mainLayout = new QGraphicsGridLayout();

    HbPushButton* button = NULL;
    int row = 0;

    // FETCH =================================================================

    button = new HbPushButton("Multi Fetch");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchMultiFetch()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchMultiFetch_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Multi fetch SMS");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchMultiFetch_sms()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchMultiFetch_sms_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Multi fetch EMAIL");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchMultiFetch_email()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchMultiFetch_email_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Single fetch");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchSingleFetch()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchSingleFetch_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    // EDIT - create new =====================================================

    button = new HbPushButton("Edit create new - number");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_number()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_number_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Edit create new - number with subtype");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_numberWithSubtype()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_numberWithSubtype_old()));
    mainLayout->addItem(button, row, 1);
    row++;    
    
    button = new HbPushButton("Edit create new - email");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_email()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_email_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Edit create new - onlineAccount");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_onlineAccount()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_onlineAccount_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Edit create new - onlineAccount with subtype");
    bool res = QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_onlineAccountWithSubtype()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    res = QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditCreateNew_onlineAccountWithSubtype_old()));
    mainLayout->addItem(button, row, 1);
    row++;
    
    button = new HbPushButton("Launch editor with vCard");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditorVCard()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditorVCard_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    // EDIT - update existing ================================================

    button = new HbPushButton("Edit/update existing - number");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_number()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_number_old()));
    mainLayout->addItem(button, row, 1);
    row++;
    
    button = new HbPushButton("Edit/update existing - number with subtype");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_numberWithSubtype()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_numberWithSubtype_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Edit/update existing - email");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_email()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_email_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Edit/update existing - onlineAccount");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_onlineAccount()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_onlineAccount_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Edit/update existing - onlineAccount with subtype");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_onlineAccountWithSubtype()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchEditUpdateExisting_onlineAccountWithSubtype_old()));
    mainLayout->addItem(button, row, 1);
    row++;
    
    // CONTACT CARD ==========================================================

    button = new HbPushButton("Launch contact card");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCard()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCard_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Launch contact card with number");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCardNumber()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCardNumber_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Launch contact card with email");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCardEmail()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCardEmail_old()));
    mainLayout->addItem(button, row, 1);
    row++;

    button = new HbPushButton("Launch contact card with online account");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCardOnlineAccount()));
    mainLayout->addItem(button, row, 0);
    button = new HbPushButton("old");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchContactCardOnlineAccount_old()));
    mainLayout->addItem(button, row, 1);
    row++;
    
    // Launch - Group Member View ==========================================================
    
    button = new HbPushButton("Launch group member view");
    QObject::connect(button, SIGNAL(pressed()), service, SLOT(launchGroupMemberView()));
    mainLayout->addItem(button, row, 0);
    row++;
    
    QGraphicsWidget *graphicsWidget = new QGraphicsWidget();
    graphicsWidget->setLayout(mainLayout);
    HbScrollArea *scrollArea = new HbScrollArea();
    scrollArea->setContentWidget(graphicsWidget);
    view->setWidget(scrollArea);
    mainWindow.addView(view);
    row++;

    // Show widget
    mainWindow.show();

    // Enter event loop
    return app.exec();
}
