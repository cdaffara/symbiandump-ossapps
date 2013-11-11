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

#include "msgcontactcardwidget.h"

// SYSTEM INCLUDES
#include <HbStyle>
#include <HbIconItem>
#include <HbTextItem>
#include <HbTapGesture>
#include <HbInstantFeedback>
#include <HbFrameItem>
#include <qcontactphonenumber.h>
#include <qcontactavatar.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <xqservicerequest.h>
#include <cntservicescontact.h>
#include <QGraphicsSceneMouseEvent>
#include <HbMenu>
#include <thumbnailmanager_qt.h>
#include <QTimer>

#include <ccsdefs.h>
#include <debugtraces.h>

// USER INCLUDES
#include "conversationsenginedefines.h"
#include "conversationsengine.h"
#include "convergedmessage.h"
#include "msgcontacthandler.h"
#include "msgsendutil.h"

// LOCAL CONSTANTS
const QString DEFAULT_AVATAR_ICON("qtg_large_avatar");
const QString BT_ICON("qtg_large_bluetooth");
const QString BG_FRAME_NORMAL("qtg_fr_groupbox_normal");
const QString BG_FRAME_PRESSED("qtg_fr_groupbox_pressed");

static const char PLUS[] = "+";

// LOCALIZATION CONSTANTS
#define LOC_RECEIVED_FILES hbTrId("txt_messaging_title_received_files")
#define LOC_MENU_CONTACT_INFO hbTrId("txt_messaging_menu_contact_info")
#define LOC_COMMON_MENU_CALL hbTrId("txt_common_menu_call_verb")
#define LOC_SAVETO_CONTACTS hbTrId("txt_messaging_menu_save_to_contacts")

// LOCAL FUNCTIONS

//---------------------------------------------------------------
// MsgContactCardWidget::MsgContactCardWidget
// @see header
//---------------------------------------------------------------
MsgContactCardWidget::MsgContactCardWidget(QGraphicsItem *parent) :
    HbWidget(parent), mAvatarIconItem(NULL), mPresenceIconItem(NULL),
        mAddressTextItem(NULL), mThumbnailManager(NULL)
{
    init();
}

//---------------------------------------------------------------
// MsgContactCardWidget::~MsgContactCardWidget
// @see header
//---------------------------------------------------------------
MsgContactCardWidget::~MsgContactCardWidget()
{
}

//---------------------------------------------------------------
// MsgContactCardWidget::init
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::init()
{
    this->setProperty("state", "normal");
    this->grabGesture(Qt::TapGesture);

    HbFrameItem *frameItem = new HbFrameItem(BG_FRAME_NORMAL, HbFrameDrawer::NinePieces, this);
    this->setBackgroundItem(frameItem);

    mAvatarIconItem = new HbIconItem(this);
    HbStyle::setItemName(mAvatarIconItem, "avatar");

    mPresenceIconItem = new HbIconItem(this);
    HbStyle::setItemName(mPresenceIconItem, "presence");

    mAddressTextItem = new HbTextItem(this);
    HbStyle::setItemName(mAddressTextItem, "addressText");

    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);

    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)), this,
        SLOT(thumbnailReady(QPixmap, void*, int, int)));
}

//---------------------------------------------------------------
// MsgContactCardWidget::setAvatar
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setAvatar(const HbIcon &avatar)
{
    mAvatarIconItem->setIcon(avatar);
}

//---------------------------------------------------------------
// MsgContactCardWidget::setPresenceIcon
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setPresenceIcon(const HbIcon &presenceIcon)
{
    mPresenceIconItem->setIcon(presenceIcon);
}

//---------------------------------------------------------------
// MsgContactCardWidget::setAddress
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setAddress(const QString &address)
{
    mAddress = address;
    mAddressTextItem->setText(address);
}

