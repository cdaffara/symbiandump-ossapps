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
 * Description:Conversation (chat) view for messaging application.
 *
 */

#include "msgconversationview.h"

// SYSTEM INCLUDES
#include <hbapplication.h>
#include <HbMenu>
#include <HbAction>
#include <HbListView>
#include <HbMessageBox>
#include <HbFrameBackground>
#include <HbStaticVkbHost>
#include <HbStyleLoader>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <HbMainWindow>

#include <QInputContext>
#include <QDir>
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>

#include <cntservicescontact.h>
#include <ccsdefs.h>
#include <centralrepository.h>
#include <MmsEngineDomainCRKeys.h>

// USER INCLUDES
#include "msgcontactsutil.h"
#include "msgsendutil.h"
#include "msgconversationviewitem.h"
#include "conversationsengine.h"
#include "convergedmessageid.h"
#include "conversationsenginedefines.h"
#include "msgcontactcardwidget.h"
#include "msgeditorwidget.h"
#include "msgviewdefines.h"
#include "debugtraces.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "ringbc.h"
#include "mmsconformancecheck.h"
#include "msgsettingsview.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
#include "msgaudiofetcherdialog.h"
#include "msgservicelaunchutil.h"

// LOCALIZATION
#define LOC_TITLE hbTrId("txt_messaging_title_messaging")

//Item specific menu.
#define LOC_COMMON_OPEN hbTrId("txt_common_menu_open")
#define LOC_COMMON_DELETE hbTrId("txt_common_menu_delete")
#define LOC_COMMON_FORWARD hbTrId("txt_common_menu_forward")
#define LOC_COMMON_DOWNLOAD hbTrId("txt_messaging_menu_download")
#define LOC_COMMON_SEND  hbTrId("txt_common_button_send") 
#define LOC_COMMON_SAVE  hbTrId("txt_common_menu_save")

#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_DELETE_SHARED_MESSAGE hbTrId("txt_messaging_dialog_same_message_exists_in_multip")
#define LOC_SAVE_TO_CONTACTS hbTrId("txt_messaging_menu_save_to_contacts")

//main menu
#define LOC_ATTACH          hbTrId("txt_messaging_opt_attach")
#define LOC_PHOTO           hbTrId("txt_messaging_opt_attach_sub_photo")
#define LOC_SOUND           hbTrId("txt_messaging_opt_attach_sub_sound")
#define LOC_VCARD           hbTrId("txt_messaging_list_business_card")
#define LOC_ADD_RECIPIENTS  hbTrId("txt_messaging_opt_add_recipients")
#define LOC_ADD_SUBJECT     hbTrId("txt_messaging_opt_add_subject")

#define LOC_MSG_SEND_FAILED hbTrId("txt_messaging_dialog_message_sending_failed")
#define LOC_DIALOG_SMS_SETTINGS_INCOMPLETE hbTrId("txt_messaging_dialog_sms_message_centre_does_not_e")
#define LOC_DIALOG_SAVE_RINGTONE hbTrId("txt_conversations_dialog_save_ringing_tone")
#define LOC_MMS_RETRIEVAL_FAILED hbTrId("txt_messaging_dialog_mms_retrieval_failed")


const int INVALID_MSGID = -1;
const int INVALID_CONVID = -1;
const int CONTACT_INSERTION_MODE = 1;
const int VCARD_INSERTION_MODE = 0;

//---------------------------------------------------------------
// MsgConversationView::MsgConversationView
// @see header file
//---------------------------------------------------------------
MsgConversationView::MsgConversationView(MsgContactCardWidget *contactCardWidget,
    QGraphicsItem *parent) :
    MsgBaseView(parent),
    mConversationList(NULL),
    mMessageModel(NULL),
    mEditorWidget(NULL),
    mContactCardWidget(contactCardWidget),
    mSendUtil(NULL),
    mVkbHost(NULL),
    mDialog(NULL),
    mVisibleIndex(),
    mModelPopulated(false),
    mViewReady(false)
{
    //create send utils
    mSendUtil = new MsgSendUtil(this);
    //initialize view
    setupView();
    setupMenu();
    
    connect(ConversationsEngine::instance(), 
                     SIGNAL(conversationModelUpdated()),
                     this, 
                     SLOT(scrollToBottom()));

    connect(ConversationsEngine::instance(), 
                         SIGNAL(conversationViewEmpty()),
                         this, 
                         SLOT(onConversationViewEmpty()));
}

