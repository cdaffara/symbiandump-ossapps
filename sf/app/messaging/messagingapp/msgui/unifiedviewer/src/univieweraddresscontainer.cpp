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

#include "univieweraddresscontainer.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>

// USER INCLUDES
#include "univieweraddresswidget.h"

// LOCAL CONSTANTS
#define LOC_FROM hbTrId("txt_messaging_formlabel_from")
#define LOC_TO hbTrId("txt_messaging_viewer_formlabel_to")
#define LOC_CC hbTrId("txt_messaging_viewer_formlabel_cc")
#define LOC_BCC hbTrId("txt_messaging_viewer_formlabel_bcc")

//---------------------------------------------------------------
// UniViewerAddressContainer::UniViewerAddressContainer
// @see header file
//---------------------------------------------------------------
UniViewerAddressContainer::UniViewerAddressContainer(QGraphicsItem* parent) :
    HbWidget(parent)
{
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setSpacing(0);
    setLayout(mMainLayout);
}

//---------------------------------------------------------------
// UniViewerAddressContainer::~UniViewerAddressContainer
// @see header file
//---------------------------------------------------------------
UniViewerAddressContainer::~UniViewerAddressContainer()
{
}

//---------------------------------------------------------------
// UniViewerAddressContainer::setFromField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setFromField(const QString& fromRecipient, const QString& alias)
{
    UniViewerAddressWidget* fromWidget = new UniViewerAddressWidget(this);

    connect(fromWidget, SIGNAL(sendMessage(const QString&,const QString&)), this,
        SIGNAL(sendMessage(const QString&,const QString&)));

    mMainLayout->addItem(fromWidget);

    fromWidget->populate(LOC_FROM, fromRecipient, alias);
}

//---------------------------------------------------------------
// UniViewerAddressContainer::setToField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setToField(ConvergedMessageAddressList toRecipients)
{
    UniViewerAddressWidget* toWidget = new UniViewerAddressWidget();

    connect(toWidget, SIGNAL(sendMessage(const QString&,const QString&)), this,
        SIGNAL(sendMessage(const QString&,const QString&)));

    mMainLayout->addItem(toWidget);

    toWidget->populate(LOC_TO, toRecipients);

}

//---------------------------------------------------------------
// UniViewerAddressContainer::setCcField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setCcField(ConvergedMessageAddressList ccRecipients)
{
    UniViewerAddressWidget* ccWidget = new UniViewerAddressWidget();

    connect(ccWidget, SIGNAL(sendMessage(const QString&,const QString&)), this,
        SIGNAL(sendMessage(const QString&,const QString&)));

    mMainLayout->addItem(ccWidget);

    ccWidget->populate(LOC_CC, ccRecipients);
}

//---------------------------------------------------------------
// UniViewerAddressContainer :: setBccField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setBccField(ConvergedMessageAddressList bccRecipients)
{
    UniViewerAddressWidget* bccWidget = new UniViewerAddressWidget();

    connect(bccWidget, SIGNAL(sendMessage(const QString&,const QString&)), this,
        SIGNAL(sendMessage(const QString&,const QString&)));

    mMainLayout->addItem(bccWidget);

    bccWidget->populate(LOC_BCC, bccRecipients);
}

// EOF
