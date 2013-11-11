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

// SYSTEM INCLUDES
#include <e32base.h>
#include <HbMenu>
#include <HbAction>
#include <HbToolBar>
#include <QGraphicsLinearLayout>
#include <HbGroupBox>
#include <HbScrollArea>
#include <QDateTime>
#include <QDir>
#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <xqconversions.h>
#include <HbToolBarExtension>
#include <HbListWidget>
#include <HbFrameBackground>
#include <HbListViewItem>
#include <HbListWidgetItem>
#include <HbNotificationDialog>
#include <HbDeviceNotificationDialog>
#include <HbMessageBox>
#include <HbAbstractVkbHost>
#include <HbMainWindow>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <HbStyleLoader>
// QT Mobility for fetching business card
#include <qmobilityglobal.h>
#include <qversitwriter.h>
#include <qversitdocument.h>
#include <qcontact.h>
#include <qcontactmanager.h>
#include <qversitcontactexporter.h>
#include <cntservicescontact.h>
#include <commonphoneparser.h>      // Common phone number validity checker

// USER INCLUDES
#include "debugtraces.h"
#include "msgunieditorview.h"
#include "msgunieditoraddress.h"
#include "msgunieditorsubject.h"
#include "msgunieditorbody.h"
#include "msgunieditormonitor.h"
#include "msgunieditorattachmentcontainer.h"
#include "msgsendutil.h"
#include "convergedmessageaddress.h"
#include "UniEditorGenUtils.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
#include "msgsettingsview.h"
#include "msgcontacthandler.h"
#include "msgaudiofetcherdialog.h"

QTM_USE_NAMESPACE
// Constants
const QString SEND_ICON("qtg_mono_send");
const QString ATTACH_ICON("qtg_mono_attach");
// temporary folder for unieditor
const QString UNIFIED_EDITOR_TEMP_FOLDER("unifiededitor");
// invalid chars in vcard
const QString INVALID_FILENAME_CHARS("[?*<>/\"|\\:]");
// replacement char for invalid char
const QChar REPLACE_CHAR('_');

const int INVALID_MSGID = -1;
// vcard file extn.
const QString FILE_EXTN(".vcf");
// Max vcards inside a msg. Using a very large number.
// TODO: how we can avoid this?
const int MAX_VCARDS(1000);

// LOCALIZED CONSTANTS
#define LOC_TITLE hbTrId("txt_messaging_title_messaging")
//To,Cc.Bcc
#define LOC_TO  hbTrId("txt_messaging_formlabel_to")
#define LOC_CC hbTrId("txt_messaging_formlabel_cc")
#define LOC_BCC hbTrId("txt_messaging_formlabel_bcc")

//attach options
#define LOC_PHOTO           hbTrId("txt_messaging_opt_attach_sub_photo")
#define LOC_SOUND           hbTrId("txt_messaging_opt_attach_sub_sound")
#define LOC_BUSINESS_CARD   hbTrId("txt_messaging_opt_sub_business_card")

//options menu.
#define LOC_ADD_SUBJECT     hbTrId("txt_messaging_opt_add_subject")
#define LOC_ADD_CC_BCC      hbTrId("txt_messaging_opt_add_cc_bcc")
#define LOC_PRIORITY        hbTrId("txt_messaging_opt_priority")
#define LOC_DELETE_MESSAGE  hbTrId("txt_messaging_opt_delete_message")

//priority sub menu
#define LOC_HIGH hbTrId("txt_messaging_opt_attach_sub_high")
#define LOC_NORMAL hbTrId("txt_messaging_opt_attach_sub_normal")
#define LOC_LOW hbTrId("txt_messaging_opt_attach_sub_low")

//group box
#define LOC_OTHER_RECIPIENTS(n) hbTrId("txt_messaging_title_ln_other_recipients",n)
#define LOC_OTHER_RECIPIENTS_EXPAND hbTrId("txt_messaging_title_other_recipients")

//saved to draft note
#define LOC_SAVED_TO_DRAFTS    hbTrId("txt_messaging_dpopinfo_saved_to_drafts")

//delete confermation
#define LOC_NOTE_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")

// attachment addition failure note
#define LOC_UNABLE_TO_ADD_ATTACHMENTS hbTrId("txt_messaging_dpopinfo_unable_to_attach_l1_of_l2")

//extension list item frame.
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");

//settings confirmation
#define LOC_DIALOG_SMS_SETTINGS_INCOMPLETE hbTrId("txt_messaging_dialog_sms_message_centre_does_not_e")
#define LOC_DIALOG_MMS_SETTINGS_INCOMPLETE hbTrId("txt_messaging_dialog_mms_access_point_not_defined")
#define LOC_NOTE_FILES_MISSED_DRAFTS hbTrId("txt_messaging_dpopinfo_some_files_in_the_message")
#define LOC_NOTE_FILES_MISSED_SEND hbTrId("txt_messaging_dialog_unable_to_send_message_some")
// LOCAL FUNCTIONS