//---------------------------------------------------------------
// MsgConversationView::~MsgConversationView
// @see header file
//---------------------------------------------------------------
MsgConversationView::~MsgConversationView()
{
    //delete the popup dialog
    if (mDialog) {
        delete mDialog;
    }
}
//---------------------------------------------------------------
// MsgConversationView::setupView
// @see header file
//---------------------------------------------------------------
void MsgConversationView::setupView()
{
    // Create HbListView and set properties
    mConversationList = new HbListView();
    if (!HbStyleLoader::registerFilePath(":/layouts")) {
        QDEBUG_WRITE("ERROR: ConversationView -> HbStyleLoader::registerFilePath");
    }
    mConversationList->setLayoutName("custom");
    mConversationList->setItemRecycling(true);
	mConversationList->setItemPixmapCacheEnabled(true);
    MsgConversationViewItem *item = new MsgConversationViewItem(this);
    HbFrameBackground defaultBackground;
    defaultBackground.setFrameGraphicsName(QString(""));
    item->setDefaultFrame(defaultBackground);
    mConversationList->setItemPrototype(item);
    mConversationList->setSelectionMode(HbListView::NoSelection);
    mConversationList->setClampingStyle(HbScrollArea::BounceBackClamping);

    mMessageModel = ConversationsEngine::instance()->getConversationsModel();
    
    connect(ConversationsEngine::instance(), 
            SIGNAL(conversationModelPopulated()), 
            this, 
            SLOT(populateConversationsView()));
    
    connect(mConversationList, SIGNAL(activated(QModelIndex)),
            this, SLOT(openItem(QModelIndex)));
    connect(this->mainWindow(), SIGNAL(aboutToChangeOrientation()),
                this, SLOT(onOrientationAboutToBeChanged()));
    
    connect(this->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                    this, SLOT(onOrientationChanged(Qt::Orientation)));

    // Long tap list item
    connect(mConversationList, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)),
            this, SLOT(longPressed(HbAbstractViewItem*, QPointF)));

    // Create message editor widget, will be displayed based on msg type.
    mEditorWidget = new MsgEditorWidget(this);
    mEditorWidget->hide();

    connect(mEditorWidget, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mEditorWidget, SIGNAL(smsCharLimitReached()), 
        this, SLOT(handleSmsCharLimitReached()));
    connect(mEditorWidget, SIGNAL(replyStarted()), this, SIGNAL(replyStarted()));

    qreal spacing = HbDeviceProfile::profile(mConversationList).unitValue();
    
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical,this); 
    
    mMainLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                    CONTENT_MARGIN, CONTENT_MARGIN);   
    mMainLayout->setSpacing(spacing);

    mMainLayout->addItem(mConversationList);

    setLayout(mMainLayout);

    //Create VKB instance and listen to VKB open and close signals for resizing the view.
    mVkbHost = new HbStaticVkbHost(this);
	connect(mVkbHost, SIGNAL(keypadOpened()), this, SLOT(vkbOpened()));
	connect(mVkbHost, SIGNAL(keypadClosed()), this, SLOT(vkbClosed()));

    // Refresh view to show the header details
    refreshView();
}

//---------------------------------------------------------------
// MsgConversationView::addMenu
// @see header file
//---------------------------------------------------------------
void MsgConversationView::setupMenu()
{
    // Just create dummy menu.
    // Actual menu will be created in menuAboutToShow()
    HbMenu *mainMenu = this->menu();
    mainMenu->addAction(QString());
    connect(mainMenu, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()));
}

//---------------------------------------------------------------
// MsgConversationView::fetchMoreConversations
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchMoreConversations()
{
    if (mViewReady && mModelPopulated) {
        ConversationsEngine::instance()->fetchMoreConversations();
        mViewReady = mModelPopulated = false;
    }
}

//---------------------------------------------------------------
// MsgConversationView::refreshView()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::refreshView()
{
    // Hide editor in case of BT conversations.
    qint64 convId = ConversationsEngine::instance()->getCurrentConversationId();
    if (INVALID_CONVID != convId) {
        mContactCardWidget->updateContents();
        if (KBluetoothMsgsConversationId == convId || !(mContactCardWidget->isValidAddress())) {
            this->menu()->clearActions();
            mMainLayout->removeItem(mEditorWidget);
            mEditorWidget->hide();
            mEditorWidget->setParent(this);
        }
        else {
            HbMenu *mainMenu = this->menu();
            if(mainMenu->isEmpty()) {
               mainMenu->addAction(QString());
            }
            mMainLayout->addItem(mEditorWidget);
            mEditorWidget->show();
        }
        TRAP_IGNORE(mEditorWidget->setEncodingSettingsL());
    }
}

//---------------------------------------------------------------
// MsgConversationView::scrollToBottom()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::scrollToBottom()
{
    const int rowCnt = mMessageModel->rowCount();
    mConversationList->scrollTo(
            mMessageModel->index(rowCnt - 1, 0));
}

void MsgConversationView::onConversationViewEmpty()
{
    QVariantList param;
    param << MsgBaseView::CLV; // target view
    param << MsgBaseView::CV; // source view
    emit switchView(param);
}

//---------------------------------------------------------------
// MsgConversationView::longPressed
// @see header file
//---------------------------------------------------------------
void MsgConversationView::longPressed(HbAbstractViewItem* viewItem, const QPointF& point)
{ 
    showContextMenu(viewItem,point,HbPopup::TopLeftCorner);
}

//---------------------------------------------------------------
// MsgConversationView::setContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::setContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    addOpenItemToContextMenu(item , contextMenu,sendingState);
    addResendItemToContextMenu(item, contextMenu, sendingState);
    addForwardItemToContextMenu(item, contextMenu, sendingState);
    addDownloadItemToContextMenu(item, contextMenu);
    addSaveItemToContextMenu(item , contextMenu,sendingState);
    addDeleteItemToContextMenu(item, contextMenu, sendingState);
}


//---------------------------------------------------------------
// MsgEditorPrivate::addSaveItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addSaveItemToContextMenu(MsgConversationViewItem* item,
    HbMenu* contextMenu, int sendingState)
{
    Q_UNUSED(sendingState)

    int messageSubType = item->modelIndex().data(MessageSubType).toInt();
    int direction =  item->modelIndex().data(Direction).toInt();
    if ((messageSubType == ConvergedMessage::RingingTone) && 
        (direction == ConvergedMessage::Incoming)) {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_SAVE);
        connect(contextItem, SIGNAL(triggered()), this, SLOT(saveRingingTone()));
    }
}

//---------------------------------------------------------------
// MsgEditorPrivate::addOpenItemToContextMenu
// @see header
//---------------------------------------------------------------

void MsgConversationView::addOpenItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    int direction = item->modelIndex().data(Direction).toInt();
    int messageType = item->modelIndex().data(MessageType).toInt();
    int messageSubType = item->modelIndex().data(MessageSubType).toInt();
    
    if((messageSubType == ConvergedMessage::VCal) ||
        (messageSubType == ConvergedMessage::RingingTone) ||
        (messageType == ConvergedMessage::MmsNotification))
        {
        return;
        }
    if ((messageSubType == ConvergedMessage::VCard) &&
        (direction == ConvergedMessage::Incoming))
        {
        HbAction *contextItem = contextMenu->addAction(LOC_SAVE_TO_CONTACTS);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(saveVCard()));
        return;
        }
    if( (sendingState == ConvergedMessage::SentState &&
         messageSubType != ConvergedMessage::VCard) ||
        (direction == ConvergedMessage::Incoming))
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_OPEN);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(openItem()));
    }
    
}

