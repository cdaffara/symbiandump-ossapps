/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiengineheaders.h"

/*!
 \class NmBaseClientPlugin

 \brief The NmBaseClientPlugin plugin provides ui services for IMAP/POP mail protocols.

 The NmBaseClientPlugin contains the following services:
 \li get actions
 \li refresh mailbox
 \li create new email
 \li open settings
 \li send email
 \li delete email
 \li open email
 \li reply email
 \li reply all email
 \li forward email
 \li search emails

 The plugin provides a list of supported UI services as NmAction objects. Actions are connected to
 different public slots and when they are triggered plugin will handle action by itself directly to
 NmUiEngine or by using NmActionResponse to inform user of service.

*/

/*!
    \fn void NmBaseClientPlugin::getActions(const NmActionRequest &request, QList<NmAction*> &actionList)
    Implementation of NmUiExtension interface. The plugin provides to caller supported actions for email protocol.
    \a request contains NmActionRequest with needed request data.
    \a actionList QList array will be filled by action supported protocol and controlled by NmActionRequest.
*/

/*!
    \fn void NmBaseClientPlugin::openMessage()
    When open action is triggered this slot is called to open message.
*/

/*!
    \fn void NmBaseClientPlugin::deleteMessage()
    When delete action is triggered this slot is called to delete message.
*/

/*!
    \fn void NmBaseClientPlugin::refresh()
    When refresh action is triggered this slot is called to start refreshing the mailbox.
*/

/*!
    \fn void NmBaseClientPlugin::createNewMail()
    When create new action is triggered this slot is called to notify responce observer
    with NmActionResponseCommandNewMail enum.
*/

/*!
    \fn void NmBaseClientPlugin::settings()
    When settings action is triggered this slot is called to open settings.
*/

/*!
    \fn void NmBaseClientPlugin::sendMail()
    When send mail action is triggered this slot is called to send an email.
*/

/*!
    \fn void NmBaseClientPlugin::replyMail()
    When reply mail action is triggered this slot is called to reply an email.
*/

/*!
    \fn void NmBaseClientPlugin::replyAllMail()
    When reply all mail action is triggered this slot is called to reply all an email.
*/

/*!
    \fn void NmBaseClientPlugin::forwardMail()
    When forward mail action is triggered this slot is called to forward an email.
*/

/*!
    Constructs a new NmBaseClientPlugin.
*/
NmBaseClientPlugin::NmBaseClientPlugin()
:mMenuRequest(NULL),
mUiEngine(NULL),
mEditorToolBarRequest(NULL),
mViewerToolBarRequest(NULL),
mViewerViewRequest(NULL)
{
    NM_FUNCTION;
    
    mUiEngine = NmUiEngine::instance();
}

/*!
    Destructor.
*/
NmBaseClientPlugin::~NmBaseClientPlugin()
{
    NM_FUNCTION;
    
    NmUiEngine::releaseInstance(mUiEngine);
    mUiEngine = NULL;
}


/*!
    Provides a list of supported NmActions.
    Implementation of NmUiExtensionInterface.

    Parameter \a request controls list of request services.
    Parameter \a actionList is updated by supported NmActions.
*/
void NmBaseClientPlugin::getActions(const NmActionRequest &request,
                                    QList<NmAction *> &actionList)
{
    NM_FUNCTION;
    
    if (request.observer()) {
        switch (request.contextView()) {
            case NmActionContextViewMessageList:
            case NmActionContextViewMessageSearchList:
            {
                createMessageListCommands(request, actionList);
                break;
            }
            case NmActionContextViewViewer:
            {
                createViewerViewCommands(request, actionList);
                break;
            }
            case NmActionContextViewEditor:
            {
                createEditorViewCommands(request, actionList);
                break;
            }
            default:
            {
                NM_COMMENT(QString("NmBaseClientPlugin::getActions(): unknown contextView()=%1").arg(request.contextView()));
                break;
            }
        }
    }
}


