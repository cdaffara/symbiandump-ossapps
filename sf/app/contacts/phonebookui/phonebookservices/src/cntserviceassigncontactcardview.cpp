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

#include "cntserviceassigncontactcardview.h"
#include "cntabstractserviceprovider.h"
#include "cntdebug.h"

#include <hbaction.h>
#include <hbdialog.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbwidget.h>
#include <hbtoolbar.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <QCoreApplication>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceAssignContactCardView::CntServiceAssignContactCardView( CntAbstractServiceProvider& aServiceProvider ) : 
    CntContactCardView(true),
    mProvider( aServiceProvider )
{
    CNT_ENTRY
    connect(this, SIGNAL(backPressed(int)), this, SLOT(doCloseView(int)));
    connect(this, SIGNAL(viewActivated(CntAbstractViewManager*, const CntViewParameters)), this, SLOT(doViewActivated(CntAbstractViewManager*,const CntViewParameters)));
    connect(this, SIGNAL(addToContacts()), this, SLOT(addToContacts()));
    CNT_EXIT
}

/*!
Destructor
*/
CntServiceAssignContactCardView::~CntServiceAssignContactCardView()
{
    CNT_ENTRY
    CNT_EXIT
}

/*!
Opens the Add to Contacts popup
*/
void CntServiceAssignContactCardView::addToContacts()
{  
    CNT_ENTRY
    
    HbDialog* popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setHeadingWidget(
            new HbLabel(hbTrId("txt_phob_title_add_to_contacts"), popup));
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->setTimeout( HbPopup::NoTimeout );
    popup->addAction(
            new HbAction(hbTrId("txt_common_button_cancel"), popup));

    HbWidget* buttonWidget = new HbWidget(popup);
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    
    HbPushButton* addButton = new HbPushButton(buttonWidget);
    addButton->setStretched(true);
    addButton->setText(hbTrId("txt_phob_button_save_as_a_new_contact"));
    HbIcon plusIcon("qtg_mono_plus");
    addButton->setIcon(plusIcon);
    connect(addButton, SIGNAL(clicked()), popup, SLOT(close()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(saveNew()));
    connect(addButton, SIGNAL(longPress(QPointF)), popup, SLOT(close()));
    connect(addButton, SIGNAL(longPress(QPointF)), this, SLOT(saveNew()));
    
    HbPushButton* updateButton = new HbPushButton(buttonWidget);
    updateButton->setStretched(true);
    updateButton->setText(hbTrId("txt_phob_button_update_existing_contact"));
    updateButton->setIcon(plusIcon);
    connect(updateButton, SIGNAL(clicked()), popup, SLOT(close()));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateExisting()));
    connect(updateButton, SIGNAL(longPress(QPointF)), popup, SLOT(close()));
    connect(updateButton, SIGNAL(longPress(QPointF)), this, SLOT(updateExisting()));
    
    
    layout->addItem(addButton);
    layout->addItem(updateButton);
    
    buttonWidget->setLayout(layout);
    popup->setContentWidget(buttonWidget);

    popup->open();
    
    CNT_EXIT
}

/*!
Create a new contact with the detail
*/
void CntServiceAssignContactCardView::saveNew()
{
    CNT_ENTRY
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, serviceEditView);
    QContactName contactName = mContact.detail<QContactName>();
    mContact.removeDetail(&contactName);
    QVariant var;
    var.setValue(mContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
    CNT_EXIT
}

/*!
Update an existing contact with the detail
*/
void CntServiceAssignContactCardView::updateExisting()
{
    CNT_ENTRY
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, serviceContactSelectionView);
    QVariant var;
    var.setValue(mDetail);
    viewParameters.insert(ESelectedDetail, var);
    mViewManager->changeView(viewParameters);
    CNT_EXIT
}

/*!
Called after the view has been activated
*/
void CntServiceAssignContactCardView::doViewActivated(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    CNT_ENTRY
    mViewManager = aMgr;
    mContact = aArgs.value(ESelectedContact).value<QContact>();
    mDetail = aArgs.value(ESelectedDetail).value<QContactDetail>();
    CNT_EXIT
}

/*!
Close the view (quits the service as well)
*/
void CntServiceAssignContactCardView::doCloseView(int value)
{
    CNT_ENTRY
    QVariant variant;
    variant.setValue(value);
    mProvider.CompleteServiceAndCloseApp(variant);
    CNT_EXIT
}

// end of file