//---------------------------------------------------------------
// MsgEditorPrivate::addResendItemToContextMenu
// @see header
//---------------------------------------------------------------

void MsgConversationView::addResendItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    Q_UNUSED(item)
    int direction = item->modelIndex().data(Direction).toInt();
    int messageSubType = item->modelIndex().data(MessageSubType).toInt();
    
    
    if( ((direction == ConvergedMessage::Outgoing) &&
        (messageSubType != ConvergedMessage::VCard))&&
        ((sendingState == ConvergedMessage::Resend ) ||
        
        (sendingState == ConvergedMessage::Failed )))
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_SEND);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(resendMessage()));
    }
}

//---------------------------------------------------------------
// MsgEditorPrivate::addForwardItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addForwardItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    int messageType = item->modelIndex().data(MessageType).toInt();
    int direction = item->modelIndex().data(Direction).toInt();
    int messageSubType = item->modelIndex().data(MessageSubType).toInt();
    
    if( (messageType == ConvergedMessage::BT) ||
        (messageType == ConvergedMessage::MmsNotification) ||
        (messageSubType == ConvergedMessage::Provisioning ) ||
        (messageSubType == ConvergedMessage::RingingTone) ||
        (messageSubType == ConvergedMessage::VCal))
    {
        return;
    }
    
    qint32 messageId = item->modelIndex().data(ConvergedMsgId).toInt();
    qint32 messageProperty = item->modelIndex().data(MessageProperty).toInt();
    
    bool canForwardMessage  = true;
    if (messageType == ConvergedMessage::Mms){
        canForwardMessage = (messageProperty & EPreviewForward)? true:false;
    }            

    if( ((sendingState == ConvergedMessage::SentState) ||
        (sendingState == ConvergedMessage::Resend) ||
        (sendingState == ConvergedMessage::Failed) ||
        (sendingState == ConvergedMessage::Suspended )||
        (direction == ConvergedMessage::Incoming) ) &&
        canForwardMessage)
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_FORWARD);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(forwardMessage()));
    }

}

//---------------------------------------------------------------
// MsgEditorPrivate::addDeleteItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addDeleteItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    int direction = item->modelIndex().data(Direction).toInt();
    int messageType = item->modelIndex().data(MessageType).toInt();
    int notificationState = item->modelIndex().data(NotificationStatus).toInt();
    
    if( (messageType == ConvergedMessage::MmsNotification) && 
        ((notificationState == ConvergedMessage::NotifNull) || 
         (notificationState ==  ConvergedMessage::NotifRetrieving) ||
         (notificationState == ConvergedMessage::NotifWaiting)))
    {
        return;
    }
    
    if( (sendingState == ConvergedMessage::SentState) ||
        (sendingState == ConvergedMessage::Resend) ||
        (sendingState == ConvergedMessage::Suspended) ||
        (sendingState == ConvergedMessage::Failed) ||
        (direction == ConvergedMessage::Incoming))
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_DELETE);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(deleteItem()));
    }  
}

//---------------------------------------------------------------
// MsgEditorPrivate::addDownloadItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addDownloadItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu)
{
    int notificationState = item->modelIndex().data(NotificationStatus).toInt();
    int messageType = item->modelIndex().data(MessageType).toInt();
    qint32 messageId = item->modelIndex().data(ConvergedMsgId).toLongLong();
        
    if( messageType == ConvergedMessage::MmsNotification &&
        ConversationsEngine::instance()->downloadOperationSupported(messageId))
    {            
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_DOWNLOAD);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(downloadMessage()));
    }
}
//---------------------------------------------------------------
// MsgEditorPrivate::send
// @see header
//---------------------------------------------------------------
void MsgConversationView::send()
{
    activateInputBlocker();
    ConvergedMessageAddressList addresses;
    addresses = mContactCardWidget->address();
    int sendResult = KErrNone;

    // Populate the ConvergedMessage.
    if (!addresses.isEmpty())
    {
        ConvergedMessage msg;
        populateForSending(msg);
        msg.addToRecipients(addresses);

        // Send
        sendResult = mSendUtil->send(msg);
                   
        if( sendResult == KErrNone)
        {
            mEditorWidget->clear();
        }
    }
    deactivateInputBlocker();

    // make sure virtual keyboard is closed
    QInputContext *ic = qApp->inputContext();
    if (ic) {
    QEvent *closeEvent = new QEvent(QEvent::CloseSoftwareInputPanel);
    ic->filterEvent(closeEvent);
    delete closeEvent;
    }

    if( sendResult == KErrNotFound)
    {
    HbMessageBox::question(LOC_DIALOG_SMS_SETTINGS_INCOMPLETE, this,
                           SLOT(onDialogSettingsLaunch(HbAction*)), 
                           HbMessageBox::Ok | HbMessageBox::Cancel);
    }
}

//---------------------------------------------------------------
// MsgConversationView::contactsFetchedForVCards
// @see header file
//---------------------------------------------------------------
void MsgConversationView::contactsFetchedForVCards(const QVariant& value)
{
    // get received contact-list and launch unieditor
    CntServicesContactList contactList = 
            qVariantValue<CntServicesContactList>(value);
    int cntCount = contactList.count();
    if(cntCount > 0)
    {
        QVariantList params;
        params << MsgBaseView::ADD_VCARD;
        params << value;
        launchUniEditor(params);        
    }
}