/*!
    Public slot connected to options menu refresh NmAction.
    Refreshes mailbox using the NmUiEngine instance.
*/
void NmBaseClientPlugin::refresh()
{
    NM_FUNCTION;
    
    int err = mUiEngine->refreshMailbox(mMenuRequest.mailboxId());

    if (NmNoError != err) {
        // Failed to refresh the mailbox!
        NM_ERROR(1,QString("NmBaseClientPlugin::refresh(): failed err=%1").arg(err));
    }
}

/*!
    Public slot connected to triggered() signal of open message action instance.
    The method sends an open response command to the action observer.
*/
void NmBaseClientPlugin::openMessage()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandOpen, mMenuRequest);
}

/*!
    Public slot connected to delete message. Method asks UI engine to delete messages
*/
void NmBaseClientPlugin::deleteMessage()
{
    NM_FUNCTION;
    
    QList<NmId> messageList;
    messageList.append(mMenuRequest.messageId());

    int err = mUiEngine->deleteMessages(mMenuRequest.mailboxId(),
                                        mMenuRequest.folderId(),
                                        messageList);
    if (NmNoError != err) {
        // Failed to delete the messages!
        NM_ERROR(1,QString("NmBaseClientPlugin::deleteMessage(): failed err=%1").arg(err));
    }
    messageList.clear();
}

/*!
    Public slot connected to triggered() signal of delete action instance.
    The method sends a delete mail command to the action observer.
*/
void NmBaseClientPlugin::deleteMessageFromViewerView()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandDeleteMail, mViewerViewRequest);
}

/*!
    Public slot connected to triggered() signal of create mail action instance.
    The method sends a new mail command to the action observer.
*/
void NmBaseClientPlugin::createNewMailViewerToolBar()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandNewMail, mViewerToolBarRequest);
}

/*!
    Public slot for handling the event of menu request for creating a new email.
    The method sends a new mail command to the action observer of the menu request.
*/
void NmBaseClientPlugin::createNewMail()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandNewMail, mMenuRequest);
}

/*!
    Public slot connected to options menu settings NmAction.
    Opens mailbox settings.
*/
void NmBaseClientPlugin::settings()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandSettings, mMenuRequest);
}

/*!
    Public slot connected to triggered() signal of send mail action instance.
    Sends the send mail command to the action observer.
*/
void NmBaseClientPlugin::sendMail()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandSendMail, mEditorToolBarRequest);
}

/*!
    Public slot connected to triggered() signal of reply action instance of the
    toolbar. Sends a reply mail command to the action observer.
*/

void NmBaseClientPlugin::replyMail()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandReply, mViewerViewRequest);
}

/*!
    Public slot connected to triggered signal of reply all action instance of
    the toolbar. Sends a reply all mail command to the action observer.
*/

void NmBaseClientPlugin::replyAllMail()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandReplyAll, mViewerViewRequest);
}

/*!
    Public slot connected to triggered() signal of forward action instance of
    the toolbar. Sends a forward mail command to the action observer.
*/
void NmBaseClientPlugin::forwardMail()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandForward, mViewerViewRequest);
}

/*!
    Public slot connected to set high priority toolbar action.
*/
void NmBaseClientPlugin::setPriorityHigh()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandPriorityHigh, mMenuRequest);
}

/*!
    Public slot connected to set normal priority toolbar action.
*/
void NmBaseClientPlugin::setPriorityNormal()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandNone, mMenuRequest);
}

/*!
    Public slot connected to set low priority toolbar action.
*/
void NmBaseClientPlugin::setPriorityLow()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandPriorityLow, mMenuRequest);
}

/*!
    Public slot connected to attach toolbar action.
*/
void NmBaseClientPlugin::attach()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandAttach, mEditorToolBarRequest);
}

/*!
    Public slot connected to remove attachment context menu action
*/
void NmBaseClientPlugin::removeAttachment()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandRemoveAttachment, mMenuRequest);
}

