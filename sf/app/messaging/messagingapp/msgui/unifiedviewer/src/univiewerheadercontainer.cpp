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
 * Description: This widget holds the message header.
 *
 */

#include "univiewerheadercontainer.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbGroupBox>
#include <HbFrameItem>

// USER INCLUDES
#include "univieweraddresscontainer.h"
#include "univiewerdetailswidget.h"
#include "univiewerfeeder.h"
#include "univiewerattachmentwidget.h"

#include "nativemessageconsts.h"

// LOCALIZATION
#define LOC_SEND_FAILED hbTrId("txt_messaging_list_message_sending_failed")

// LOCAL CONSTANTS
const QString ADDR_LIST_SEPARATOR(", ");
const QString BG_FRAME_GRAPHICS("qtg_fr_form_heading");
const QString DIVIDER_FRAME("qtg_graf_divider_h_thin");

//---------------------------------------------------------------
// UniViewerHeaderContainer :: UniViewerHeaderContainer
// @see header file
//---------------------------------------------------------------
UniViewerHeaderContainer::UniViewerHeaderContainer(UniViewerFeeder* feeder, QGraphicsItem *parent) :
    HbWidget(parent), mViewFeeder(feeder), mViewerDetails(0), mHeaderGroupBox(0), mSeparator(0),
        mAddressContainer(0)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);

    // Address Group box
    mHeaderGroupBox = new HbGroupBox(this);
    connect(mHeaderGroupBox, SIGNAL(toggled(bool)), this, SLOT(addressBoxToggled(bool)));

    // Address container
    mAddressContainer = new UniViewerAddressContainer(this);
    connect(mAddressContainer,SIGNAL(sendMessage(const QString&,const QString&)),
            this, SIGNAL(sendMessage(const QString&,const QString&)));

    mHeaderGroupBox->setContentWidget(mAddressContainer);

    // Separator
    mSeparator = new HbFrameItem(DIVIDER_FRAME, HbFrameDrawer::OnePiece, this);
    mSeparator->setMinimumHeight(2.0);
    mSeparator->frameDrawer().fillWholeRect();
    mSeparator->hide();

    // Viewer Details widget
    mViewerDetails = new UniViewerDetailsWidget(this);

    // This widget is created to apply background for header & details.
    HbWidget *headerContainer = new HbWidget(this);

    QGraphicsLinearLayout *headerLayout = new QGraphicsLinearLayout(Qt::Vertical, headerContainer);
    headerLayout->setSpacing(0);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    HbFrameItem *bgItem = new HbFrameItem(BG_FRAME_GRAPHICS, HbFrameDrawer::NinePieces, this);
    headerContainer->setBackgroundItem(bgItem, -2.0);

    //Add address group box and insert into layout
    headerLayout->addItem(mHeaderGroupBox);
    headerLayout->addItem(mSeparator);
    headerLayout->addItem(mViewerDetails);

    headerContainer->setLayout(headerLayout);

    mMainLayout->addItem(headerContainer);

    this->setLayout(mMainLayout);
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: ~UniViewerHeaderContainer
// @see header file
//---------------------------------------------------------------
UniViewerHeaderContainer::~UniViewerHeaderContainer()
{

}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateContent
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateContent()
{
    setAddrGroupBoxHeading();

    // Address container
    populateAddressContainer();

    // Time stamp
    mViewerDetails->setTimeStamp(mViewFeeder->timeStamp(), mViewFeeder->sendingState());

    // Priority
    mViewerDetails->setPriorityIcon(mViewFeeder->priority());

    // Subject
    populateSubject();

    // Attachment list
    populateAttachments();

    // Expand address group box for outgoing messages.
    if (mViewFeeder->isIncoming()) {
        mHeaderGroupBox->setCollapsed(true);
        addressBoxToggled(true);
    }
    else {
        mHeaderGroupBox->setCollapsed(false);
        addressBoxToggled(false);
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: clearContent
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::clearContent()
{
    if (mViewerDetails) {
        mViewerDetails->clearContent();
    }

    // Clear the attachments.
    for (int i = 0; i < mUniViewerAttachmentstList.count(); ++i) {
        mMainLayout->removeItem(mUniViewerAttachmentstList[i]);
        mUniViewerAttachmentstList[i]->setParent(NULL);
        delete mUniViewerAttachmentstList[i];
        mUniViewerAttachmentstList[i] = NULL;
    }
    mUniViewerAttachmentstList.clear();

    resize(rect().width(), -1);
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateSubject
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateSubject()
{
    if (KSenduiMtmSmsUidValue == mViewFeeder->msgType()) {
        if ((ConvergedMessage::Failed == mViewFeeder->sendingState()) && !mViewFeeder->isIncoming()) {
            mViewerDetails->setSubject(LOC_SEND_FAILED);
        }
    }
    else if (KSenduiMtmMmsUidValue == mViewFeeder->msgType()) {
        QString subject;
        if (ConvergedMessage::Failed == mViewFeeder->sendingState()) {
            subject.append(LOC_SEND_FAILED);
            subject.append(QChar::LineSeparator);
        }
        subject.append(mViewFeeder->subject());
        if (!subject.isEmpty()) {
            mViewerDetails->setSubject(subject);
        }
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateAttachments
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateAttachments()
{
    UniMessageInfoList attachList = mViewFeeder->attachmentsList();
    for (int a = 0; a < attachList.count(); ++a) {
        UniMessageInfo* info = attachList.at(a);
        UniViewerAttachmentWidget *attachmentWidget = new UniViewerAttachmentWidget(this);
        mUniViewerAttachmentstList.append(attachmentWidget);
        attachmentWidget->populate(info);
        delete info;
        mMainLayout->addItem(attachmentWidget);
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateAttachments
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateAddressContainer()
{
    QString from, alias;
    mViewFeeder->fromAddressAndAlias(from, alias);
    ConvergedMessageAddressList toList = mViewFeeder->toAddressList();

    // From field is added ONLY for incoming messages.
    if (mViewFeeder->isIncoming()) {
        mAddressContainer->setFromField(from, alias);
    }
    // For outgoing SMS messages add TO field.
    else if (mViewFeeder->msgType() == KSenduiMtmSmsUidValue && !toList.isEmpty()) {
        mAddressContainer->setToField(toList);
    }

    // For MMS messages add TO, CC, BCC fields irrespective of incoming/outgoing.
    if (mViewFeeder->msgType() == KSenduiMtmMmsUidValue) {
        if (!toList.isEmpty()) {
            mAddressContainer->setToField(toList);
        }
        ConvergedMessageAddressList ccList = mViewFeeder->ccAddressList();
        if (!ccList.isEmpty()) {
            mAddressContainer->setCcField(ccList);
        }
        ConvergedMessageAddressList bccList = mViewFeeder->bccAddressList();
        if (!bccList.isEmpty()) {
            mAddressContainer->setBccField(bccList);
        }
    }

}
//---------------------------------------------------------------
// UniViewerHeaderContainer :: setAddrGroupBoxHeading
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::setAddrGroupBoxHeading()
{
    if (mViewFeeder->isIncoming()) {
        QString from, alias;
        mViewFeeder->fromAddressAndAlias(from, alias);
        (alias.isEmpty()) ? mHeaderGroupBox->setHeading(from) : mHeaderGroupBox->setHeading(alias);
    }
    else {
        ConvergedMessageAddressList addrList;
        QString headingStr;
        if ((addrList = mViewFeeder->toAddressList()).count()) {
            headingStr.append(createAddressList(addrList));
        }
        if ((addrList = mViewFeeder->ccAddressList()).count()) {
            if (!headingStr.isEmpty()) {
                headingStr.append(ADDR_LIST_SEPARATOR);
            }
            headingStr.append(createAddressList(addrList));
        }
        if ((addrList = mViewFeeder->bccAddressList()).count()) {
            if (!headingStr.isEmpty()) {
                headingStr.append(ADDR_LIST_SEPARATOR);
            }
            headingStr.append(createAddressList(addrList));
        }
        mHeaderGroupBox->setHeading(headingStr);
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: createAddressList
// @see header file
//---------------------------------------------------------------
QString UniViewerHeaderContainer::createAddressList(const ConvergedMessageAddressList &addressList)
{
    QString address;
    for (int i = 0; i < addressList.count(); ++i) {
        address.append((addressList[i]->alias().isEmpty()) ? addressList[i]->address()
            : addressList[i]->alias());
        address.append(ADDR_LIST_SEPARATOR);
    }
    address.chop(ADDR_LIST_SEPARATOR.size());
    return address;
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: addressBoxToggled
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::addressBoxToggled(bool state)
{
    (state) ? mSeparator->hide() : mSeparator->show();
}

// EOF