//---------------------------------------------------------------
// MsgConversationView::fetchContacts
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchContacts()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(!action)
        return;

    QList<QVariant> args;
    args << LOC_TITLE;

    QString service("phonebookservices");
    QString interface("com.nokia.symbian.IContactsFetch");
    QString operation("multiFetch(QString,QString)");
    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(service, interface, operation, true); //embedded
    if ( request == NULL )
    {
        return;       
    }

    int mode = action->data().toInt();
    
    if( VCARD_INSERTION_MODE == mode) //vcard-insert mode
    {
        args << KCntActionAll;
        connect(request, SIGNAL(requestOk(const QVariant&)),
            this, SLOT(contactsFetchedForVCards(const QVariant&)));      
    }
    else  //contact-insert mode
    {
        args << KCntActionSms;
        connect(request, SIGNAL(requestOk(const QVariant&)),
            this, SLOT(contactsFetched(const QVariant&)));
    }
    connect (request, SIGNAL(requestError(int,const QString&)), 
        this, SLOT(serviceRequestError(int,const QString&)));

    args << KCntFilterDisplayAll;
    request->setArguments(args);
    request->send();
    delete request;
}
//---------------------------------------------------------------
// MsgConversationView::fetchImages
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchImages()
{
    QString service("photos");
    QString interface("com.nokia.symbian.IImageFetch");
    QString operation("fetch()");
    XQAiwRequest* request = NULL;
    XQApplicationManager appManager;
    request = appManager.create(service,interface, operation, true); // embedded

    if(!request)
    {
        QDEBUG_WRITE("AIW-ERROR: NULL request");
        return;
    }

    request->setSynchronous(true); // synchronous
    
    connect(request, SIGNAL(requestOk(const QVariant&)),
        this, SLOT(imagesFetched(const QVariant&)));
    connect(request, SIGNAL(requestError(int,const QString&)),
        this, SLOT(serviceRequestError(int,const QString&)));

    // Make the request
    if (!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed:" , request->lastError());
    }  
    delete request;
}

//---------------------------------------------------------------
// MsgConversationView::fetchAudio
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchAudio()
{
    // Launch Audio fetcher dialog
    if (!mDialog)
    {
        mDialog = new MsgAudioFetcherDialog();
        bool b = connect(mDialog,
                SIGNAL(audioSelected(QString&)),
                this,
                SLOT(onAudioSelected(QString&)));
    }

    //show the dialog
    mDialog->show();    
}

//---------------------------------------------------------------
// MsgConversationView::contactsFetched
// @see header file
//---------------------------------------------------------------
void MsgConversationView::contactsFetched(const QVariant& value)
{
    //switch to editor.
    QVariantList params;
    params << MsgBaseView::ADD_RECIPIENTS;
    params << value;
    launchUniEditor(params);
}