/*!
    Public slot connected to open attachment context menu action
*/
void NmBaseClientPlugin::openAttachment()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandOpenAttachment, mMenuRequest);
}

/*!
    Requests the handling of the search action.
    This public slot gets called when the search button in a toolbar is clicked.
*/
void NmBaseClientPlugin::search()
{
    NM_FUNCTION;
    
    handleRequest(NmActionResponseCommandSearch, mViewerToolBarRequest);
}

/*!
    Private slot connected to settings view launcher.

    \param mailboxId Id of mailbox.
    \param type type of change.
*/
void NmBaseClientPlugin::mailboxListChanged(const NmId &mailboxId,
    NmSettings::MailboxEventType type)
{
    NM_FUNCTION;
    
    Q_UNUSED(mailboxId)
    Q_UNUSED(type)
    handleRequest(NmActionResponseCommandMailboxDeleted, mMenuRequest);
}

/*!
    Private slot connected to settings view launcher.

    \param mailboxId Id of mailbox.
    \param property
    \param value
*/
void NmBaseClientPlugin::mailboxPropertyChanged(const NmId &mailboxId, QVariant property,
    QVariant value)
{
    NM_FUNCTION;
    
    Q_UNUSED(property)
    Q_UNUSED(value)

    NmActionObserver *observer = mMenuRequest.observer();
    if (observer) {
        // Force model item to be updated, because framework adapter sends it too slowly.
        mUiEngine->mailboxListModel().refreshModelItem(mailboxId);

        // Notify view of changes.
        NmActionResponse response(NmActionResponseCommandUpdateMailboxName, mMenuRequest);
        observer->handleActionCommand(response);
    }
}

/*!
    Private slot connected to settings view launcher.
    \param mailboxId Id of mailbox.
*/
void NmBaseClientPlugin::goOnline(const NmId &mailboxId)
{
    NM_FUNCTION;
    
		(void) mUiEngine->refreshMailbox(mailboxId);
}
/*!
    Private slot connected to settings view launcher.
    \param mailboxId Id of mailbox.
*/
void NmBaseClientPlugin::goOffline(const NmId &mailboxId)
{
    NM_FUNCTION;
    
        mUiEngine->goOffline(mailboxId);
}