//---------------------------------------------------------------
// MsgContactCardWidget::address
// @see header
//---------------------------------------------------------------
ConvergedMessageAddressList MsgContactCardWidget::address()
{
    QCRITICAL_WRITE("MsgContactCardWidget::address start.")
    ConvergedMessageAddressList addresses;
    QStandardItemModel* msgModel = ConversationsEngine::instance()->getConversationsModel();
    
    const int rowCnt = msgModel->rowCount();
    
    QModelIndex recentIndex = msgModel->index(rowCnt-1, 0);
    QString recentAddr = recentIndex.data(ConversationAddress).toString();
    
    
    QModelIndexList indexList = msgModel->match(msgModel->index(0, 0), SendingState,
        ConvergedMessage::SentState, -1, Qt::MatchExactly);

    QString addr;
    
    bool found = false;
    if(indexList.count() > 0)
    {
        QCRITICAL_WRITE("MsgContactCardWidget::address indexList.")
        for (int i = indexList.count()-1; i >= 0; --i) 
        {
            addr = indexList[i].data(ConversationAddress).toString();
            if (addr.length() >= recentAddr.length() && addr.endsWith(recentAddr)) 
            {
              QCRITICAL_WRITE("MsgContactCardWidget::address inside for indexList.")
			  found = true; 
              break;
            }
        }
        
    }
    else 
    {   
        QCRITICAL_WRITE("MsgContactCardWidget::address inside else of indexList.")
        for(int i=rowCnt-1; i>=0;--i)
            {
            QModelIndex index = msgModel->index(i, 0);
            addr = index.data(ConversationAddress).toString();
            if(addr.startsWith(PLUS) && addr.length() >= recentAddr.length() && addr.endsWith(recentAddr))
                {
                found = true; 
                break;
                }
            }
           
    }
    
    if (!found) 
    {
        addr = recentAddr;
    }
    
    ConvergedMessageAddress* address = new ConvergedMessageAddress(addr);
    // resolve contact
    QString displayname;
    int count;
    int retval = MsgContactHandler::resolveContactDisplayName(
            addr, displayname, count);
    if(retval != -1)
    {
        address->setAlias(displayname);
    }
    addresses.append(address);
    QCRITICAL_WRITE("MsgContactCardWidget::address end.")
    return addresses;
}