//---------------------------------------------------------------
// MsgConversationView::imagesFetched()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::imagesFetched(const QVariant& result )
{
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 )
        {
            QString filepath(QDir::toNativeSeparators(fileList.at(0)));
            QVariantList params;
            params << MsgBaseView::ADD_PHOTO;
            params << filepath;
            launchUniEditor(params);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationView::addSubject()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::addSubject()
{

    QString filepath;
    QVariantList params;
    params << MsgBaseView::ADD_SUBJECT;
    launchUniEditor(params);
}

//---------------------------------------------------------------
// MsgConversationView::forwardMessage()
// Forwards the message
//---------------------------------------------------------------
void MsgConversationView::forwardMessage()
{
    QModelIndex index = mConversationList->currentIndex();
    //messageId & messageType to be forwarded
    qint32 messageId = index.data(ConvergedMsgId).toLongLong();
    int messageType = index.data(MessageType).toInt();
 
    //Mark the message to read before forwarding.
    if(index.data(UnReadStatus).toBool())
    {
        QList<int> msgIdList;
        msgIdList.append(messageId);
        ConversationsEngine::instance()->markMessagesRead(msgIdList);
    }
    // populate params and launch editor 
    QVariantList params;
    params << MsgBaseView::FORWARD_MSG;
    params << messageId;
    params << messageType;
    launchUniEditor(params);
}

//---------------------------------------------------------------
// MsgConversationView::resendMessage()
// Resends the message in the failed messages case
//---------------------------------------------------------------
void MsgConversationView::resendMessage()
{
    QModelIndex index = mConversationList->currentIndex();
    if(index.isValid())
    {
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();    
        if(!(ConversationsEngine::instance()->resendMessage(messageId)))
        {
            HbMessageBox::warning(LOC_MSG_SEND_FAILED, 0, 0, HbMessageBox::Ok);
        }
    }
    
}

//---------------------------------------------------------------
// MsgConversationView::downloadMessage()
// @see header
//---------------------------------------------------------------
void MsgConversationView::downloadMessage()
{
    QModelIndex index = mConversationList->currentIndex();
    if(index.isValid())
    {
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();
        if(ConversationsEngine::instance()->downloadMessage(messageId)!=KErrNone)
        {
            HbMessageBox::warning(LOC_MMS_RETRIEVAL_FAILED, 0, 0, HbMessageBox::Ok);
        }
    }
    
}

//---------------------------------------------------------------
// MsgConversationView::deleteItem()
// Deletes the message
//---------------------------------------------------------------
void MsgConversationView::deleteItem()
{
    QString str = LOC_DELETE_MESSAGE;

    QModelIndex index = mConversationList->currentIndex();
    if(index.isValid())
    {
        int messageType = index.data(MessageType).toInt();        
        int direction = index.data(Direction).toInt();

        if ( direction == ConvergedMessage::Outgoing &&
            messageType == ConvergedMessage::Mms )
        {
            qint32 messageId = index.data(ConvergedMsgId).toLongLong();

            if(isSharedMessage(messageId))
            {
                str = LOC_DELETE_SHARED_MESSAGE;  
            }
        }
    }


    HbMessageBox::question(str,this,SLOT(onDialogdeleteMsg(HbAction*)),
                           HbMessageBox::Delete | HbMessageBox::Cancel);
}

//---------------------------------------------------------------
// MsgConversationView::openItem()
// Opens the message
//---------------------------------------------------------------
void MsgConversationView::openItem()
{
    QModelIndex index = mConversationList->currentIndex();
    openItem(index);
}

//---------------------------------------------------------------
// MsgConversationView::clearEditors()
// @See header
//---------------------------------------------------------------
void MsgConversationView::clearEditors()
{
    mEditorWidget->clear();
    mConversationList->setModel(NULL);
    mContactCardWidget->clearContent();
}

//---------------------------------------------------------------
// MsgConversationView::saveContentToDrafts()
// @See header
//---------------------------------------------------------------
int MsgConversationView::saveContentToDrafts()
{
    int msgId = INVALID_MSGID;
    if(!mEditorWidget->content().isEmpty())
    {
        activateInputBlocker();
        ConvergedMessageAddressList addresses;
        addresses = mContactCardWidget->address();

        // Populate the ConvergedMessage.
        if (!addresses.isEmpty())
        {
            ConvergedMessage msg;
            populateForSending(msg);
            msg.addToRecipients(addresses);
            
            // save to drafts
            msgId = mSendUtil->saveToDrafts(msg);
                     
            mEditorWidget->clear();
        }
        deactivateInputBlocker();
    }
    return msgId;
}

//---------------------------------------------------------------
//MsgConversationView::populateForSending()
//@see header
//---------------------------------------------------------------
void MsgConversationView::populateForSending(ConvergedMessage &message)
{
    message.setMessageType(ConvergedMessage::Sms);
    message.setBodyText(mEditorWidget->content());
    message.setDirection(ConvergedMessage::Outgoing);
    QDateTime time = QDateTime::currentDateTime();
    message.setTimeStamp(time.toTime_t());
}

//---------------------------------------------------------------
//MsgConversationView::launchBtDisplayService()
//@see header
//---------------------------------------------------------------
void MsgConversationView::launchBtDisplayService(const QModelIndex & index)
{
    qint32 messageId = index.data(ConvergedMsgId).toLongLong();

    // launch using msgservicelaunchutils
    
    MsgServiceLaunchUtil serviceLaunchUtil;
    
    serviceLaunchUtil.launchContentViewer(messageId);
   
}

//---------------------------------------------------------------
// MsgConversationView::menuAboutToShow()
// @See header
//---------------------------------------------------------------
void MsgConversationView::menuAboutToShow()
{
    // Clear all the previously added actions.
    HbMenu *mainMenu = this->menu();
    mainMenu->clearActions();

    // Attach sub-menu
    HbMenu *attachSubMenu = mainMenu->addMenu(LOC_ATTACH);
    
    attachSubMenu->addAction(LOC_PHOTO,this, SLOT(fetchImages()));
    attachSubMenu->addAction(LOC_SOUND,this, SLOT(fetchAudio()));
    
    HbAction* addVCard = attachSubMenu->addAction(LOC_VCARD);
    addVCard->setData(VCARD_INSERTION_MODE);        
    connect(addVCard, SIGNAL(triggered()),this,SLOT(fetchContacts()));

    HbAction *addRecipients = mainMenu->addAction(LOC_ADD_RECIPIENTS);
    addRecipients->setData(CONTACT_INSERTION_MODE);        
    connect(addRecipients, SIGNAL(triggered()), this, SLOT(fetchContacts()));

    mainMenu->addAction(LOC_ADD_SUBJECT,this, SLOT(addSubject()));

}

//---------------------------------------------------------------
//MsgConversationView::openItem
//@see header
//---------------------------------------------------------------
void MsgConversationView::openItem(const QModelIndex & index)
{
    // Return if invalid index.
    if (!index.isValid())
    {
        return;
    }
    
    int messageType = index.data(MessageType).toInt();
    int messageSubType = index.data(MessageSubType).toInt();
    int messageId = index.data(ConvergedMsgId).toInt();
    
    if (ConvergedMessage::BioMsg == messageType)
    {
        if (ConvergedMessage::RingingTone == messageSubType)
        {
            HbMessageBox::question(LOC_DIALOG_SAVE_RINGTONE, this,
                                   SLOT(onDialogSaveTone(HbAction*)), 
                                   HbMessageBox::Save | HbMessageBox::Cancel);
            return;
        }
        else if(ConvergedMessage::Provisioning == messageSubType)
        {
            int messageId = index.data(ConvergedMsgId).toInt();
            handleProvisoningMsg(messageId);
            QList<int> msgIdList;
            if(index.data(UnReadStatus).toInt())
            {
                msgIdList.clear();
                msgIdList << messageId;
                ConversationsEngine::instance()->markMessagesRead(msgIdList);
            }
            return;
        }
        else if(ConvergedMessage::VCard == messageSubType)
        {
            handleShortTap();
            return;
        }
        else if(ConvergedMessage::VCal == messageSubType)
        {
            return;
        }
    }
    else if (ConvergedMessage::BT == messageType)
    {
        launchBtDisplayService(index);
        QList<int> msgIdList;
        if(index.data(UnReadStatus).toInt())
            {
            msgIdList.clear();
            msgIdList << messageId;
            ConversationsEngine::instance()->markMessagesRead(msgIdList);
            }
        return;
    }
    else if(ConvergedMessage::MmsNotification == messageType)
    {
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();
        if(!ConversationsEngine::instance()->downloadOperationSupported(messageId))           
        {
           int notificationState = index.data(NotificationStatus).toInt();
           if( notificationState == ConvergedMessage::NotifExpired)
           {
               deleteItem();
           }
           return;
        }
        else
        {
            //TODO: use logical str name
            HbMessageBox::question("Download Message?",this,
                                   SLOT(onDialogDownLoadMsg(HbAction*)),
                                   HbMessageBox::Yes | HbMessageBox::Cancel);
            return;
        }
    }

    int direction = index.data(Direction).toInt();

    if (direction == ConvergedMessage::Outgoing && ConvergedMessage::Sms == messageType
        && ConversationsEngine::instance()->getMsgSubType(messageId)== ConvergedMessage::NokiaService)
    {
        return;
    }
    
    // check whether message is in sending progress, then donot launch viewer.
    int location = index.data(MessageLocation).toInt();
    int sendingState = index.data(SendingState).toInt();
    
    // For suspended message both short tap and long tap needs to show the same
    // context menu.....
    if(direction == ConvergedMessage::Outgoing 
        	&& (sendingState == ConvergedMessage::Suspended))
    {
        handleShortTap();
        return;
    }
    
    //If message is in any state other than 'Sent' or 'Failed'
    //do not open the message
    if(direction == ConvergedMessage::Outgoing 
            && sendingState != ConvergedMessage::SentState 
            && sendingState != ConvergedMessage::Failed)
           
    {
        return;
    }

    // contact Id
    qint32 contactId = index.data(ContactId).toLongLong();  
	    
    //if message unread, mark as read before opening view
    QList<int> msgIdList;
    if(index.data(UnReadStatus).toInt())
    {
        msgIdList.clear();
        msgIdList << messageId;
        ConversationsEngine::instance()->markMessagesRead(msgIdList);
    }

    //switch view
    QVariantList param;
    param << MsgBaseView::UNIVIEWER;  // target view
    param << MsgBaseView::CV; // source view

    param << contactId;
    param << messageId;
    param << mMessageModel->rowCount();
    emit switchView(param);
}

//---------------------------------------------------------------
// MsgConversationView::launchUniEditor
// @see header file
//---------------------------------------------------------------
void MsgConversationView::launchUniEditor(const QVariantList& data)
{
    // param list for switching to editor view
    QVariantList params;
    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);

    // first arg is always the editor operation
    int editorOperation = data.at(0).toInt();
    
    ConvergedMessage message;
    QVariant data2;
    if( editorOperation != MsgBaseView::FORWARD_MSG )
    {
        message.setBodyText(mEditorWidget->content());

        // add address from contact-card to to-field
        ConvergedMessageAddressList addresses;
        addresses = mContactCardWidget->address();
        
        ConvergedMessageAddress address;
        address.setAlias(addresses.at(0)->alias());
        address.setAddress(addresses.at(0)->address());
        message.addToRecipient(address);

        if(editorOperation == MsgBaseView::ADD_PHOTO ||
           editorOperation == MsgBaseView::ADD_AUDIO ||
           editorOperation == MsgBaseView::ADD_VIDEO )
        {
            // filepath is sent in cases like ADD_PHOTO, ADD_AUDIO etc.
            QString filepath;
            filepath = data.at(1).toString();
            if(!filepath.isEmpty())
            {
                ConvergedMessageAttachment* attachment = 
                        new ConvergedMessageAttachment(filepath);
                ConvergedMessageAttachmentList attachmentList;
                attachmentList.append(attachment);
                message.addAttachments(attachmentList);
            }            
        }
        else if(editorOperation == MsgBaseView::ADD_VCARD)
        {
            // filepath is not sent in cases like VCards & recipients
            // instead, we will get a list of contacts. Pass it as it is.
            data2 = data.at(1);
        }
        else if(editorOperation == MsgBaseView::ADD_RECIPIENTS)
        {
            ConvergedMessageAddressList addresses;
            CntServicesContactList contactList = 
                    qVariantValue<CntServicesContactList>(data.at(1));
            // now add fetched contacts from contact selection dialog
            for(int i = 0; i < contactList.count(); i++ )
            {
                ConvergedMessageAddress* address = new ConvergedMessageAddress;
                address->setAlias(contactList[i].mDisplayName);
                if(!contactList[i].mPhoneNumber.isEmpty())
                {
                    address->setAddress(contactList[i].mPhoneNumber);
                }
                else
                {
                    address->setAddress(contactList[i].mEmailAddress);
                }
                addresses.append(address);
            }
            message.addToRecipients(addresses);
        }
    }
    else
    {
        qint32 msgId = (qint32)data.at(1).toInt();
        int msgType = data.at(2).toInt();
        ConvergedMessageId id(msgId);
        message.setMessageId(id);
        message.setMessageType((ConvergedMessage::MessageType)msgType);
    }

    message.serialize(messageStream);
    params << MsgBaseView::UNIEDITOR;
    params << MsgBaseView::CV;
    params << dataArray;
    params << editorOperation;
    if(!data2.isNull())
        params << data2;

    emit switchView(params);
}