/*!
    Create messagelist command actions.
    Internal helper method.
    Parameter \a request is used to identify requested menu type.
    Parameter \a actionList is updated by supported NmActions for message list.
*/
void NmBaseClientPlugin::createMessageListCommands(
    const NmActionRequest &request,
    QList<NmAction *> &actionList)
{
    NM_FUNCTION;
    
    switch (request.menuType()) {
        case NmActionOptionsMenu:
        {
            mMenuRequest = request;

            NmAction *refreshAction = new NmAction(0);
            refreshAction->setObjectName("baseclientplugin_refresh");
            refreshAction->setText(hbTrId("txt_mail_opt_send_and_receive_now"));
            connect(refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));
            actionList.append(refreshAction);

            NmAction *settingsAction = new NmAction(0);
            settingsAction->setObjectName("baseclientplugin_settings");
            settingsAction->setText(hbTrId("txt_common_opt_settings"));
            connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings()));
            actionList.append(settingsAction);
            break;
        }
        case NmActionToolbar:
        {
            // Create the search button.
            NmAction *mailSearchAction = new NmAction(0);
            mailSearchAction->setObjectName("baseclientplugin_mailsearchaction");
            mailSearchAction->setIcon(HbIcon("qtg_mono_search"));
            connect(mailSearchAction, SIGNAL(triggered()), this, SLOT(search()));
            actionList.append(mailSearchAction);

            // Create new mail button.
            mViewerToolBarRequest = request;
            NmAction *createMailAction = new NmAction(0);
            createMailAction->setObjectName("baseclientplugin_createmailaction");
            createMailAction->setIcon(NmIcons::getIcon(NmIcons::NmIconNewEmail));
            connect(createMailAction, SIGNAL(triggered()),
                    this, SLOT(createNewMailViewerToolBar()));
            actionList.append(createMailAction);
            break;
        }
        case NmActionContextMenu:
        {
            if (request.contextDataType() == NmActionContextDataMessage){
                mMenuRequest = request;

                NmFolderType folderType(NmFolderInbox);
                if (mUiEngine){
                    folderType = mUiEngine->folderTypeById(request.mailboxId(),request.folderId());                
                }
                
                if (folderType!=NmFolderOutbox){
                    // Open message
                    NmAction* openAction = new NmAction(0);
                    openAction->setObjectName("baseclientplugin_openaction");
                    openAction->setText(hbTrId("txt_common_menu_open"));
                    connect(openAction, SIGNAL(triggered()), this, SLOT(openMessage()));
                    actionList.append(openAction);                
                }                

                // Delete message
                NmAction* deleteAction = new NmAction(0);
                deleteAction->setObjectName("baseclientplugin_deleteaction");
                deleteAction->setText(hbTrId("txt_common_menu_delete"));
                connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteMessage()));
                actionList.append(deleteAction);

                if (folderType!=NmFolderOutbox &&
                    folderType!=NmFolderDrafts ){
                    NmMessageEnvelope *envelope =
                        request.requestData().value<NmMessageEnvelope*>();    
                    if (envelope){
                        if (envelope->isRead()){
                            NM_COMMENT("nmailui: envelope is read");
                            NmAction* unreadAction = new NmAction(0);
                            unreadAction->setObjectName("baseclientplugin_unreadaction");
                            unreadAction->setText(hbTrId("txt_mail_menu_mark_as_unread"));
    
                            connect(unreadAction,
                                SIGNAL(triggered()),
                                this,
                                SLOT(markAsUnread()));
    
                            actionList.append(unreadAction);
                        }
                        else {
                            NM_COMMENT("nmailui: envelope is unread");
                            NmAction* readAction = new NmAction(0);
                            readAction->setObjectName("baseclientplugin_readaction");
                            readAction->setText(hbTrId("txt_mail_menu_mark_as_read"));
    
                            connect(readAction,
                                SIGNAL(triggered()),
                                this,
                                SLOT(markAsRead()));
    
                            actionList.append(readAction);
                        }
                    }
                }
            }
            break;
        }
        default:
        {
            NM_COMMENT(QString("NmBaseClientPlugin::createMessageListCommands(): unknown menuType()=%1").arg(request.menuType()));
            break;
        }
    }
}