//---------------------------------------------------------------
// editorTempPath
// @return fullPath of unified editor's temporary dir
//---------------------------------------------------------------
QString editorTempPath()
{
    QDir tempDir;
    QString tempPath(QDir::toNativeSeparators(tempDir.tempPath()));
    tempPath.append(QDir::separator());
    tempPath.append(UNIFIED_EDITOR_TEMP_FOLDER);
    tempPath.append(QDir::separator());
    return tempPath;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::MsgUnifiedEditorView
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorView::MsgUnifiedEditorView( QGraphicsItem *parent ) :
    MsgBaseView(parent),
    mSubjectAction(0),
    mCcBccAction(0),
    mSendAction(0),
    mMainLayout(0),
    mSubjectField(0),
    mToField(0),
    mCcField(0),
    mBccField(0),
    mBody(0),
    mContentWidget(0),
    mMsgMonitor(0),    
    mAttachmentContainer(0),
    mPluginLoader(0),
    mCanSaveToDrafts(true),
    mVkbHost(NULL),
	mDialog(NULL),
    mOriginatingSC(0),
    mOriginatingSME(0),
    mReplyPath(false)
    {
    connect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
    
    addToolBar();
    initView();
    }

//---------------------------------------------------------------
// MsgUnifiedEditorView::~MsgUnifiedEditorView
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorView::~MsgUnifiedEditorView()
{
    // clean editor's temporary contents before exiting
    removeTempFolder();
    
    //delete the popup dialog
    delete mDialog;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::initView
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::initView()
{
    if (!HbStyleLoader::registerFilePath(":/layouts")) {
        QDEBUG_WRITE("ERROR: MsgUnifiedEditorView -> HbStyleLoader::registerFilePath");
    }
    HbScrollArea* scrollArea = new HbScrollArea(this);
    this->setWidget(scrollArea);

    mContentWidget = new HbWidget(this);
    scrollArea->setContentWidget(mContentWidget);

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, mContentWidget);
    qreal vTopSpacing = 0.0;
    qreal vItemSpacing = 0.0;
    style()->parameter("hb-param-margin-gene-top",vTopSpacing);    
    style()->parameter("hb-param-margin-gene-middle-vertical",vItemSpacing);
    
    mMainLayout->setContentsMargins(0,vTopSpacing,0,0);
    mMainLayout->setSpacing(vItemSpacing);

    mMsgMonitor = new MsgUnifiedEditorMonitor(this);
    connect(mMsgMonitor, SIGNAL(enableSend(bool)), this, SLOT(enableSendButton(bool)));

    mToField = new MsgUnifiedEditorAddress( LOC_TO, mContentWidget );
    
    mBody = new MsgUnifiedEditorBody( mContentWidget);

    mMainLayout->addItem(mToField);
    mMainLayout->addItem(mBody);
 
    //Set the invalid msg id
    mOpenedMessageId.setId(-1);

    connect(mToField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mToField, SIGNAL(contentChanged()),
            mMsgMonitor, SLOT(handleContentChange()));
    connect(mToField, SIGNAL(contentChanged()),this,SLOT(onContentChanged()));
    
    connect(mBody, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBody, SIGNAL(contentChanged()),this,SLOT(onContentChanged()));
    connect(mBody, SIGNAL(contentChanged()),
            mMsgMonitor, SLOT(handleContentChange()));
    connect(mBody, SIGNAL(enableSendButton(bool)), this, SLOT(enableSendButton(bool)));    
    
}

void MsgUnifiedEditorView::addMenu()
{
    //Create Menu Options
    HbMenu* mainMenu = this->menu();
    mainMenu->setFocusPolicy(Qt::NoFocus);
	
    //if subject field / cc,bcc fields are already present don't add corresponding actions.
    if(!mSubjectField)
    {
        mSubjectAction = mainMenu->addAction(LOC_ADD_SUBJECT);
    }
    
    if(!mCcField)
    {
        mCcBccAction = mainMenu->addAction(LOC_ADD_CC_BCC);
    }
    
    HbMenu* prioritySubMenu = mainMenu->addMenu(LOC_PRIORITY);

    HbAction* highPriorityAction = prioritySubMenu->addAction(LOC_HIGH);
    highPriorityAction->setData(ConvergedMessage::High);

    HbAction* normalPriorityAction = prioritySubMenu->addAction(LOC_NORMAL);
    normalPriorityAction->setData(ConvergedMessage::Normal);

    HbAction* lowPriorityAction = prioritySubMenu->addAction(LOC_LOW);
    lowPriorityAction->setData(ConvergedMessage::Low);

    HbAction* deleteMsgAction = mainMenu->addAction(LOC_DELETE_MESSAGE);

    connect(mSubjectAction,SIGNAL(triggered()),this, SLOT(addSubject()));
    connect(mCcBccAction,SIGNAL(triggered()),this, SLOT(addCcBcc()));
    connect(highPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(normalPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(lowPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(deleteMsgAction,SIGNAL(triggered()),this, SLOT(deleteMessage()));

}

void MsgUnifiedEditorView::openDraftsMessage(const QVariantList& editorData)
{
    // unpack editor's data
    // first arg is convergedmessage
    QByteArray dataArray = editorData.at(0).toByteArray();
    ConvergedMessage* messageDetails = new ConvergedMessage;
    QDataStream stream(&dataArray, QIODevice::ReadOnly);
    messageDetails->deserialize(stream);
    ConvergedMessage::MessageType messageType = messageDetails->messageType();
    ConvergedMessageId messageId = *(messageDetails->id());
    delete messageDetails;

    if(!mPluginLoader)
    {
        mPluginLoader = new UniEditorPluginLoader(this);
    }

    UniEditorPluginInterface* pluginInterface =
        mPluginLoader->getUniEditorPlugin(messageType);

    mOpenedMessageId.setId(messageId.getId());
    mmOpenedMessageType = messageType;

    //Fetch the converged message from the msgId
    ConvergedMessage* msg;
    msg = pluginInterface->convertFrom(messageId.getId());

    if( msg != NULL )
    {
        mReplyPath = msg->replyPath();
        if(mReplyPath)
        {
            mOriginatingSC = msg->originatingSC();
            mOriginatingSME = msg->toAddressList().at(0)->address();
        }
        //Populate the content inside editor
        populateContentIntoEditor(*msg,true); // true as it is  draft message
        delete msg;
    }
    
    mCanSaveToDrafts = false;  
}

void MsgUnifiedEditorView::fetchMessageFromStore(
        ConvergedMessageId& messageId,
        ConvergedMessage::MessageType messageType,
        int editorOperation)
{
    if(!mPluginLoader)
    {
        mPluginLoader = new UniEditorPluginLoader(this);
    }
    UniEditorPluginInterface* pluginInterface = NULL;
    if( messageType == ConvergedMessage::Mms )
    {
        pluginInterface =
                mPluginLoader->getUniEditorPlugin(ConvergedMessage::Mms);
    }
    else // For sms,vcard,vcal cases
    {
        pluginInterface =
                mPluginLoader->getUniEditorPlugin(ConvergedMessage::Sms);
    }

    //Fetch the converged message from the msgId
    ConvergedMessage* msg;
    msg = pluginInterface->convertFrom(messageId.getId(),
                    (UniEditorPluginInterface::EditorOperation)editorOperation);
    if( msg != NULL )
    {
        if(editorOperation == UniEditorPluginInterface::Reply)
        {
            mReplyPath = msg->replyPath();
            if(mReplyPath)
            {
                mOriginatingSC = msg->originatingSC();
                mOriginatingSME = msg->toAddressList().at(0)->address();
            }
        }
        //Populate the content inside editor
        populateContentIntoEditor(*msg);
        delete msg;
    }
}

void MsgUnifiedEditorView::populateContent(const QVariantList& editorData)
{
    // unpack editor's data
    // first arg is convergedmessage
    QByteArray dataArray = editorData.at(0).toByteArray();
    ConvergedMessage* messageDetails = new ConvergedMessage;
    QDataStream stream(&dataArray, QIODevice::ReadOnly);
    messageDetails->deserialize(stream);

    // get next arg i.e. editor operation command
    int editorOp = 0;
    if(editorData.length() > 1)
    {
        editorOp = editorData.at(1).toInt();
    }

    // population logic based on editor Operation command
    switch(editorOp)
    {
        case MsgBaseView::ADD_SUBJECT:
        {
            addSubject();
            setFocus(mSubjectField);
        }
        break;
        case MsgBaseView::ADD_VCARD:
        {
            contactsFetched(editorData.at(2));
        }
        break;
        case MsgBaseView::FORWARD_MSG:
        {
            fetchMessageFromStore(*messageDetails->id(),
                                   messageDetails->messageType(),
                                   UniEditorPluginInterface::Forward);
        }
        break;
        case MsgBaseView::REPLY_MSG:
        {
            fetchMessageFromStore(*messageDetails->id(),
                                   messageDetails->messageType(),
                                   UniEditorPluginInterface::Reply);
        }
        break;
        case MsgBaseView::REPLY_ALL_MSG:
        {
            fetchMessageFromStore(*messageDetails->id(),
                                   messageDetails->messageType(),
                                   UniEditorPluginInterface::ReplyAll);
        }
        break;
        default:
        break;
    }

    // additional common operations for non-forwarded messages
    if(editorOp != MsgBaseView::FORWARD_MSG)
    {
        if(editorOp == MsgBaseView::ADD_RECIPIENTS)
        {
            // CV sends contact card address as the first address
            ConvergedMessageAddressList toAddresses = 
                    messageDetails->toAddressList();
            int addrCount = toAddresses.count();
            if(addrCount > 0)
            {
                // add contact card address first
                ConvergedMessageAddress *firstAddress =
                        new ConvergedMessageAddress();
                firstAddress->setAlias(toAddresses.at(0)->alias());
                firstAddress->setAddress(toAddresses.at(0)->address());
                ConvergedMessageAddressList firstList;
                firstList << firstAddress;
                mToField->setAddresses(firstList);
            
                // add remaining contacts now
                ConvergedMessageAddressList otherList;
                for(int i=1; i<addrCount; i++)
                {
                    otherList << toAddresses.at(i);
                }
                mToField->setAddresses(otherList);
            }
        }
        else
        {
            mToField->setAddresses(messageDetails->toAddressList());
        }
        QString bodyTxt = messageDetails->bodyText();
        mBody->setText(bodyTxt);

        int attachmentCount = messageDetails->attachments().count();
        QStringList pendingAttList;
        for(int i=0; i<attachmentCount; i++)
        {
            int imageSize=0;
            QString mimeType;
            TMsgMediaType mediaType = EMsgMediaUnknown;
            QString filePath = messageDetails->attachments().at(i)->filePath();
            
           
            UniEditorGenUtils* genUtils = q_check_ptr(new UniEditorGenUtils);
            
            TRAP_IGNORE(genUtils->getFileInfoL(filePath,imageSize,
                                           mimeType,mediaType));
            delete genUtils;
            switch(mediaType)
            {
                case EMsgMediaImage:
                {
                    mBody->setImage(filePath);
                }
                break;
                 case EMsgMediaAudio:
                {
                    mBody->setAudio(filePath);
                }
                break;
                default:
                {
                    pendingAttList << filePath;
                }
                break;
            }
        }
        // add pending attachments in bulk
        addAttachments(pendingAttList);
    }
    delete messageDetails; 
}

void MsgUnifiedEditorView::populateContentIntoEditor(
    const ConvergedMessage& messageDetails,bool draftMessage)
{
    // skip first-time MMS type switch note for draft
    mMsgMonitor->setSkipNote(true);
    mToField->skipMaxRecipientQuery(true);

    mToField->setAddresses(messageDetails.toAddressList(),draftMessage);
    if(messageDetails.ccAddressList().count() > 0 )
    {
        if(!mCcField)
        {
        addCcBcc();
        }
        mCcField->setAddresses(messageDetails.ccAddressList(),draftMessage);
    }
    if(messageDetails.bccAddressList().count() > 0 )
    {
        if(!mBccField)
        {
        addCcBcc();
        }
        mBccField->setAddresses(messageDetails.bccAddressList(),draftMessage);
    }
    if(messageDetails.subject().size() > 0)
    {
       if(!mSubjectField)
       {
           addSubject();
       }
       mSubjectField->setText(messageDetails.subject());
    }

    if(messageDetails.priority()==ConvergedMessage::High ||
            messageDetails.priority() == ConvergedMessage::Low)
    {
        if(!mSubjectField)
        {
            addSubject();
        }
        mSubjectField->setPriority(messageDetails.priority());
    }

    QString bodyTxt = messageDetails.bodyText();
    mBody->setText(bodyTxt);

    ConvergedMessageAttachmentList attachmentList =
        messageDetails.attachments();
    int attachmentCount = attachmentList.count();
     
    UniEditorGenUtils* genUtils = q_check_ptr(new UniEditorGenUtils);

    QStringList pendingAttList;
    for( int i=0; i < attachmentCount; i++ )
    {
        QString filePath = messageDetails.attachments().at(i)->filePath();

        if(attachmentList.at(i)->attachmentType() ==
            ConvergedMessageAttachment::EInline)
        {
            int imageSize=0;
            QString mimeType;
            TMsgMediaType mediaType = EMsgMediaUnknown;
            TRAP_IGNORE(genUtils->getFileInfoL(filePath,imageSize,
                mimeType,mediaType));

            switch(mediaType)
            {
                case EMsgMediaImage:
                {
                    mBody->setImage(filePath,draftMessage);
                    break;
                }
                case EMsgMediaAudio:
                {
                    mBody->setAudio(filePath);
                    break;
                }
                default:
                {
                    pendingAttList << filePath;
                    break;
                }
            }
        }
        else
        {
            pendingAttList << filePath;
        }
    }
    // add pending attachments to editor
    addAttachments(pendingAttList);

    delete genUtils;
    // ensure that any msg-type change after this are shown
    mToField->skipMaxRecipientQuery(false);
    mMsgMonitor->setSkipNote(false);
}

void MsgUnifiedEditorView::addToolBar()
{
    //Create Tool Bar
    HbToolBar *toolBar = new HbToolBar();
    toolBar->setOrientation(Qt::Horizontal);
    
    //tool bar extension for attach action.
    HbToolBarExtension* attachExtension = new HbToolBarExtension();
    mAttachAction = toolBar->addExtension(attachExtension);    
    mAttachAction->setIcon(HbIcon(ATTACH_ICON));
    
    mTBExtnContentWidget = new HbListWidget();
    mTBExtnContentWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    mTBExtnContentWidget->addItem(LOC_PHOTO);
    mTBExtnContentWidget->addItem(LOC_SOUND);
    mTBExtnContentWidget->addItem(LOC_BUSINESS_CARD);

    HbListViewItem *prototype = mTBExtnContentWidget->listItemPrototype();
    HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    connect(mTBExtnContentWidget, SIGNAL(activated(HbListWidgetItem*)),
            this, SLOT(handleViewExtnActivated(HbListWidgetItem*)));
    connect(mTBExtnContentWidget, SIGNAL(activated(HbListWidgetItem*)),
            attachExtension, SLOT(close()));

    attachExtension->setContentWidget(mTBExtnContentWidget);

    //Add Action to the toolbar and show toolbar
    mSendAction = toolBar->addAction(HbIcon(SEND_ICON),QString(),this,SLOT(send()));
    mSendAction->setDisabled(true);

    setToolBar(toolBar);
}

void MsgUnifiedEditorView::addSubject()
{
    if(mSubjectField)
    { // do nothing if already present
        return;
    }

    int index =0;
    int offset = 1;
    if(mAttachmentContainer)
    {
        ++offset;
    }
    index = mMainLayout->count() - offset;

    mSubjectField = new MsgUnifiedEditorSubject( mContentWidget);
    
    mMainLayout->insertItem(index,mSubjectField);
    connect(mSubjectField, SIGNAL(contentChanged()),
            mMsgMonitor, SLOT(handleContentChange()));
    connect(mSubjectField, SIGNAL(contentChanged()),this,SLOT(onContentChanged()));
    
    //set focus to subject field.
    HbAction* subjectAction = qobject_cast<HbAction*>(this->sender());
    if(subjectAction)
    {
        setFocus(mSubjectField);
    }
    
    // remove mainmenu's "Add Subject" action
    if(mSubjectAction)
    {
        HbMenu* mainMenu = this->menu();
        mainMenu->removeAction(mSubjectAction);
        mSubjectAction->setParent(NULL);
        delete mSubjectAction;
    }
}

void MsgUnifiedEditorView::addCcBcc()
{
    if(mCcField && mBccField)
    { // do nothing if already present
        return;
    }

    mCcField    = new MsgUnifiedEditorAddress( LOC_CC, mContentWidget );
    mBccField   = new MsgUnifiedEditorAddress( LOC_BCC, mContentWidget );
    mCcField->skipMaxRecipientQuery(true);
    mBccField->skipMaxRecipientQuery(true);

    connect(mCcField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mCcField, SIGNAL(contentChanged()), mMsgMonitor, SLOT(handleContentChange()));
    connect(mCcField, SIGNAL(contentChanged()),this,SLOT(onContentChanged()));
    
    connect(mBccField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBccField, SIGNAL(contentChanged()), mMsgMonitor, SLOT(handleContentChange()));
    connect(mBccField, SIGNAL(contentChanged()),this,SLOT(onContentChanged()));

    HbWidget* groupWidget = new HbWidget(mContentWidget);
    groupWidget->setContentsMargins(0,0,0,0);
    
    QGraphicsLinearLayout* ccBccLayout = new QGraphicsLinearLayout(Qt::Vertical, groupWidget);
    ccBccLayout->setContentsMargins(0,0,0,0);

    qreal vItemSpacing = 0.0;    
    style()->parameter("hb-param-margin-gene-middle-vertical",vItemSpacing);
    ccBccLayout->setSpacing(vItemSpacing);
    ccBccLayout->addItem(mCcField);
    ccBccLayout->addItem(mBccField);

    HbGroupBox* groupBox = new HbGroupBox(0);  
    groupBox->setObjectName("groupBox");
    
    groupBox->setContentWidget(groupWidget);
    groupBox->setHeading(LOC_OTHER_RECIPIENTS_EXPAND);
    mMainLayout->insertItem(1,groupBox);
    connect(groupBox, SIGNAL(toggled(bool)), this, SLOT(updateOtherRecipientCount(bool)));
    
    // add subject field too
    addSubject();

    //set focus to Cc field.
    HbAction* ccBccAction = qobject_cast<HbAction*>(this->sender());
    if(mCcBccAction)
    {
        setFocus(mCcField);
    }
    
    // remove mainmenu's "Add Cc/Bcc" & "Add Subject" actions
    if(mCcBccAction)
    {
    HbMenu* mainmenu = this->menu();
    mainmenu->removeAction(mCcBccAction);
    mCcBccAction->setParent(NULL);
    delete mCcBccAction;
    }
    
    this->updateGeometry();
}

void MsgUnifiedEditorView::updateOtherRecipientCount(bool state)
{
    HbGroupBox* groupBox = qobject_cast<HbGroupBox*>(sender());
    if(groupBox)
    {
        if(!state)
        {
           groupBox->setHeading(LOC_OTHER_RECIPIENTS_EXPAND);
        }
        else
        {
            int addrCount = mCcField->addressCount();
            addrCount += mBccField->addressCount();
            if(addrCount > 0)
            {
                groupBox->setHeading(LOC_OTHER_RECIPIENTS(addrCount));    
            }
        }
    }
}

void MsgUnifiedEditorView::changePriority()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    ConvergedMessage::Priority priority = ConvergedMessage::Normal;
   if(action)
        {
        int data = action->data().toInt();
        priority = ConvergedMessage::Priority(data);
        }

    addSubject();
    mSubjectField->setPriority(priority);
}

void MsgUnifiedEditorView::deleteMessage()
{
    HbMessageBox::question(LOC_NOTE_DELETE_MESSAGE,this,
                           SLOT(onDialogDeleteMsg(HbAction*)),
                           HbMessageBox::Delete | HbMessageBox::Cancel);
}

void MsgUnifiedEditorView::removeAttachmentContainer()
{
    if(mAttachmentContainer)
    {
        mMainLayout->removeItem(mAttachmentContainer);
        mAttachmentContainer->setParent(NULL);
        delete mAttachmentContainer;
        mAttachmentContainer = NULL;
    }
}

void MsgUnifiedEditorView::addAttachments(QStringList files)
{    
    int fcount = files.count();
    int i=0;
    for(i=0; i<fcount; i++)
    {
        int status = addAttachment(files.at(i));
        if(status == MsgAttachmentContainer::EAddSizeExceed)
        {
            QString displayStr = QString(LOC_UNABLE_TO_ADD_ATTACHMENTS)
                    .arg(fcount-i).arg(fcount);
            HbNotificationDialog::launchDialog(displayStr);
            break;
        }
    }
}

int MsgUnifiedEditorView::addAttachment(const QString& filepath)
{
    // do nothing if filepath is empty
    if(filepath.isEmpty())
    {
        return MsgAttachmentContainer::EAddSuccess;
    }

    if(!mAttachmentContainer)
    {
        mAttachmentContainer = new MsgAttachmentContainer( mContentWidget);
        connect(mAttachmentContainer, SIGNAL(emptyAttachmentContainer()),
                this, SLOT(removeAttachmentContainer()));
        connect(mAttachmentContainer, SIGNAL(contentChanged()),
                mMsgMonitor, SLOT(handleContentChange()));
        connect(mAttachmentContainer, SIGNAL(contentChanged()),
                this,SLOT(onContentChanged()));
        
        int index = mMainLayout->count() - 1;
        mMainLayout->insertItem(index,mAttachmentContainer);
    }

    int ret = mAttachmentContainer->addAttachment(filepath);
    if(ret != MsgAttachmentContainer::EAddSuccess)
    {
        // delete container, if it is empty
        if(mAttachmentContainer->count() == 0)
        {
            removeAttachmentContainer();
        }
    }
    return ret;
}

void MsgUnifiedEditorView::send()
{
    activateInputBlocker();
    
    // first run the address validation tests
    if( !mToField->validateContacts() ||
        (mCcField && !mCcField->validateContacts()) ||
        (mBccField && !mBccField->validateContacts()) )
    {
        deactivateInputBlocker();
        return;
    }

    // converged msg for sending
    ConvergedMessage msg;
    ConvergedMessage::MessageType messageType = MsgUnifiedEditorMonitor::messageType();
    msg.setMessageType(messageType);

    // we need to remove duplicate addresses
    bool removeDuplicates = true;
    ConvergedMessageAddressList addresses =
            mToField->addresses(removeDuplicates);
    if(messageType == ConvergedMessage::Sms &&
       addresses.isEmpty())
    {
        // no recipient specified for sms, do not send msg
        deactivateInputBlocker();
        return;
    }

    ConvergedMessageAddressList ccAddresses;
    ConvergedMessageAddressList bccAddresses;
    if(mCcField)
    {
        ccAddresses = mCcField->addresses(removeDuplicates);
    }
    if(mBccField)
    {
        bccAddresses = mBccField->addresses(removeDuplicates);
    }
    if( messageType == ConvergedMessage::Mms &&
            addresses.isEmpty() &&
            ccAddresses.isEmpty() &&
            bccAddresses.isEmpty())
    {
        // no recipient specified for mms, do not send msg
        deactivateInputBlocker();
        return;
    }
    
    //close vkb before switching view.
    mVkbHost->closeKeypad(true);

    int result = packMessage(msg);
    if(result == KErrNotFound)
    {
        HbMessageBox::information(LOC_NOTE_FILES_MISSED_SEND, 0, 0, HbMessageBox::Ok);
        deactivateInputBlocker();
        return;
    }
    // send message
    MsgSendUtil *sendUtil = new MsgSendUtil(this);
    int sendResult = sendUtil->send(msg);
    delete sendUtil;
    
    // all checks and validations happen before send
    if( sendResult == KErrNone)
    {
        //After sending the new content to drafts chk if the msg
        //was originally opened from drafts and delete the opened entry
        if( mOpenedMessageId.getId() != -1)
        {

            if(!mPluginLoader)
            {
                mPluginLoader = new UniEditorPluginLoader(this);
            }

            UniEditorPluginInterface* pluginInterface =
                mPluginLoader->getUniEditorPlugin(messageType);
            //TODO delete the entry
            pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }

        ConvergedMessageAddressList addrList = addresses;
        QString receipient;

        int recepientCount = addrList.count();
        if(recepientCount)
        {
            receipient = addrList.at(0)->address();
        }

        if( mCcField && mBccField)
        {
            addrList = ccAddresses;
            int ccCount = addrList.count();
            recepientCount += ccCount;
            if(ccCount)
            {
                receipient = addrList.at(0)->address();
            }

            addrList = bccAddresses;
            int bccCount = addrList.count();
            recepientCount += bccCount;
            if(bccCount)
            {
                receipient = addrList.at(0)->address();
            }
        }
        
    QVariantList params;

    if(recepientCount == 1 )
        {
        params << MsgBaseView::CV;  // target view
        params << MsgBaseView::UNIEDITOR; // source view

        params << receipient;
        }
    else
        {
        params << MsgBaseView::CLV;// target view
        params << MsgBaseView::UNIEDITOR; // source view
        }
        deactivateInputBlocker();
        emit switchView(params);
    }
    else
    {
        deactivateInputBlocker();
        if(sendResult == KErrNotFound)
        {
            if (messageType == ConvergedMessage::Sms)
            {
                HbMessageBox::question(LOC_DIALOG_SMS_SETTINGS_INCOMPLETE,
                                       this,SLOT(onDialogSmsSettings(HbAction*)),
                                       HbMessageBox::Ok | HbMessageBox::Cancel);
            }
            else
            {
                HbMessageBox::question(LOC_DIALOG_MMS_SETTINGS_INCOMPLETE,
                                       this,SLOT(onDialogMmsSettings(HbAction*)),                             
                                       HbMessageBox::Ok | HbMessageBox::Cancel);
            }
        }
    }
}

int MsgUnifiedEditorView::packMessage(ConvergedMessage &msg, bool isSave)
{
    // reset reply-path if originating SME constraint is broken
    if(mReplyPath && isReplyPathBroken())
    {
        mReplyPath = false;
    }

    msg.setReplyPath(mReplyPath);
    if(mReplyPath)
    {
        msg.setOriginatingSC(mOriginatingSC);
    }

    ConvergedMessage::MessageType messageType = MsgUnifiedEditorMonitor::messageType();
    msg.setMessageType(messageType);
    // If isSave is true (save to draft usecase), then don't remove duplicates
    // If isSave is false (send usecase), then remove duplicates
    bool removeDuplicates = !isSave;
    ConvergedMessageAddressList addresses = 
            mToField->addresses(removeDuplicates);
    ConvergedMessageAddressList ccAddresses;
    ConvergedMessageAddressList bccAddresses;
    ConvergedMessageAttachmentList mediaList;
    int errorCode = 0;
	//Don't format the addresses for save to drfats case
	if(!isSave)
	{
       formatAddresses(addresses);
    }
	
    msg.addToRecipients(addresses);
    msg.setBodyText(mBody->text());
    msg.setDirection(ConvergedMessage::Outgoing);
    QDateTime time = QDateTime::currentDateTime();
    msg.setTimeStamp(time.toTime_t());

    if(messageType == ConvergedMessage::Mms)
    {
        if(mCcField)
        {
            ccAddresses = mCcField->addresses(removeDuplicates);
        }

        if(mBccField)
        {
            bccAddresses = mBccField->addresses(removeDuplicates);
        }

        if(removeDuplicates)
        {
            int matchDigitsCount = MsgUnifiedEditorAddress::contactMatchDigits();
            //comapre cc and to field,remove duplicate from cc
            foreach(ConvergedMessageAddress *ccAddress,ccAddresses)
            {
                foreach(ConvergedMessageAddress *toAddress,addresses)
                {
                    if(0 == ccAddress->address().right(matchDigitsCount).compare(toAddress->address().right(matchDigitsCount)))
                    {
                        ccAddresses.removeOne(ccAddress);
                    }
                }
            }
            //comapre bcc and cc field,remove duplicate from bcc
            foreach(ConvergedMessageAddress *bccAddress,bccAddresses)
            {
                foreach(ConvergedMessageAddress *ccAddress,ccAddresses)
                {
                    if(0 == bccAddress->address().right(matchDigitsCount).compare(ccAddress->address().right(matchDigitsCount)))
                    {
                        bccAddresses.removeOne(bccAddress);
                    }
                }
            }
            //comapre bcc and to field,remove duplicate from bcc
            foreach(ConvergedMessageAddress *bccAddress,bccAddresses)
            {
                foreach(ConvergedMessageAddress *toAddress,addresses)
                {
                    if(0 == bccAddress->address().right(matchDigitsCount).compare(toAddress->address().right(matchDigitsCount)))
                    {
                        bccAddresses.removeOne(bccAddress);
                    }
                }
            }
        }

        if(ccAddresses.count()>0)
        {
		//Don't format the addresses for save to drfats case
	    if(!isSave)
	    {
           formatAddresses(ccAddresses);
        }        
        msg.addCcRecipients(ccAddresses);
        }
        if(bccAddresses.count()>0)
        {
		//Don't format the addresses for save to drfats case
	    if(!isSave)
	    {
           formatAddresses(bccAddresses);        
		}
        msg.addBccRecipients(bccAddresses);
        }
        if(mSubjectField)
        {
            msg.setSubject(mSubjectField->text());
            msg.setPriority(mSubjectField->priority());
        }

        QStringList mediafiles(mBody->mediaContent());
        if (!mediafiles.isEmpty())
        {
            for (int i = 0; i < mediafiles.size(); ++i)
            {
                if(QFile::exists(mediafiles.at(i)))
                {
                ConvergedMessageAttachment* attachment =
                    new ConvergedMessageAttachment(
                        mediafiles.at(i),
                        ConvergedMessageAttachment::EInline);
                mediaList << attachment;
                }
                else
                {   mBody->removeMediaContent(mediafiles.at(i));
                    errorCode = KErrNotFound;
                }
            }

        }
        if(mediaList.count() > 0)
        {
            msg.addAttachments(mediaList);
        }
    }

    // sms/mms attachment list
    ConvergedMessageAttachmentList attachmentList;
        if(mAttachmentContainer)
        {
            MsgUnifiedEditorAttachmentList editorAttachmentList =
                mAttachmentContainer->attachmentList();
                for (int i = 0; i < editorAttachmentList.count(); ++i)
                {
                    if(QFile::exists(editorAttachmentList.at(i)->path()))
                    {
                        ConvergedMessageAttachment* attachment =
                                                new ConvergedMessageAttachment(
                                                    editorAttachmentList.at(i)->path(),
                                                    ConvergedMessageAttachment::EAttachment);
                                            attachmentList << attachment;    
                    }
                    else
                    {
                        mAttachmentContainer->deleteAttachment(editorAttachmentList.at(i));
                        errorCode = KErrNotFound;
                    }   
                }
            }
        if(attachmentList.count() > 0)
        {
            msg.addAttachments(attachmentList);
        }
        return errorCode;
}

int MsgUnifiedEditorView::saveContentToDrafts()
{
    if(!mCanSaveToDrafts)
        {
        return mOpenedMessageId.getId(); // return currently opened message id
        }
    
    activateInputBlocker();
    ConvergedMessage::MessageType messageType = MsgUnifiedEditorMonitor::messageType();

    ConvergedMessageAddressList addresses = mToField->addresses();

    UniEditorPluginInterface* pluginInterface = NULL;

    if( mOpenedMessageId.getId() != -1)
    {
        if(!mPluginLoader)
        {
            mPluginLoader = new UniEditorPluginLoader(this);
        }

        pluginInterface =
            mPluginLoader->getUniEditorPlugin(messageType);
    }

    if(messageType == ConvergedMessage::Sms &&
            addresses.isEmpty() &&
            MsgUnifiedEditorMonitor::bodySize() <= 0 &&
            MsgUnifiedEditorMonitor::containerSize() <= 0)
    {
        if(mOpenedMessageId.getId() != -1)
        {
        pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }

        // if empty msg, do not save
        deactivateInputBlocker();
        return INVALID_MSGID;
    }

    ConvergedMessageAddressList ccAddresses;
    ConvergedMessageAddressList bccAddresses;
    if(mCcField)
    {
        ccAddresses = mCcField->addresses();
    }
    if(mBccField)
    {
        bccAddresses = mBccField->addresses();
    }
    int subectSize =0;
    if(mSubjectField)
    {
        subectSize = mSubjectField->text().size();
    }

    if( messageType == ConvergedMessage::Mms &&
            addresses.isEmpty() &&
            ccAddresses.isEmpty() &&
            bccAddresses.isEmpty() &&
            subectSize <= 0 &&
            MsgUnifiedEditorMonitor::bodySize() <= 0 &&
            MsgUnifiedEditorMonitor::containerSize() <= 0)
    {
        if(mOpenedMessageId.getId() != -1)
        {
            pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }
        // if empty msg, do not send
        deactivateInputBlocker();
        return INVALID_MSGID;
    }
    ConvergedMessage msg;
    int result = packMessage(msg, true);
    if(result == KErrNotFound)
        {
        HbNotificationDialog::launchDialog(LOC_NOTE_FILES_MISSED_DRAFTS);
        if(messageType == ConvergedMessage::Sms &&
                    addresses.isEmpty() &&
                    MsgUnifiedEditorMonitor::bodySize() <= 0 &&
                    MsgUnifiedEditorMonitor::containerSize() <= 0)
            {
                if(mOpenedMessageId.getId() != -1)
                {
                pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
                }

                // if empty msg, do not save
                deactivateInputBlocker();
                return INVALID_MSGID;
            }
        }
    // save to drafts
    MsgSendUtil *sendUtil = new MsgSendUtil(this);
    int msgId = sendUtil->saveToDrafts(msg);
    delete sendUtil;

    // If saving new msg to draft succeeded and also if the originally
    // opened msg was from drafts, then delete the original msg entry
    if(msgId != -1 && mOpenedMessageId.getId() != -1)
    {
        pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        mOpenedMessageId.setId(INVALID_MSGID);
    }

    deactivateInputBlocker();

    bool res = ((msgId > INVALID_MSGID)? true : false);
    
    if(res)
        {
        HbDeviceNotificationDialog::notification("", LOC_SAVED_TO_DRAFTS);
        }
    return msgId;
}

bool MsgUnifiedEditorView::handleKeyEvent(int key)
{
    bool eventHandled = false;
    if (Qt::Key_Yes == key && mSendAction->isEnabled()) {
        eventHandled = true;
        send();
    }

    return eventHandled;
}

void MsgUnifiedEditorView::resizeEvent( QGraphicsSceneResizeEvent * event )
{
 Q_UNUSED(event)
 #ifdef _DEBUG_TRACES_
 qDebug()<<"resize event";
#endif

 mContentWidget->resize(this->rect().width(),this->rect().height()+1);
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::createVCards
// @see header file
//---------------------------------------------------------------
int MsgUnifiedEditorView::createVCards(
        const QVariant& value, QStringList& filelist)
{
    // make sure that temp-folder is created for storing vcards
    if(!createTempFolder())
    {
        return KErrGeneral;
    }

    // extract contact-list
    QContactManager* contactManager = new QContactManager("symbian");
    CntServicesContactList cntServicesContacts = qVariantValue<CntServicesContactList>(value);
    int cntCount = cntServicesContacts.count();
    
    QCRITICAL_WRITE_FORMAT("servicecontactlist count:",cntCount);
    
    QList<QtMobility::QContact> contactList;
    for(int i = 0; i < cntCount; i++ )
    {        
        contactList << contactManager->contact( cntServicesContacts.at(i).mContactId );
    }
    delete contactManager;
   
    // get list of all versit-documents
    QVersitDocument::VersitType versitType(QVersitDocument::VCard21Type);
    
    QVersitContactExporter exporter;
    bool ret_val = exporter.exportContacts(contactList, versitType);
    
    if(ret_val == false)
        { 
        QCRITICAL_WRITE("QVersitContactExporter::exportContacts returned false");
        return KErrGeneral;
        }  
    
    // process the documents
	QList<QtMobility::QVersitDocument> documentList = exporter.documents();
	
    // loop though and create a vcard for each contact
    QVersitWriter* writer = new QVersitWriter();
    for(int i = 0; i < cntCount; i++ )
    {
        // buffer to hold vcard data fetched from contacts
        QBuffer contactsbuf;
        contactsbuf.open(QBuffer::ReadWrite);
        writer->setDevice( &contactsbuf );

        //write document data to buffer
        QList<QtMobility::QVersitDocument> currDoc;
        currDoc << documentList.at(i);
        writer->startWriting(currDoc);
        if(writer->waitForFinished())
        {
            // generate file name
            QString displayLabel = contactList.at(i).displayLabel();
            displayLabel.replace(QRegExp(INVALID_FILENAME_CHARS), REPLACE_CHAR);
            QString filepath = generateFileName(displayLabel);
            
            // create file
            QFile file(filepath);
            if(file.open(QIODevice::WriteOnly))
            {
                // trap ignore so that, incase of multiselection, other vcards are still created
                QByteArray bufArr;
                TRAP_IGNORE(
                        HBufC8* contactBuf8 = XQConversions::qStringToS60Desc8(contactsbuf.data());
                        if(contactBuf8)
                            {
                            CleanupStack::PushL(contactBuf8);
                            CBufBase* contactbufbase = CBufFlat::NewL(contactsbuf.size());
                            CleanupStack::PushL(contactbufbase);
                            
                            contactbufbase->InsertL( contactbufbase->Size(), *contactBuf8);
                            
                            TPtr8 ptrbuf(contactbufbase->Ptr(0));
                            bufArr = XQConversions::s60Desc8ToQByteArray(ptrbuf);
                            
                            CleanupStack::PopAndDestroy(contactbufbase);
                            CleanupStack::PopAndDestroy(contactBuf8);
                            
                            // write to file
                            file.write(bufArr);                                            
                            filelist << filepath;
                            }
                ); // TRAP END
                
                //close file
                file.close();                
            }
        }
    }

    // cleanup & return
    delete writer;
    return KErrNone;
}


//---------------------------------------------------------------
// MsgUnifiedEditorView::generateFileName
// @param suggestedName suggested name of the file
// @return fullPath of the generated filename
// @algo For multiselected 'Unnamed' contacts, the filename should
// be synthesized as unnamed.vcf, unnamed1.vcf, unnamed2.vcf etc.
//---------------------------------------------------------------
QString MsgUnifiedEditorView::generateFileName(QString& suggestedName)
{
    QDir editorTempDir;
    editorTempDir.cd(editorTempPath());

    for(int i=0; i<MAX_VCARDS; i++)
    {
        QString searchFileName = suggestedName;
        if(i>0)
        {
            searchFileName.append(QString::number(i));
        }
        searchFileName.append(FILE_EXTN);

        // check if filename is already used by an attachment in container
        bool alreadyExists = false;
        if(mAttachmentContainer)
        {
            MsgUnifiedEditorAttachmentList attList =
                    mAttachmentContainer->attachmentList();
            int attCount = attList.count();
            for(int j=0; j<attCount; j++)
            {
                QFileInfo fileinfo(attList.at(j)->path());
                if(searchFileName == fileinfo.fileName())
                {
                    alreadyExists = true;
                    break;
                }
            }
        }

        if(!alreadyExists)
        {
            if(i>0)
            {
                suggestedName.append(QString::number(i));
            }
            break;
        }
    }
    QString filepath(editorTempPath());
    filepath.append(suggestedName);
    filepath.append(FILE_EXTN);
    return filepath;
}

void MsgUnifiedEditorView::handleViewExtnActivated(HbListWidgetItem* item)
    {
    QString itemText = item->text();
    
    if(itemText == LOC_PHOTO)
        {
        //launch photo picker.
        fetchImages();  
        }
    else if(itemText == LOC_SOUND)
        {
        //launch audio picker
        fetchAudio();
        }
    else if(itemText == LOC_BUSINESS_CARD)
        {
        //launch contact card picker.
        fetchContacts();
        }
   
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::fetchContacts
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::fetchContacts()
{
    QString service("phonebookservices");
    QString interface("com.nokia.symbian.IContactsFetch");
    QString operation("multiFetch(QString,QString)");
  
    XQApplicationManager appManager;   
    XQAiwRequest* request = appManager.create(service, interface, operation, true); //embedded
	
    if ( request == NULL )
    {
        QCRITICAL_WRITE("AIW-ERROR: NULL request");
        return;
    }
	
    request->setSynchronous(false); // synchronous
	
    // Result handlers
    connect (request, SIGNAL(requestOk(const QVariant&)),
        this, SLOT(contactsFetched(const QVariant&)));
    connect (request, SIGNAL(requestError(int,const QString&)),
        this, SLOT(serviceRequestError(int,const QString&)));

    QList<QVariant> args;
    args << LOC_TITLE;
    args << KCntActionAll;
    args << KCntFilterDisplayAll;

    request->setArguments(args);
    if(!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed:",request->lastError());
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::fetchImages
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::fetchImages()
{
    QString service("photos");
    QString interface("com.nokia.symbian.IImageFetch");
    QString operation("fetch()");
    XQApplicationManager appManager;
    XQAiwRequest* request = appManager.create(service,interface, operation, true);//embedded
    if(!request)
    {     
        QCRITICAL_WRITE("AIW-ERROR: NULL request");
        return;
    }
    request->setSynchronous(false); // synchronous

    connect(request, SIGNAL(requestOk(const QVariant&)),
        this, SLOT(imagesFetched(const QVariant&)));
    connect(request, SIGNAL(requestError(int,const QString&)),
        this, SLOT(serviceRequestError(int,const QString&)));
    
    // Make the request
    if (!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed:" , request->lastError());
    }  
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::fetchAudio
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::fetchAudio()
{
    // Launch Audio fetcher dialog
    if(!mDialog)
    {
       mDialog = new MsgAudioFetcherDialog();
       connect(mDialog,
            SIGNAL(audioSelected(QString&)),
            this,
            SLOT(onAudioSelected(QString&)));
    }

    //show the dialog
    mDialog->show();    
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::contactsFetched
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::contactsFetched(const QVariant& value)
{
    XQAiwRequest* request = qobject_cast<XQAiwRequest*>(this->sender());
    if(request)
    {
        delete request;
    }

    // create a vcard for each contact fetched and add as attachment
    QStringList attachmentList;
    if(createVCards(value, attachmentList) == KErrNone)
    {
        addAttachments(attachmentList);
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::imagesFetched
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::imagesFetched(const QVariant& result )
{
    XQAiwRequest* request = qobject_cast<XQAiwRequest*>(this->sender());
    if(request)
    {
        delete request;        
    }
	
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 )
        {
            QString filepath(QDir::toNativeSeparators(fileList.at(0)));
            mBody->setImage(filepath);
        }
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::serviceRequestError
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::serviceRequestError(int errorCode, const QString& errorMessage)
{
    QDEBUG_WRITE_FORMAT(errorMessage,errorCode);
    Q_UNUSED(errorCode)
    Q_UNUSED(errorMessage)

    XQAiwRequest* request = qobject_cast<XQAiwRequest*>(this->sender());
    if(request)
    {
        delete request;	
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::activateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgUnifiedEditorView::activateInputBlocker()
{
    mainWindow()->setInteractive(false);
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::deactivateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgUnifiedEditorView::deactivateInputBlocker()
{
    mainWindow()->setInteractive(true);
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::setAttachOptionEnabled
// @see header file
//--------------------------------------------------------------
void MsgUnifiedEditorView::setAttachOptionEnabled(
        MsgUnifiedEditorView::TBE_AttachOption opt, bool enable)
{
    HbListWidgetItem* wgtItem = mTBExtnContentWidget->item(opt);
    wgtItem->setEnabled(enable);
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::vkbOpened
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::vkbOpened()
{
    hideChrome(true); 
    
    disconnect(mVkbHost,SIGNAL(keypadOpened()),this,SLOT(vkbOpened()));
}
      
//---------------------------------------------------------------
// MsgUnifiedEditorView::vkbClosed
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::vkbClosed()
{
    hideChrome(false);
    
    connect(mVkbHost,SIGNAL(keypadOpened()),this,SLOT(vkbOpened()));
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::vkbAboutToOpen
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::vkbAboutToOpen()
{    
	  //This is done to avoid user action on attachment
	  //toolbar button when vkb is opening
    mAttachAction->setDisabled(true);
    disconnect(mVkbHost,SIGNAL(aboutToOpen()),
            this,SLOT(vkbAboutToOpen()));
}
      
//---------------------------------------------------------------
// MsgUnifiedEditorView::vkbAboutToClose
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::vkbAboutToClose()
{      
    mAttachAction->setDisabled(false);
    connect(mVkbHost,SIGNAL(aboutToOpen()),
            this,SLOT(vkbAboutToOpen()));
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::onAudioSelected
// @see header file
//---------------------------------------------------------------
void 
MsgUnifiedEditorView::onAudioSelected(QString& filePath)
{
    if (!filePath.isEmpty())
        {
            mBody->setAudio(filePath);
        }    
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::hideChrome
//
//---------------------------------------------------------------
void MsgUnifiedEditorView::hideChrome(bool hide)
{
    if(hide)
    {        
        this->setContentFullScreen(true);
        this->hideItems(Hb::StatusBarItem | Hb::TitleBarItem);
    }
    else
    {
        this->setContentFullScreen(false);
        this->showItems(Hb::StatusBarItem | Hb::TitleBarItem);
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::doDelayedConstruction
//
//---------------------------------------------------------------
void MsgUnifiedEditorView::doDelayedConstruction()
{
    addMenu();
    createTempFolder();
    
    //Create VKB instance and listen to VKB open and close signals.
    mVkbHost = new HbAbstractVkbHost(this);
    connect(mVkbHost, SIGNAL(keypadOpened()), this, SLOT(vkbOpened()));
    connect(mVkbHost, SIGNAL(keypadClosed()), this, SLOT(vkbClosed()));
    connect(mVkbHost,SIGNAL(aboutToOpen()),this,SLOT(vkbAboutToOpen()));
    connect(mVkbHost,SIGNAL(aboutToClose()),this,SLOT(vkbAboutToClose()));
    
    disconnect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
    
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::createTempFolder
//
//---------------------------------------------------------------
bool MsgUnifiedEditorView::createTempFolder()
{
    // create editor's temp folder
    QDir tempDir;
    QString tempPath(editorTempPath());
    bool result = tempDir.mkpath(tempPath);    
    return result;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::removeTempFolder
//
//---------------------------------------------------------------
void MsgUnifiedEditorView::removeTempFolder()
{
    QDir tempDir;
    QString tempPath(editorTempPath());
    tempDir.cd(tempPath);
    QStringList contentList(tempDir.entryList());
    
    int contentCount = contentList.count();
    for(int i=0; i<contentCount; i++)
    {
        tempDir.remove(contentList.at(i));
    }
    
    tempDir.cdUp();
    tempDir.rmdir(UNIFIED_EDITOR_TEMP_FOLDER); 
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::setFocus
//
//---------------------------------------------------------------
void MsgUnifiedEditorView::setFocus(MsgUnifiedEditorBaseWidget* item)
{
    if(item)
    {
        item->setFocus();  
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::onContentChanged
//
//---------------------------------------------------------------
void MsgUnifiedEditorView::onContentChanged()
{
    mCanSaveToDrafts = true; 
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::onDialogDeleteMsg
//---------------------------------------------------------------
void MsgUnifiedEditorView::onDialogDeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {

        mCanSaveToDrafts = false;

        //delete if draft entry opened
        if (mOpenedMessageId.getId() != -1) {
            if (!mPluginLoader) {
                mPluginLoader = new UniEditorPluginLoader(this);
            }

            UniEditorPluginInterface* pluginInterface = mPluginLoader->getUniEditorPlugin(
                MsgUnifiedEditorMonitor::messageType());

            pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }

        //trigger back action.
        HbAction* backAction = this->navigationAction();
        if (backAction) {
            backAction->trigger();
        }

    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::onDialogSmsSettings
//---------------------------------------------------------------
void MsgUnifiedEditorView::onDialogSmsSettings(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        
        QVariantList params;
        params << MsgBaseView::MSGSETTINGS;// target view
        params << MsgBaseView::UNIEDITOR; // source view
        params << MsgSettingsView::SMSView;
        emit switchView(params);
    
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::onDialogMmsSettings
//---------------------------------------------------------------
void MsgUnifiedEditorView::onDialogMmsSettings(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        
        QVariantList params;
        params << MsgBaseView::MSGSETTINGS;// target view
        params << MsgBaseView::UNIEDITOR; // source view
        params << MsgSettingsView::MMSView;
        emit switchView(params); 
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::enableSendButton
// @see header file
//--------------------------------------------------------------
void MsgUnifiedEditorView::enableSendButton(bool enable)
    {
    if(mSendAction)
        {
         // enable/disable based on only if its disabled/enabled.
         // this check is to avoid unnecessary calls to mSendAction->setEnabled(enable);
        if(mSendAction->isEnabled() != enable )
            mSendAction->setEnabled(enable);
        }
    }

// ----------------------------------------------------------------------------
// MsgUnifiedEditorView::formatAddresses
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorView::formatAddresses(
        ConvergedMessageAddressList& addresses)
{       

    for(int i=0; i < addresses.count() ;i++ )
    {
        QString addr = addresses[i]->address();
        
        HBufC *tempAddr = XQConversions::qStringToS60Desc(addr);     
            
        TPtr ptr = tempAddr->Des();
                    
         // Note: This is just to parse spaces etc away from phonenumbers.
         //       Ignore EFalse returned for email addresses.   
        CommonPhoneParser::ParsePhoneNumber(ptr , 
                                            CommonPhoneParser::ESMSNumber );        
       
        addr = XQConversions::s60DescToQString(tempAddr->Des()); 
        
        addresses[i]->setAddress(addr);
        
        delete tempAddr;                                                       
    }       
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorView::isReplyPathBroken
// @see header
// ----------------------------------------------------------------------------
bool MsgUnifiedEditorView::isReplyPathBroken()
{
    // 1. Never set for MMS
    // 2. if additional recipients exits
    if( (MsgUnifiedEditorMonitor::messageType() == ConvergedMessage::Mms) ||
        (mToField->addressCount() != 1) )
    {
        // broken
        return true;
    }

    // 3. if only recipient is not same as originating SME
    QString dispName;
    int phCount;
    int origCntLocalId = MsgContactHandler::resolveContactDisplayName(
            mOriginatingSME, dispName, phCount);
    int currCntLocalId = -1;
    QString currAddress(mToField->addresses().at(0)->address());
    if(origCntLocalId != -1)
    {
        currCntLocalId = MsgContactHandler::resolveContactDisplayName(
            currAddress, dispName, phCount);
    }

    if(currCntLocalId != -1)
    { // both are mapped contacts present in contacts db
        if(currCntLocalId != origCntLocalId)
        {
            return true;
        }
    }
    else
    { // atleast one contact is not present in contacts db
      // direct compare
        UniEditorGenUtils* genUtils = q_check_ptr(new UniEditorGenUtils);
        bool compareResult = false;
        TRAP_IGNORE(
        compareResult = genUtils->MatchPhoneNumberL(
                *XQConversions::qStringToS60Desc(mOriginatingSME),
                *XQConversions::qStringToS60Desc(currAddress))
        );
        delete genUtils;
        if(!compareResult)
        {
            return true;
        }
    }

    return false;
}

//EOF