//---------------------------------------------------------------
// MsgConversationView::populateConversationsView
// @see header file
//---------------------------------------------------------------
void MsgConversationView::populateConversationsView()
{    
    mModelPopulated = true;
    mConversationList->setModel(mMessageModel);
    
    refreshView();
    scrollToBottom();
    fetchMoreConversations();
}

//---------------------------------------------------------------
// MsgConversationView::saveRingingTone
// @see header file
//---------------------------------------------------------------
void MsgConversationView::saveRingingTone()
{
    QModelIndex index = mConversationList->currentIndex();
    int messageId = index.data(ConvergedMsgId).toInt();

    UniDataModelLoader* pluginLoader = new UniDataModelLoader();
    UniDataModelPluginInterface* pluginInterface = 
    pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
    pluginInterface->setMessageId(messageId);
    UniMessageInfoList attachments = pluginInterface->attachmentList();
    if(attachments.count() > 0)
    {
        QString attachmentPath = attachments[0]->path();

        RingBc* ringBc = new RingBc();

        ringBc->saveTone(attachmentPath);

        // clear attachement list : its allocated at data model
        while(!attachments.isEmpty())
        {
            delete attachments.takeFirst();
        }

        delete ringBc;
    }
    delete pluginLoader;
}

//---------------------------------------------------------------
// MsgConversationView::handleSmsCharLimitReached
// @see header file
//---------------------------------------------------------------
void MsgConversationView::handleSmsCharLimitReached()   
{
    QString filepath;
    QVariantList params;
    params << MsgBaseView::ADD_OTHERS;
    launchUniEditor(params);   
}