/*!
    Create Viewer view command actions.
    Internal helper method.
    Parameter \a request is used to identify requested menu type.
    Parameter \a actionList is updated by supported NmActions for message list.
*/
void NmBaseClientPlugin::createViewerViewCommands(
    const NmActionRequest &request,
    QList<NmAction *> &actionList)
{
    NM_FUNCTION;
    
    switch (request.menuType()) {
    	case NmActionOptionsMenu:
		{
			mViewerViewRequest = request;

			// Options menu Reply action
			NmAction *replyAction = new NmAction(0);
			replyAction->setObjectName("baseclientplugin_reply");
			replyAction->setText(hbTrId("txt_mail_viewer_opt_reply"));
			connect(replyAction, SIGNAL(triggered()), this, SLOT(replyMail()));
			actionList.append(replyAction);

			// Options menu Reply all action
			NmAction *replyAllAction = new NmAction(0);
			replyAllAction->setObjectName("baseclientplugin_reply_all");
			replyAllAction->setText(hbTrId("txt_mail_viewer_opt_reply_all"));
			connect(replyAllAction, SIGNAL(triggered()), this, SLOT(replyAllMail()));
			actionList.append(replyAllAction);

			// Options menu Forward action
			NmAction *forwardAction = new NmAction(0);
			forwardAction->setObjectName("baseclientplugin_forward");
			forwardAction->setText(hbTrId("txt_mail_viewer_opt_forward"));
			connect(forwardAction, SIGNAL(triggered()), this, SLOT(forwardMail()));
			actionList.append(forwardAction);

			// Options menu Delete action
			NmAction *deleteAction = new NmAction(0);
			deleteAction->setObjectName("baseclientplugin_delete");
			deleteAction->setText(hbTrId("txt_mail_viewer_opt_delete"));
			connect(deleteAction, SIGNAL(triggered()),
					this, SLOT(deleteMessageFromViewerView()));
			actionList.append(deleteAction);
			break;
		}
        case NmActionToolbar:
        {
            mViewerViewRequest = request;

            // ToolBar Reply action
            NmAction *replyAction = new NmAction(0);
            replyAction->setToolTip(hbTrId("txt_mail_button_reply"));
            replyAction->setIcon(NmIcons::getIcon(NmIcons::NmIconReply));
            replyAction->setObjectName("baseclientplugin_reply");
            connect(replyAction, SIGNAL(triggered()), this, SLOT(replyMail()));
            actionList.append(replyAction);

            // ToolBar Reply all action
            NmAction *replyAllAction = new NmAction(0);
            replyAllAction->setToolTip(hbTrId("txt_mail_button_reply_all"));
            replyAllAction->setIcon(NmIcons::getIcon(NmIcons::NmIconReplyAll));
            replyAllAction->setObjectName("baseclientplugin_reply_all");
            connect(replyAllAction, SIGNAL(triggered()), this, SLOT(replyAllMail()));
            actionList.append(replyAllAction);

            // ToolBar Forward action
            NmAction *forwardAction = new NmAction(0);
            forwardAction->setToolTip(hbTrId("txt_mail_button_forward"));
            forwardAction->setIcon(NmIcons::getIcon(NmIcons::NmIconForward));
            forwardAction->setObjectName("baseclientplugin_forward");
            connect(forwardAction, SIGNAL(triggered()), this, SLOT(forwardMail()));
            actionList.append(forwardAction);

            // ToolBar Delete action
            NmAction *deleteAction = new NmAction(0);
            deleteAction->setToolTip(hbTrId("txt_mail_button_delete"));
            deleteAction->setIcon(NmIcons::getIcon(NmIcons::NmIconDelete));
            deleteAction->setObjectName("baseclientplugin_delete");
            connect(deleteAction, SIGNAL(triggered()),
                    this, SLOT(deleteMessageFromViewerView()));
            actionList.append(deleteAction);
            break;
        }
        default:
        {
            NM_COMMENT(QString("NmBaseClientPlugin::createViewerViewCommands(): unknown menuType()=%1").arg(request.menuType()));
            break;
        }
    }

}