//---------------------------------------------------------------
// MsgContactCardWidget::updateContents
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::updateContents()
{
    // Get the Model Index
    QModelIndex index = ConversationsEngine::instance()->getConversationsModel()->index(0, 0);

    // Index is not valid, return.
    if (!index.isValid()) {
        return;
    }

    qint64 convId = ConversationsEngine::instance()->getCurrentConversationId();

    if (KBluetoothMsgsConversationId == convId) {
        setAddress(LOC_RECEIVED_FILES);
        setAvatar(HbIcon(BT_ICON));
    }
    else {
        //Set the Contact Name/Number
        qint64 convId;
        QString displayName;
        QString contactAddress;
        convId = ConversationsEngine::instance()->getCurrentConversationId();
        ConversationsEngine::instance()->getContactDetails(convId, displayName, contactAddress);

        mContactNumber = contactAddress;
        QString contactName;
        if (displayName.isEmpty()) {
            contactName.append(contactAddress);
        }
        else {
            contactName.append(displayName);
        }

        setAddress(contactName);

        // Set Avatar
        QList<QContact> contactList = MsgContactHandler::findContactList(mContactNumber);

        if (!contactList.isEmpty()) {
            QList<QContactAvatar> avatarDetails = contactList.at(0).details<QContactAvatar> ();

            if (!avatarDetails.isEmpty()) {
                mThumbnailManager->getThumbnail(avatarDetails.at(0).imageUrl().toString());
            }
        }

        // Set default avatar till actual is set.
        setAvatar(HbIcon(DEFAULT_AVATAR_ICON));
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::clearContent
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::clearContent()
{
    setAddress("");
    setAvatar(HbIcon());
}

//---------------------------------------------------------------
// MsgContactCardWidget::isValidAddress
// @see header file
//---------------------------------------------------------------
bool MsgContactCardWidget::isValidAddress()
{
    QScopedPointer<MsgSendUtil> sendUtil(new MsgSendUtil());
    return sendUtil->isValidAddress(mContactNumber);
}

//---------------------------------------------------------------
// MsgContactCardWidget::gestureEvent
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *tapGesture = qobject_cast<HbTapGesture*> (event->gesture(Qt::TapGesture));
    if (tapGesture) {
        switch (tapGesture->state()) {
        case Qt::GestureStarted:
        {
            // Trigger haptic feedback.
            HbInstantFeedback::play(HbFeedback::Basic);
            setPressed(true);
            break;
        }
        case Qt::GestureUpdated:
        {
            if (HbTapGesture::TapAndHold == tapGesture->tapStyleHint()) {
                // Handle longtap.
                setPressed(false);
                handleLongTap(tapGesture->scenePosition());
            }
            break;
        }
        case Qt::GestureFinished:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            if (HbTapGesture::Tap == tapGesture->tapStyleHint()) {
                // Handle short tap.
                setPressed(false);
                handleShortTap(tapGesture->scenePosition());
            }
            break;
        }
        case Qt::GestureCanceled:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            setPressed(false);
            break;
        }
        }
    }
    else {
        HbWidget::gestureEvent(event);
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::setPressed
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::setPressed(bool pressed)
{
    HbFrameItem *frameItem = static_cast<HbFrameItem *> (this->backgroundItem());
    if (pressed) {
        this->setProperty("state", "pressed");
        frameItem->frameDrawer().setFrameGraphicsName(BG_FRAME_PRESSED);
    }
    else {
        this->setProperty("state", "normal");
        frameItem->frameDrawer().setFrameGraphicsName(BG_FRAME_NORMAL);
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::handleLongTap
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::handleLongTap(const QPointF &position)
{
    if (KBluetoothMsgsConversationId != ConversationsEngine::instance()->getCurrentConversationId()) {
        HbMenu* contextMenu = new HbMenu();
        contextMenu->setDismissPolicy(HbPopup::TapAnywhere);
        contextMenu->setAttribute(Qt::WA_DeleteOnClose, true);
        contextMenu->setPreferredPos(position);   

        //If contact doesn't exist in phonebook then add another menu item "Save to Contacts"
        int contactId = resolveContactId(mContactNumber);
        if (contactId < 0) {
            contextMenu->addAction(LOC_SAVETO_CONTACTS, this, SLOT(addToContacts()));
        }
        else{
            contextMenu->addAction(LOC_MENU_CONTACT_INFO, this, SLOT(openContactInfo()));
        }
        contextMenu->addAction(LOC_COMMON_MENU_CALL, this, SLOT(call()));

        contextMenu->show();
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::handleShortTap
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::handleShortTap(const QPointF &position)
{
    this->ungrabGesture(Qt::TapGesture);
    
    int contactId = resolveContactId(mContactNumber);
    if(contactId > 0)
    {
        //resolved contact open contact card.
        openContactInfo();
    }
    else
    {
        //unresolved contact show longpress options menu.
        handleLongTap(position);
    }
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));    
}

//---------------------------------------------------------------
// MsgContactCardWidget::openContactInfo
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::openContactInfo()
{
    //service stuff.
    QString service("phonebookservices");
    QString interface;
    QString operation;
    QList<QVariant> args;
    if (KBluetoothMsgsConversationId != ConversationsEngine::instance()->getCurrentConversationId()) {
        int contactId = resolveContactId(mContactNumber);
        if (contactId > 0) {
            //open existing contact card
            interface = QString("com.nokia.symbian.IContactsView");
            operation = QString("openContactCard(int)");
            args << contactId;
        }
        else {
            //populate data and open unknown contact template
            interface = QString("com.nokia.symbian.IContactsEdit");
            operation = QString("editCreateNew(QString,QString)");
            QString type = QContactPhoneNumber::DefinitionName;
            args << type;
            args << mAddress;
        }
        XQAiwRequest* request;
        XQApplicationManager appManager;
        request = appManager.create(service, interface, operation, true); // embedded
        if (request == NULL) {
            return;
        }

        // Result handlers
        connect(request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
        connect(request, SIGNAL(requestError(const QVariant&)), this,
            SLOT(handleError(const QVariant&)));

        //disbale subscritption for the CV events
        ConversationsEngine::instance()->disableRegisterationForCVEvents();

        request->setArguments(args);
        request->send();
        delete request;
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::resolveContactId
// @see header
//---------------------------------------------------------------
int MsgContactCardWidget::resolveContactId(const QString& value)
{
    QString displayLabel;

    int count;
    return MsgContactHandler::resolveContactDisplayName(value, displayLabel, count);
}

//---------------------------------------------------------------
// MsgContactCardWidget::call
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::call()
{
    //Launch dialer service     
    QString serviceName("com.nokia.symbian.ICallDial");
    QString operation("dial(QString)");

    XQServiceRequest* serviceRequest = new XQServiceRequest(serviceName, operation, false);

    connect(serviceRequest, SIGNAL(requestCompleted(QVariant)), this,
        SLOT(onServiceRequestCompleted()));

    connect(serviceRequest, SIGNAL(requestError(int)), this, SLOT(onServiceRequestCompleted()));

    *serviceRequest << mContactNumber;
    serviceRequest->send();
}

//---------------------------------------------------------------
// MsgContactCardWidget::addToContacts
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::addToContacts()
{
    openContactInfo();
}

//---------------------------------------------------------------
// MsgContactCardWidget::handleOk
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::handleOk(const QVariant& result)
{
    Q_UNUSED(result)

    QList<QContact> matchingContacts = MsgContactHandler::findContactList(mContactNumber);

    if (!matchingContacts.isEmpty()) {
        QString displayLabel = matchingContacts.at(0).displayLabel();
        if(!displayLabel.isEmpty())
        {
            setAddress(displayLabel);
        }
        else
        {
            setAddress(mContactNumber);
        }
        
        QList<QContactAvatar> avatarDetails = matchingContacts.at(0).details<QContactAvatar> ();

        if (!avatarDetails.isEmpty()) {
            mThumbnailManager->getThumbnail(avatarDetails.at(0).imageUrl().toString());
        }
        else {
            //Set default avatar since avtar is deleted        
            setAvatar(HbIcon(DEFAULT_AVATAR_ICON));
        }        
	}
    else  {
        //case for deletion from contact card widget
        //set the default number as address
        setAddress(mContactNumber);
        //Set default avatar since contact is deleted
        setAvatar(HbIcon(DEFAULT_AVATAR_ICON));
    }
	
	// Get the new conversation id.
    qint64 convId = ConversationsEngine::instance()->getConversationIdFromAddress(
                mContactNumber);
    emit conversationIdChanged(convId);
            
    ConversationsEngine::instance(
                    )->emitOpenConversationViewIdUpdate(convId);
    
}

//---------------------------------------------------------------
// MsgContactCardWidget::handleError
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)    
    
    //unblock the cv events in case of contacts save error
    ConversationsEngine::instance(
                        )->emitOpenConversationViewIdUpdate(
                                ConversationsEngine::instance(
                                        )->getCurrentConversationId(
                                                ));
}

//---------------------------------------------------------------
// MsgContactCardWidget::onServiceRequestCompleted
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::onServiceRequestCompleted()
{
    //service request is now complete. delete it.
    XQServiceRequest* request = qobject_cast<XQServiceRequest*> (sender());
    if (request) {
        delete request;
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::thumbnailReady
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data)
    Q_UNUSED(id)

    if (!error) {
        QIcon icon(pixmap);
        setAvatar(HbIcon(icon));
    }
    else {
        setAvatar(HbIcon(DEFAULT_AVATAR_ICON));
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