//---------------------------------------------------------------
// MsgConversationView::vkbOpened
// @see header file
//---------------------------------------------------------------
void MsgConversationView::vkbOpened()
{
    emit vkbOpened(true);
    
    scrollToBottom();
    
    QRectF appRect = mVkbHost->applicationArea();    
    qreal spacing = 0.0;
    qreal cardHeight = 0.0;
    if(mContactCardWidget->isVisible())
        {
        cardHeight = mContactCardWidget->rect().height();
        spacing = HbDeviceProfile::profile(this).unitValue();
        }
    
    this->setMaximumHeight(appRect.height()- cardHeight - spacing);

    
    disconnect(mVkbHost,SIGNAL(keypadOpened()),this,SLOT(vkbOpened()));

}
	  
//---------------------------------------------------------------
// MsgConversationView::vkbClosed
// @see header file
//---------------------------------------------------------------
void MsgConversationView::vkbClosed()
{
    emit vkbOpened(false);
    
    scrollToBottom();
    
    this->setMaximumHeight(-1);
    
    
    connect(mVkbHost,SIGNAL(keypadOpened()),this,SLOT(vkbOpened()));
}

//---------------------------------------------------------------
// MsgConversationView::serviceRequestError
// @see header file
//---------------------------------------------------------------
void MsgConversationView::serviceRequestError(int errorCode, const QString& errorMessage)
{
    QDEBUG_WRITE_FORMAT(errorMessage,errorCode);
}

//---------------------------------------------------------------
// MsgConversationView::activateInputBlocker
// @see header file
//---------------------------------------------------------------
void MsgConversationView::activateInputBlocker()
{
    mainWindow()->setInteractive(false);
}

//---------------------------------------------------------------
// MsgConversationView::deactivateInputBlocker
// @see header file
//---------------------------------------------------------------
void MsgConversationView::deactivateInputBlocker()
{
    mainWindow()->setInteractive(true);
}

//---------------------------------------------------------------
// MsgConversationView::handleProvisoningMsg
// @see header file
//---------------------------------------------------------------
void MsgConversationView::handleProvisoningMsg(int msgId)
	{
		QString messageId;
    messageId.setNum(msgId);

    XQApplicationManager* aiwMgr = new XQApplicationManager();

    XQAiwRequest* request = aiwMgr->create("com.nokia.services.MDM", 
            "Provisioning",
            "ProcessMessage(QString)", true); // embedded

    if (request) {
    QList<QVariant> args;
    args << QVariant(messageId);
    request->setArguments(args);

    // Send the request
    bool res = request->send();

    // Cleanup
    delete request;
    }

    delete aiwMgr;
	}

//---------------------------------------------------------------
// MsgConversationView::onDialogSettingsLaunch
// @see header file
//---------------------------------------------------------------
void MsgConversationView::onDialogSettingsLaunch(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        //switch to settings view
         QVariantList param;
         param << MsgBaseView::MSGSETTINGS;
         param << MsgBaseView::CV;
         param << MsgSettingsView::SMSView;
         emit switchView(param);
    }

}

//---------------------------------------------------------------
// MsgConversationView::onDialogdeleteMsg
// @see header file
//---------------------------------------------------------------
void MsgConversationView::onDialogdeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        QModelIndex index = mConversationList->currentIndex();
        if (index.isValid()) {
            int count = mMessageModel->rowCount();
            //delete message
            qint32 messageId = index.data(ConvergedMsgId).toLongLong();
            if (messageId) {
                QList<int> msgIdList;
                msgIdList.append(messageId);
                ConversationsEngine::instance()->deleteMessages(msgIdList);
                //switch view               
                if (count == 1) {
                    QVariantList param;
                    param << MsgBaseView::CLV; // target view
                    param << MsgBaseView::CV; // source view
                    emit switchView(param);
                }
            }
        }

    }

}

//---------------------------------------------------------------
// MsgConversationView::onDialogDownLoadMsg
// @see header file
//--------------------------------------------------------------
void MsgConversationView::onDialogDownLoadMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        downloadMessage();
    }
    
    //if message unread, mark as read now
    QModelIndex index = mConversationList->currentIndex();
    qint32 messageId = index.data(ConvergedMsgId).toLongLong();
    QList<int> msgIdList;
    if(index.data(UnReadStatus).toInt())
    {
        msgIdList.clear();
        msgIdList << messageId;
        ConversationsEngine::instance()->markMessagesRead(msgIdList);
    }
}

//---------------------------------------------------------------
// MsgConversationView::onDialogSaveTone
// @see header file
//--------------------------------------------------------------
void MsgConversationView::onDialogSaveTone(HbAction* action)
    {
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        saveRingingTone();
    }
}

//---------------------------------------------------------------
// MsgConversationView::onOrientationChanged
// @see header file
//---------------------------------------------------------------
void MsgConversationView::onOrientationChanged(Qt::Orientation newOrientation)
{
    Q_UNUSED(newOrientation)

    // On orientation change always make the preserved index(last visible item) to be visible
    if(mVisibleIndex.isValid())
    {
    mConversationList->scrollTo(mVisibleIndex, HbAbstractItemView::PositionAtBottom);
    mVisibleIndex = QModelIndex();
    }
}