/*!
    Create Editor view command actions.
    Internal helper method.
    Parameter \a request is used to identify requested menu type.
    Parameter \a actionList is updated by supported NmActions for message list.
*/
void NmBaseClientPlugin::createEditorViewCommands(
    const NmActionRequest &request,
    QList<NmAction *> &actionList)
{
    NM_FUNCTION;
    
    switch (request.menuType()) {
        case NmActionToolbar:
        {
            mEditorToolBarRequest = request;

            // ToolBar Attach action
            NmAction *attachAction = new NmAction(0);
            attachAction->setObjectName("baseclientplugin_attachaction");
            attachAction->setIcon(NmIcons::getIcon(NmIcons::NmIconAttach));
            // Action only available when attachment can be added
            attachAction->setAvailabilityCondition(NmAction::NmAttachable);
            // connect action/add toolbar extensions
            connect(attachAction, SIGNAL(triggered()), this, SLOT(attach()));
            actionList.append(attachAction);

            // ToolBar Send action
            NmAction *sendAction = new NmAction(0);
            sendAction->setObjectName("baseclientplugin_sendaction");
            sendAction->setIcon(NmIcons::getIcon(NmIcons::NmIconSend));
            // Action only available when the message related to it can be sent.
            sendAction->setAvailabilityCondition(NmAction::NmSendable);
            connect(sendAction, SIGNAL(triggered()), this, SLOT(sendMail()));
            actionList.append(sendAction);
            break;
        }
        case NmActionOptionsMenu:
        {
            mMenuRequest = request;

            NmAction *action = new NmAction(0);
            action->setObjectName("baseclientplugin_highpriorityaction");
            action->setText(hbTrId("txt_mail_opt_add_priority_sub_high"));
            connect(action, SIGNAL(triggered()), this, SLOT(setPriorityHigh()));
            actionList.append(action);

            action = new NmAction(0);
            action->setObjectName("baseclientplugin_normalpriorityaction");
            action->setText(hbTrId("txt_mail_opt_add_priority_sub_normal"));
            connect(action, SIGNAL(triggered()), this, SLOT(setPriorityNormal()));
            actionList.append(action);

            action = new NmAction(0);
            action->setObjectName("baseclientplugin_lowpriorityaction");
            action->setText(hbTrId("txt_mail_opt_add_priority_sub_low"));
            connect(action, SIGNAL(triggered()), this, SLOT(setPriorityLow()));
            actionList.append(action);
            break;
        }
        case NmActionContextMenu:
        {
            mMenuRequest = request;

            NmAction *action = new NmAction(0);
            action->setObjectName("baseclientplugin_removeattachmentaction");
            action->setText(hbTrId("txt_common_menu_remove"));
            connect(action, SIGNAL(triggered()), this, SLOT(removeAttachment()));
            actionList.append(action);

            action = new NmAction(0);
            action->setObjectName("baseclientplugin_openattachmentaction");
            action->setText(hbTrId("txt_common_menu_open"));
            connect(action, SIGNAL(triggered()), this, SLOT(openAttachment()));
            actionList.append(action);
            break;
        }
        case NmActionVKB:
        {
            mMenuRequest = request;

            // Virtual Keyboard Send action
            NmAction *sendAction = new NmAction(0);
            sendAction->setObjectName("baseclientplugin_vblsendaction");
            sendAction->setText(hbTrId("txt_common_button_send"));
            // Action only available when the message related to it can be sent.
            sendAction->setAvailabilityCondition(NmAction::NmSendable);
            connect(sendAction, SIGNAL(triggered()), this, SLOT(sendMail()));
            actionList.append(sendAction);
            break;
        }
        default:
        {
            NM_COMMENT(QString("NmBaseClientPlugin::createEditorViewCommands(): unknown menuType()=%1").arg(request.menuType()));
            break;
        }
    }
}

/*!
    Marks the message as read.
*/
void NmBaseClientPlugin::markAsRead()
{
    NM_FUNCTION;
    
    updateEnvelopeProperty(MarkAsRead);
}

/*!
    Marks the message as unread.
*/
void NmBaseClientPlugin::markAsUnread()
{
    NM_FUNCTION;
    
    updateEnvelopeProperty(MarkAsUnread);
}

/*!
    Handles requests.
*/
void NmBaseClientPlugin::handleRequest(NmActionResponseCommand command, const NmActionRequest &request)
{
    NM_FUNCTION;
    
    NmActionObserver *observer = request.observer();
    if (observer) {
        NmActionResponse response(command, request);
        observer->handleActionCommand(response);
    }
}
/*!
    Store envelope property.
*/
void NmBaseClientPlugin::updateEnvelopeProperty(NmEnvelopeProperties property)
{
    NM_FUNCTION;
    
    QList<const NmMessageEnvelope*> envelopeList;
    NmMessageEnvelope *envelope =
            mMenuRequest.requestData().value<NmMessageEnvelope*>();

    if (envelope) {
        envelopeList.append(envelope);

        QPointer<NmStoreEnvelopesOperation> op =
                mUiEngine->setEnvelopes(mMenuRequest.mailboxId(),
                                        mMenuRequest.folderId(),
                                        property,
                                        envelopeList);
    }
    envelopeList.clear();
}