//---------------------------------------------------------------
// MsgConversationView::onOrientationAboutToBeChanged
// @see header file
//---------------------------------------------------------------
void MsgConversationView::onOrientationAboutToBeChanged()
{
    // Preserve the model index of the last visible item to be scrolled on orientation change
    QList<HbAbstractViewItem *>items = mConversationList->visibleItems();
    if (items.count() > 0) {
        mVisibleIndex = items.last()->modelIndex();
    }
}

//---------------------------------------------------------------
// MsgConversationView::onViewReady
// @see header file
//---------------------------------------------------------------
void MsgConversationView::onViewReady()
{
    mViewReady = true;
    //Disconnect list View's signals, for avoiding execution of the default implementaion
    disconnect(mainWindow(), SIGNAL(aboutToChangeOrientation()), mConversationList, 0);
    disconnect(mainWindow(), SIGNAL(orientationChanged(Qt: rientation)), mConversationList, 0);
    
   fetchMoreConversations();
}

//---------------------------------------------------------------
// MsgConversationView::handleShortTap
// @see header file
//---------------------------------------------------------------
void MsgConversationView::handleShortTap()
{
    HbAbstractViewItem* item = mConversationList->currentViewItem();
    QRectF rc = item->rect();
    QPointF p = item->mapToScene(rc.center());
    
    showContextMenu(item,p,HbPopup::TopEdgeCenter);
}

//---------------------------------------------------------------
// MsgConversationView::handleShortTap
// @see header file
//---------------------------------------------------------------
void MsgConversationView::showContextMenu(HbAbstractViewItem* viewItem,const QPointF& point, int placement)
{    
    MsgConversationViewItem* item = qgraphicsitem_cast<MsgConversationViewItem *>(viewItem);

    // Show the item-specific menu
    if (this->isVisible())
    {
        //If message is in Sending state or is Scheduled to be sent later,
        //do not allow any operations on the message
        int sendingState = item->modelIndex().data(SendingState).toInt();
        
        if(sendingState == ConvergedMessage::Scheduled ||
            sendingState == ConvergedMessage::Sending ||
            sendingState == ConvergedMessage::Waiting)
            {
                return;
            }
        // Create new menu
        HbMenu* contextMenu = new HbMenu();
        contextMenu->setAttribute(Qt::WA_DeleteOnClose);
        contextMenu->setPreferredPos(point,HbPopup::Placement(placement));
        setContextMenu(item, contextMenu, sendingState);
        contextMenu->show();
    }
}

//---------------------------------------------------------------
// MsgConversationView::saveVCard
// @see header file
//---------------------------------------------------------------
void MsgConversationView::saveVCard()
{
    QModelIndex index = mConversationList->currentIndex();
    
    QString filepath = index.data(Attachments).toStringList().at(0);
    bool result = MsgContactsUtil::launchVCardViewer(filepath);
    if(result)
    {
        int messageId = index.data(ConvergedMsgId).toInt();
        QList<int> msgIdList;
        if(index.data(UnReadStatus).toInt())
        {
            msgIdList.clear();
            msgIdList << messageId;
            ConversationsEngine::instance()->markMessagesRead(msgIdList);
        }
    } 
}

//---------------------------------------------------------------
// MsgConversationView::isSharedMessage
// @see header file
//---------------------------------------------------------------
bool MsgConversationView::isSharedMessage(qint32 messageId)
{
    bool shared = false;

    UniDataModelLoader* pluginLoader = new UniDataModelLoader();
    
    UniDataModelPluginInterface* pluginInterface =
        pluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    
    CMsvSession* session = pluginInterface->session();

    TMsvEntry entry; 
    TMsvId service;
    session->GetEntry(messageId, service, entry);
    
    if(entry.MultipleRecipients())
    {
        shared = true;
    }
    delete pluginLoader;    

    return shared;
}

//---------------------------------------------------------------
// MsgConversationView::onAudioSelected
// @see header file
//---------------------------------------------------------------
void MsgConversationView::onAudioSelected(QString& filePath)
{
    QVariantList params;
    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);

    ConvergedMessage message;
    message.setBodyText(mEditorWidget->content());
   
    // add address from contact-card to to-field    
    ConvergedMessageAddressList addresses;
    addresses = mContactCardWidget->address();
    
    ConvergedMessageAddress address;
    address.setAlias(addresses.at(0)->alias());
    address.setAddress(addresses.at(0)->address());
    message.addToRecipient(address);
    
    //add the attachment as selected from audio picker
    ConvergedMessageAttachment* attachment =
                new ConvergedMessageAttachment(filePath);
    ConvergedMessageAttachmentList attachmentList;
    attachmentList.append(attachment);
    message.addAttachments(attachmentList);
    message.serialize(messageStream);

    params << MsgBaseView::UNIEDITOR; // target view
    params << MsgBaseView::CV; // source view
    params << dataArray;
    params << MsgBaseView::ADD_AUDIO;
    emit switchView(params);
}

//---------------------------------------------------------------
// MsgConversationView::handleKeyEvent
// @see header file
//---------------------------------------------------------------
bool MsgConversationView::handleKeyEvent(int key)
{
    bool eventHandled = false;
    if (Qt::Key_Yes == key) {
        //getting address of last sent/received msg.
        const int rowCount = mMessageModel->rowCount();
        QModelIndex index = mMessageModel->index(rowCount - 1, 0);
        QString address = index.data(ConversationAddress).toString();

        if (!address.isEmpty()) {
            call(address);
            eventHandled = true;
        }
    }
    return eventHandled;
}

//---------------------------------------------------------------
// MsgConversationView::call
// @see header
//---------------------------------------------------------------
void MsgConversationView::call(const QString& address)
{
    QString service("phoneui");
    QString interface("com.nokia.symbian.ICallDial");
    QString operation("dial(QString)");

    XQApplicationManager appManager;
    QScopedPointer<XQAiwRequest> request(appManager.create(service, interface, operation, false));
    if (request) {
        QList<QVariant> args;
        args << address;
        request->setArguments(args);
        request->send();
    }
}

// EOF
