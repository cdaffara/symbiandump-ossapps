/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Message editor view
*
*/
#include "nmuiheaders.h"

// Layout file and view
static const char *NMUI_EDITOR_VIEW_XML = ":/docml/nmeditorview.docml";
static const char *NMUI_EDITOR_VIEW = "editorview";
static const char *NMUI_EDITOR_VIEW_ATTACHMENT_EXTENSION =
    "attachmentTypeToolbarExtension";

// extension list item frame.
static const QString NmPopupListFrame = "qtg_fr_popup_list_normal";
static const QString NmDelimiter("; ");
static const QString NmPriorityLow("low");
static const QString NmPriorityNormal("normal");
static const QString NmPriorityHigh("high");

static const QString NmMimeTypeHtml("text/html");


/*!
	\class NmEditorView
	\brief Mail editor view
*/


/*!
    Class constructor.
*/
NmEditorView::NmEditorView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmAttachmentManager &attaManager,
    QGraphicsItem *parent)
: NmBaseView(startParam, application, parent),
  mApplication(application),
  mUiEngine(uiEngine),
  mAttaManager(attaManager),
  mDocumentLoader(NULL),
  mHeaderWidget(NULL),
  mMessage(NULL),
  mContent(NULL),
  mAttachmentListContextMenu(NULL),
  mMessageCreationOperation(NULL),
  mAddAttachmentOperation(NULL),
  mRemoveAttachmentOperation(NULL),
  mWaitDialog(NULL),
  mQueryDialog(NULL),
  mAttachmentPicker(NULL),
  mCcBccFieldVisible(false),
  mServiceSendingDialog(NULL),
  mTBExtnContentWidget(NULL),
  mHiddenPriorityName(NmPriorityNormal)
{
    NM_FUNCTION;

    mDocumentLoader = new NmUiDocumentLoader(mApplication.mainWindow());

    // Set the object name.
    setObjectName(QString(NMUI_EDITOR_VIEW));

    // Load the view layout.
    loadViewLayout();

    // Create the toolbar.
    createToolBar();
}


/*!
    Class destructor.
*/
NmEditorView::~NmEditorView()
{
    NM_FUNCTION;

    // Delete opened temporary files.
    NmUtilities::deleteTempFiles(mTempFiles);
    
    if (mRemoveAttachmentOperation && mRemoveAttachmentOperation->isRunning()) {
        mRemoveAttachmentOperation->cancelOperation();
    }

    if (mAddAttachmentOperation && mAddAttachmentOperation->isRunning()) {
        mAddAttachmentOperation->cancelOperation();
    }

    if (mMessageCreationOperation && mMessageCreationOperation->isRunning()) {
        mMessageCreationOperation->cancelOperation();
    }

    delete mMessage;
    mWidgetList.clear();
    delete mDocumentLoader;

    if (mAttachmentListContextMenu) {
        mAttachmentListContextMenu->clearActions();
        delete mAttachmentListContextMenu;
    }

    delete mWaitDialog;
    delete mQueryDialog;
    delete mAttachmentPicker;
    mAttaManager.clearObserver();
    mAttaManager.cancelFetch();

    // make sure virtual keyboard is closed
    QInputContext *ic = qApp->inputContext();

    if (ic) {
        QEvent *closeEvent = new QEvent(QEvent::CloseSoftwareInputPanel);
        ic->filterEvent(closeEvent);
        delete closeEvent;
    }

    if (!mTBExtnContentWidget->parent()) {
        delete mTBExtnContentWidget;
    }
}


/*!
    Loads the view layout from XML.
*/
void NmEditorView::loadViewLayout()
{
    NM_FUNCTION;
    
    // Use the document loader to load the view.
    QObjectList objectList;
    objectList.append(this);

    // Pass the view to documentloader. Document loader uses this view when
    // docml is parsed, instead of creating new view. The document loader is
    // created in the constructor.
    mDocumentLoader->setObjectTree(objectList);

    bool ok(false);
    mWidgetList = mDocumentLoader->load(NMUI_EDITOR_VIEW_XML, &ok);

   if (ok) {
       mContent = new NmEditorContent(this, mDocumentLoader, 
                                      mApplication.networkAccessManager(),
                                      mApplication);

       mHeaderWidget = mContent->header();

       // Set default color for user - entered text if editor is in re/reAll/fw
       // mode.
       if (mStartParam) {
           NmUiEditorStartMode mode = mStartParam->editorStartMode();

           if (mode == NmUiEditorReply ||
               mode == NmUiEditorReplyAll || 
               mode == NmUiEditorForward) {
               mContent->editor()->setCustomTextColor(true, Qt::blue);
           }
       }

       // Get the toolbar extension for adding attachments.
       mTBExtnContentWidget = qobject_cast<HbListWidget *>(
           mDocumentLoader->findWidget(NMUI_EDITOR_VIEW_ATTACHMENT_EXTENSION));

       // The rest of the view initialization is done in viewReady().
    }
}


/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view
*/
void NmEditorView::reloadViewContents(NmUiStartParam* startParam)
{
    NM_FUNCTION;

    // Check start parameter validity.
    if (startParam&&startParam->viewId()==NmUiViewMessageEditor) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam=NULL;
        // Store new start parameter data
        mStartParam=startParam;
        // Store existing edited message to drafts and reload
        // editor with new start parameters.
        // ..
        // Reload editor with new message data
        fetchMessageIfNeeded(*mStartParam);
    }
    else {
        NM_ERROR(1,"nmailui: Invalid editor start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
        startParam = NULL;
    }
}

/*!
   Screen orientation changed. Editor view needs to be scaled when
   landscape <-> portrait switch occurs because text needs to
   be wrapped again.
*/
void NmEditorView::orientationChanged(Qt::Orientation orientation)
{
    NM_FUNCTION;

    Q_UNUSED(orientation);

    // content widget height needs to be set according to the new orientation to get the scroll
	// area work correctly
	mHeaderWidget->sendDelayedHeaderHeightChanged();
	mHeaderWidget->adjustHeaderWidth();
}

/*!
    This slot is signaled by VKB when it opens
 */
void NmEditorView::vkbOpened()
{
    showChrome(false);
}

/*!
    This slot is signaled by VKB when it closes.
 */
void NmEditorView::vkbClosed()
{
	showChrome(true);
}

/*!
    Hide or show chrome.
 */
void NmEditorView::showChrome(bool show)
{
    if (show) {
        setContentFullScreen(false);
        showItems(Hb::ToolBarItem);
    }
    else {
        setContentFullScreen(true);
        hideItems(Hb::ToolBarItem);
    }
}

/*!
    View id
*/
NmUiViewId NmEditorView::nmailViewId() const
{
    NM_FUNCTION;

    return NmUiViewMessageEditor;
}

/*
   Launch dialog for query user if we want to exit the editor
*/
void NmEditorView::okToExitView()
{
    NM_FUNCTION;

    bool okToExit(true);

    if (mContent) {
        NmEditorHeader *header = mContent->header();
        // show the query if the message has not been sent
        if (mMessage && header) {
            // see if editor has any content
            int subjectLength = 0;
            if (header->subjectEdit()) {
                subjectLength = header->subjectEdit()->text().length();
            }

            QList<NmMessagePart*> attachmentList;
            mMessage->attachmentList(attachmentList);

            if (mContent->editor()) {
                okToExit = (subjectLength == 0 && mContent->editor()->document()->isEmpty());
            }

            // content exists, verify exit from user
            if (!okToExit) {
                if (mQueryDialog) {
                    delete mQueryDialog;
                    mQueryDialog = 0;
                }
                // Launch query dialog.
                mQueryDialog =
                    NmUtilities::displayQuestionNote(hbTrId("txt_mail_dialog_save_message_to_drafts"),
                                                                this,
                                                                SLOT(okToExitQuery(HbAction*)));
            }
        }
    }

    // no need to query anything, just exit.
    if(okToExit) {
        QMetaObject::invokeMethod(&mApplication,
                                  "popView",
                                  Qt::QueuedConnection);
    }
}

/*!
    Handle the user selection is it ok to exit.
*/
void NmEditorView::okToExitQuery(HbAction *action)
{
    NM_FUNCTION;

    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    // The first action in dialogs action list is for the "Yes"-button.
    if (action == dlg->actions().at(0)) {
        safeToDraft();
    }

    // Close the view
    QMetaObject::invokeMethod(&mApplication,
                              "popView",
                              Qt::QueuedConnection);
}

/*!
    Public slot to handle draft saving.
*/
void NmEditorView::safeToDraft()
{
    // Update draft message with content.
    updateMessageWithEditorContents();

    // Save message to drafts
    QList<NmOperation *> preliminaryOperations;
    if (mAddAttachmentOperation && mAddAttachmentOperation->isRunning()) {
        preliminaryOperations.append(mAddAttachmentOperation);
    }
    if (mRemoveAttachmentOperation && mRemoveAttachmentOperation->isRunning()) {
        preliminaryOperations.append(mRemoveAttachmentOperation);
    }
    // ownership of mMessage is transferred
    // NmOperations are automatically deleted after completion
    mUiEngine.saveDraftMessage(mMessage, preliminaryOperations);
    mMessage = NULL;
    preliminaryOperations.clear();
}

/*!
    About to exit view. Application calls this function when user has
    pressed back key and editor needs to delete the draft message. This is
    called when "auto-exiting" after a successful mail sending.
*/
void NmEditorView::aboutToExitView()
{
    NM_FUNCTION;

    // These operations need to be stopped before message can be deleted
    if (mAddAttachmentOperation && mAddAttachmentOperation->isRunning()) {
        mAddAttachmentOperation->cancelOperation();
    }
    if (mRemoveAttachmentOperation && mRemoveAttachmentOperation->isRunning()) {
        mRemoveAttachmentOperation->cancelOperation();
    }

    if (mMessage) { // this is NULL if sending or saving is started
        // Delete message from drafts
        mUiEngine.removeDraftMessage(mMessage);
        mMessage = NULL;
    }
}

/*!
    Lazy loading when view layout has been loaded
*/
void NmEditorView::viewReady()
{
    NM_FUNCTION;

    // Set mailbox name to title pane
    setMailboxName();

    // Connect options menu about to show to create options menu function
    // Menu needs to be create "just-in-time"
    connect(menu(), SIGNAL(aboutToShow()), this, SLOT(createOptionsMenu()));
    NmAction *dummy = new NmAction(0);
    menu()->addAction(dummy);

    mVkbHost = new HbShrinkingVkbHost(this);

    initializeVKB();

    //start to listen VKB open and close signals for hiding the chrome.
    connect(mVkbHost, SIGNAL(keypadOpened()), this, SLOT(vkbOpened()));
    connect(mVkbHost, SIGNAL(keypadClosed()), this, SLOT(vkbClosed()));

    connect(mContent->header(), SIGNAL(recipientFieldsHaveContent(bool)),
            this, SLOT(setButtonsDimming(bool)) );

    // Connect to observe orientation change events
    connect(mApplication.mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(orientationChanged(Qt::Orientation)));
    // Signal for handling the attachment list selection
    connect(mHeaderWidget, SIGNAL(attachmentShortPressed(NmId)),
            this, SLOT(openAttachmentTriggered(NmId)));
    connect(mHeaderWidget, SIGNAL(attachmentLongPressed(NmId, QPointF)),
            this, SLOT(attachmentLongPressed(NmId, QPointF)));

    if (mStartParam) {
        fetchMessageIfNeeded(*mStartParam);
    }
    else { // execution cannot proceed without start param
        QMetaObject::invokeMethod(&mApplication, "popView", Qt::QueuedConnection);
    }

    // Adjust the header width according to the current screen width.
    mHeaderWidget->adjustHeaderWidth();
}

/*!
    If entering editor for forwarding or replying, use attachment manager
    to check that we have all message parts fetched. Also show dialog for
    fetching progress.
*/
void NmEditorView::fetchMessageIfNeeded(NmUiStartParam &startParam)
{
    NM_FUNCTION;

    if (startParam.editorStartMode() == NmUiEditorForward
        || startParam.editorStartMode()== NmUiEditorReply
        || startParam.editorStartMode() == NmUiEditorReplyAll) {

        fetchProgressDialogShow();
        mAttaManager.clearObserver();
        mAttaManager.setObserver(this);
        mAttaManager.fetchAllMessageParts(
            startParam.mailboxId(),
            startParam.folderId(),
            startParam.messageId());
    }
    else {
        startMessageCreation(startParam);
    }
}

/*!
    Slot. Called when attachments fetch progress changes.
*/
void NmEditorView::progressChanged(int value)
{
    NM_FUNCTION;

    Q_UNUSED(value);
}

/*!
    Slot. Called when attachments fetch is completed. We can start
    message creation.
*/
void NmEditorView::fetchCompleted(int result)
{
    NM_FUNCTION;

    if (result == NmNoError && mStartParam) {
        startMessageCreation(*mStartParam);
    }
    else {
        // Close "Loading mail content" dialog
        mWaitDialog->close();

        // Show a fetching failed note when the failure is not caused by a Device/System failure.
        if (result != NmNoError &&
            result != NmNotFoundError &&
            result != NmGeneralError &&
            result != NmCancelError &&
            result != NmAuthenticationError &&
            result != NmServerConnectionError &&
            result != NmConnectionError) {
            HbNotificationDialog *note = new HbNotificationDialog();
            bool enalbeAttribute(true);
            note->setAttribute(Qt::WA_DeleteOnClose, enalbeAttribute);
            note->setIcon(HbIcon(QLatin1String("note_warning")));
            note->setTitle(hbTrId("txt_mail_dpopinfo_loading_failed"));
            note->setTitleTextWrapping(Hb::TextWordWrap);
            note->setDismissPolicy(HbNotificationDialog::TapAnywhere);
            note->setTimeout(HbNotificationDialog::StandardTimeout);
            note->setSequentialShow(true);
            note->show();
        }

        // Go back to Viewer view
        QMetaObject::invokeMethod(&mApplication, "popView", Qt::QueuedConnection);
    }
}

void NmEditorView::fetchProgressDialogShow()
{
    NM_FUNCTION;

    delete mWaitDialog;
    mWaitDialog = NULL;
    // Create new wait dialog and set it to me modal with dimmed background
    mWaitDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
    mWaitDialog->setModal(true);
    mWaitDialog->setBackgroundFaded(true);
    connect(mWaitDialog, SIGNAL(cancelled()), this, SLOT(fetchProgressDialogCancelled()));
    mWaitDialog->setText(hbTrId("txt_mail_dialog_loading_mail_content"));
    // Display wait dialog
    mWaitDialog->show();
}

/*!
    This is called by mFetchProgressDialog when the note is cancelled
 */
void NmEditorView::fetchProgressDialogCancelled()
{
    NM_FUNCTION;

    if (mAttaManager.isFetching()) {
        mAttaManager.cancelFetch();
        mAttaManager.clearObserver();
    }
    else {
        // For those email has no attachment or attachment has fetched.
        // Go back to Viewer view.
        QMetaObject::invokeMethod(&mApplication, "popView", Qt::QueuedConnection);
    }
}

void NmEditorView::startMessageCreation(NmUiStartParam &startParam)
{
    NM_FUNCTION;
    NM_TIMESTAMP("Start editor.");
    
    NmUiEditorStartMode startMode = startParam.editorStartMode();
    NmId mailboxId = startParam.mailboxId();
    NmId folderId = startParam.folderId();
    NmId msgId = startParam.messageId();

    if (mMessageCreationOperation && mMessageCreationOperation->isRunning()) {
        mMessageCreationOperation->cancelOperation();
    }

    // original message is now fetched so start message creation
    if (startMode == NmUiEditorForward) {
        mMessageCreationOperation = mUiEngine.createForwardMessage(mailboxId, msgId);
    }
    else if (startMode == NmUiEditorReply || startMode == NmUiEditorReplyAll) {
        mMessageCreationOperation = mUiEngine.createReplyMessage(mailboxId,
            msgId,
            startMode == NmUiEditorReplyAll);
    }
    else if (startMode == NmUiEditorFromDrafts) {
        // Draft opened, so reload message and fill editor with message data.
        mMessage = mUiEngine.message(
            mStartParam->mailboxId(),
            mStartParam->folderId(),
            mStartParam->messageId());
        fillEditorWithMessageContents();
    }
    else {
        mMessageCreationOperation = mUiEngine.createNewMessage(mailboxId);
    }

    // operation continues in messageCreated() once it finishes ok
    if (mMessageCreationOperation) {
        connect(mMessageCreationOperation,
                SIGNAL(operationCompleted(int)),
                this,
                SLOT(messageCreated(int)));
    }
    
    // Set focus
    if (mContent && mContent->header() ) {
        if (startMode == NmUiEditorCreateNew || XQServiceUtil::isEmbedded()) {
            if (mContent->header()->toEdit()) {
                mContent->header()->toEdit()->setFocus(Qt::OtherFocusReason);
            }
        }
        else
        {
            if (mContent->editor()) {
                mContent->editor()->setFocus(Qt::OtherFocusReason);
            }
        }
    }

    // Show VKB
    QInputContext *ic = qApp->inputContext();

    if(ic) {
        QEvent *event = new QEvent(QEvent::RequestSoftwareInputPanel);
        ic->filterEvent(event);
        delete event;
    }
}

/*!
    Starting the message sending is handled here.
*/
void NmEditorView::startSending()
{
    NM_FUNCTION;

    // The message contents should be verified
    updateMessageWithEditorContents();

    // verify addresses before sending
    QList<NmAddress> invalidAddresses;
    if (mMessage) {
        NmUtilities::getRecipientsFromMessage(*mMessage, invalidAddresses, NmUtilities::NmInvalidAddress);
    }

    if (invalidAddresses.count() > 0) {

        // Invalid addresses found, verify send from user.
        // Set the first failing address to the note.
        QString noteText =
            HbParameterLengthLimiter(
                "txt_mail_dialog_invalid_mail_address_send"
                ).arg(invalidAddresses.at(0).address());

        if (mQueryDialog) {
            delete mQueryDialog;
            mQueryDialog = 0;
        }
        // Launch query dialog. Pressing "yes" will finalize the sending.
        mQueryDialog = NmUtilities::displayQuestionNote(noteText,
                                                        this,
                                                        SLOT(invalidAddressQuery(HbAction*)));
    }
    else {
        // no need to ask anything, just send
        finalizeSending();
    }
}

/*!
    Send the message after all checks have been done.
*/
void NmEditorView::finalizeSending()
{
    NM_FUNCTION;

    QList<NmOperation *> preliminaryOperations;
    if (mAddAttachmentOperation && mAddAttachmentOperation->isRunning()) {
        preliminaryOperations.append(mAddAttachmentOperation);
    }
    if (mRemoveAttachmentOperation && mRemoveAttachmentOperation->isRunning()) {
        preliminaryOperations.append(mRemoveAttachmentOperation);
    }
    // ownership of mMessage is transferred
    // NmOperations are automatically deleted after completion
    mUiEngine.sendMessage(mMessage, preliminaryOperations);
    mMessage = NULL;
    preliminaryOperations.clear();

    bool service = XQServiceUtil::isService();

    // If sending is started as a service, progress dialog needs to be shown
    // so long that sending is finished otherwise we can close pop current view.
    if (service && mStartParam && mStartParam->service() &&
        mUiEngine.isSendingMessage()) {
        connect(&mUiEngine, SIGNAL(sendOperationCompleted()),
            this, SLOT(handleSendOperationCompleted()), Qt::UniqueConnection);

        // Construct and setup the wait dialog.
        mServiceSendingDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
        mServiceSendingDialog->setAttribute(Qt::WA_DeleteOnClose);
        mServiceSendingDialog->setText(hbTrId("txt_mail_shareui_sending_please_wait"));
        connect(mServiceSendingDialog, SIGNAL(cancelled()),
            this, SLOT(sendProgressDialogCancelled()));

        if (!XQServiceUtil::isEmbedded()) {
            // Hide the application.
            XQServiceUtil::toBackground(true);
        }
         // Display the wait dialog.
         mServiceSendingDialog->setModal(true);
         mServiceSendingDialog->setBackgroundFaded(true);
         mServiceSendingDialog->show();
    } else {
        // Must use delayed editor view destruction so that query dialog
        // (which has signaled this) gets time to complete.
        QMetaObject::invokeMethod(&mApplication, "popView", Qt::QueuedConnection);
    }
}

/*!
    Handle the user selection for invalid address query which was started by startSending.
*/
void NmEditorView::invalidAddressQuery(HbAction* action)
{
    NM_FUNCTION;

    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    // The first action in dialogs action list is for the "Yes"-button.
    if (action == dlg->actions().at(0)) {
        finalizeSending();
    }
}

/*!
    This is called when the view's geometry size has been changed, eg. when VKB is opened/closed.
*/
void NmEditorView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    NM_FUNCTION;

    NmBaseView::resizeEvent(event);

    emit sizeChanged();
}

/*!
    This is signalled by mMessageCreationOperation when message is created.
*/
void NmEditorView::messageCreated(int result)
{
    NM_FUNCTION;
    NM_TIMESTAMP("Editor opened.");
    
    delete mMessage;
    mMessage = NULL;

    // Close wait dialog here
    if (mWaitDialog) {
        mWaitDialog->close();
    }

    if (result == NmNoError && mStartParam && mMessageCreationOperation) {
        NmUiEditorStartMode startMode = mStartParam->editorStartMode();

        // get message "again" from engine to update the message contents
        mMessage = mUiEngine.message(
            mStartParam->mailboxId(),
            mStartParam->folderId(),
            mMessageCreationOperation->getMessageId());

        fillEditorWithMessageContents();
    }
}

/*!
   Updates the message with the editor contents.
*/
void NmEditorView::updateMessageWithEditorContents()
{
    NM_FUNCTION;

    if (mMessage) {
        if (mContent && mContent->editor()) {
            NmMessagePart* bodyPart = mMessage->htmlBodyPart();
            if (bodyPart) {
                bodyPart->setTextContent(mContent->editor()->toHtml(), NmContentTypeTextHtml);
            }
            bodyPart = mMessage->plainTextBodyPart();
            if (bodyPart) {
                bodyPart->setTextContent(mContent->editor()->toPlainText(), NmContentTypeTextPlain);
            }
        }
        if (mContent && mContent->header() ) {
            if (mContent->header()->subjectEdit()) {
                mMessage->envelope().setSubject(mContent->header()->subjectEdit()->text());
            }
            if (mContent->header()->toEdit()) {
                mMessage->envelope().setToRecipients(
                    mContent->header()->toEdit()->emailAddressList());
            }
            if (mContent->header()->ccEdit()) {
                mMessage->envelope().setCcRecipients(
                    mContent->header()->ccEdit()->emailAddressList());
            }
            if (mContent->header()->bccEdit()) {
                mMessage->envelope().setBccRecipients(
                    mContent->header()->bccEdit()->emailAddressList());
            }
        }
    }
}


/*!
    Updates the message with the editor contents. Called only once when the
    editor is launched.
*/
void NmEditorView::fillEditorWithMessageContents()
{
    NM_FUNCTION;

    if (!mStartParam || !mMessage || !mContent) {
        return;
    }

    NmMessageEnvelope messageEnvelope(mMessage->envelope());
    bool useStartParam(false);

    NmUiEditorStartMode editorStartMode = mStartParam->editorStartMode();

    if (editorStartMode == NmUiEditorMailto) {
        // Retrieve the message header data e.g. recipients from mStartParam.
        useStartParam = true;
    }

    // Set recipients (to, cc and bcc).
    QString toAddressesString;
    QString ccAddressesString;
    QString bccAddressesString;

    if (useStartParam) {
        QList<NmAddress *> toAddressList = mStartParam->mailtoAddressList();
        QList<NmAddress *> ccAddressList = mStartParam->ccAddressList();
        QList<NmAddress *> bccAddressList = mStartParam->bccAddressList();
        toAddressesString = addressListToString(toAddressList);
        ccAddressesString = addressListToString(ccAddressList);
        bccAddressesString = addressListToString(bccAddressList);
        // Also add recipients added for example from send service 
        // interface to recipient line edits.        
        mContent->header()->toEdit()->addContacts(toAddressList);
        mContent->header()->ccEdit()->addContacts(ccAddressList);
        mContent->header()->bccEdit()->addContacts(bccAddressList);
    }
    else {
        toAddressesString = addressListToString(messageEnvelope.toRecipients());
        ccAddressesString = addressListToString(messageEnvelope.ccRecipients());
        bccAddressesString = addressListToString(messageEnvelope.bccRecipients());
    }

    mContent->header()->toEdit()->setPlainText(toAddressesString);
    mContent->header()->ccEdit()->setPlainText(ccAddressesString);
    mContent->header()->bccEdit()->setPlainText(bccAddressesString);

    if (ccAddressesString.length() || bccAddressesString.length()) {
        // Since cc or/and bcc recipients exist, expand the group box to display
        // the addresses by expanding the group box.
        mCcBccFieldVisible = true;
        mHeaderWidget->setFieldVisibility(mCcBccFieldVisible);
    }

    // Set subject.
    if (useStartParam) {
        QString *subject = mStartParam->subject();

        if (subject) {
            mContent->header()->subjectEdit()->setPlainText(*subject);
        }
    }
    else {
        // Construct the subject field.
        mContent->header()->subjectEdit()->setPlainText(
            addSubjectPrefix(editorStartMode, messageEnvelope.subject()));
    }

    // Set priority.
    if (editorStartMode==NmUiEditorReply || editorStartMode==NmUiEditorReplyAll) {
        //Clear the importance flag. Replied messages dont keep the importance
        setPriority(NmActionResponseCommandNone);
    }
    mHeaderWidget->setPriority(messageEnvelope.priority());

    NmMessage *originalMessage = NULL;

    // Set the message body.
    if (editorStartMode==NmUiEditorReply||
        editorStartMode==NmUiEditorReplyAll||
        editorStartMode==NmUiEditorForward||
        editorStartMode==NmUiEditorFromDrafts){

        // Use the body from the original message.
        originalMessage = mUiEngine.message(mStartParam->mailboxId(),
                                            mStartParam->folderId(),
                                            mStartParam->messageId());

        if (originalMessage) {
            NmMessagePart *plainPart = originalMessage->plainTextBodyPart();

            if (plainPart) {
                mUiEngine.contentToMessagePart(originalMessage->envelope().mailboxId(),
                                               originalMessage->envelope().folderId(),
                                               originalMessage->envelope().messageId(),
                                               *plainPart);
            }

            NmMessagePart *htmlPart = originalMessage->htmlBodyPart();

            if (htmlPart) {
                mUiEngine.contentToMessagePart(originalMessage->envelope().mailboxId(),
                                               originalMessage->envelope().folderId(),
                                               originalMessage->envelope().messageId(),
                                               *htmlPart);
            }

        }

    }

    QString *signature = NULL;
    // return value is not relevant here
    mUiEngine.getSignature(mStartParam->mailboxId(), signature);

    mContent->setBodyContent(editorStartMode, originalMessage, signature);

    delete signature;
    signature = NULL;

    delete originalMessage;
    originalMessage = NULL;

    // Get list of attachments from the message and set those into UI attachment list
    QList<NmMessagePart*> attachments;
    mMessage->attachmentList(attachments);

    for (int i=0; i<attachments.count(); i++) {
        mHeaderWidget->addAttachment(
            attachments[i]->attachmentName(),
            QString::number(attachments[i]->size()),
            attachments[i]->partId());
    }

    // Attach passed files to the message.
    QStringList *fileList = mStartParam->attachmentList();

    if (fileList) {
        addAttachments(*fileList);
    }

    if (mStartParam->bodyText().length()) {
        // If the content is not HTML, use it as plain text
        if (mStartParam->bodyMimeType().compare(NmMimeTypeHtml, Qt::CaseInsensitive)) {
			mContent->editor()->setPlainText(mStartParam->bodyText());
		}
		else {
		    mContent->editor()->setHtml(mStartParam->bodyText());
		}
    }
}


/*!
    Constructs the toolbar according to the actions received from the extension
    manager.
*/
void NmEditorView::createToolBar()
{
    NM_FUNCTION;
    
    HbToolBar *tb = toolBar();
    NmUiExtensionManager &extMngr = mApplication.extManager();

    if (!tb || !mStartParam) {
        return;
    }

    tb->clearActions();

    NmActionRequest request(this,
                            NmActionToolbar,
                            NmActionContextViewEditor,
                            NmActionContextDataNone,
                            mStartParam->mailboxId(),
                            mStartParam->folderId()); 
    QList<NmAction *> list;
    extMngr.getActions(request, list);

    for (int i = 0; i < list.count(); i++) {
        tb->addAction(list[i]);

        // If action has NmSendable condition, it is shown only when send
        // action is available, i.e. when at least one recipient field has
        // data.
        if (list[i]->availabilityCondition() == NmAction::NmSendable) {
            list[i]->setEnabled(false);
        }
        else if (list[i]->availabilityCondition() == NmAction::NmAttachable &&
                 mTBExtnContentWidget) {
            // Setup the toolbar extension for attachments.
            HbToolBarExtension* extension = new HbToolBarExtension();
            extension->setParent(this);
            list[i]->setToolBarExtension(extension);

            mTBExtnContentWidget->addItem(hbTrId("txt_mail_list_photo"));
            mTBExtnContentWidget->addItem(hbTrId("txt_mail_list_sound"));
            mTBExtnContentWidget->addItem(hbTrId("txt_mail_list_video"));
            mTBExtnContentWidget->addItem(hbTrId("txt_mail_list_other"));
            mTBExtnContentWidget->addItem(hbTrId("txt_mail_list_new_photo"));
            mTBExtnContentWidget->addItem(hbTrId("txt_mail_list_new_video"));

            HbListViewItem *listView = mTBExtnContentWidget->listItemPrototype();
            HbFrameBackground frame(NmPopupListFrame, HbFrameDrawer::NinePieces);
            listView->setDefaultFrame(frame);
            
            extension->setContentWidget(mTBExtnContentWidget);

            connect(mTBExtnContentWidget, SIGNAL(activated(HbListWidgetItem*)),
                    extension, SLOT(close()));

            mAttachmentPicker = new NmAttachmentPicker(this);
            
            connect(mAttachmentPicker, SIGNAL(attachmentsFetchOk(const QVariant &)),
                    this, SLOT(onAttachmentReqCompleted(const QVariant &)));
            connect(this, SIGNAL(titleChanged(QString)),
                    mAttachmentPicker, SLOT(setTitle(QString)));
            connect(mTBExtnContentWidget, SIGNAL(activated(HbListWidgetItem*)), 
                    mAttachmentPicker, SLOT (selectFetcher(HbListWidgetItem*)));
        }
    } // for ()        
}


/*!
    createOptionsMenu. Functions asks menu commands from extension
    to be added to options menu.
*/
void NmEditorView::createOptionsMenu()
{
    NM_FUNCTION;

    menu()->clearActions();

	// Create CC/BCC options menu object
    if (mCcBccFieldVisible) {
        menu()->addAction(hbTrId("txt_mail_opt_hide_cc_bcc"), this, SLOT(switchCcBccFieldVisibility()));
    }
    else {
        menu()->addAction(hbTrId("txt_mail_opt_show_cc_bcc"), this, SLOT(switchCcBccFieldVisibility()));
    }

	// Create Priority options menu object
    HbMenu *prioritySubMenu = new HbMenu();
    NmActionRequest request(this, NmActionOptionsMenu, NmActionContextViewEditor,
            NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
            mStartParam->messageId());
    NmUiExtensionManager &extMngr = mApplication.extManager();
    QList<NmAction*> list;
    extMngr.getActions(request, list);
    for (int i = 0; i < list.count(); i++) {
        // check what priority has already been selected and hide it from options menu
        if (!list[i]->objectName().contains(mHiddenPriorityName)) {
	        prioritySubMenu->addAction(list[i]);
        }
    }
    prioritySubMenu->setObjectName("editorPrioritySubMenu");
    prioritySubMenu->setTitle(hbTrId("txt_mail_opt_add_priority"));
    menu()->addMenu(prioritySubMenu);
}

/*!
    Show or hide Cc field
*/
void NmEditorView::switchCcBccFieldVisibility()
{
    NM_FUNCTION;

    if (mCcBccFieldVisible) {
    	mCcBccFieldVisible = false;
    }
    else {
    	mCcBccFieldVisible = true;
    }
    mHeaderWidget->setFieldVisibility( mCcBccFieldVisible );
}

/*!
    handleActionCommand. From NmActionObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmEditorView::handleActionCommand(NmActionResponse &actionResponse)
{
    NM_FUNCTION;

    NmActionResponseCommand responseCommand = actionResponse.responseCommand();

    // Handle options menu
    if (actionResponse.menuType() == NmActionOptionsMenu) {
        setPriority(responseCommand);
    }
    else if (actionResponse.menuType() == NmActionToolbar) {
        switch (responseCommand) {
        case NmActionResponseCommandSendMail: {
            // Just in case send mail would be somehow accessible during message creation or
            // outobox checking
            if (!mMessageCreationOperation || !mMessageCreationOperation->isRunning()) {
                startSending();
            }
            break;
        }
        default:
            break;
        }
    }
    else if (actionResponse.menuType() == NmActionContextMenu) {
        switch (responseCommand) {
        case NmActionResponseCommandRemoveAttachment: {
            removeAttachmentTriggered();
            break;
        }
        case NmActionResponseCommandOpenAttachment: {
            openAttachmentTriggered(mSelectedAttachment);
            break;
        }
        default:
            break;
        }
    }
}

/*!
    Slot. Cancelled sending progress dialog.
*/
void NmEditorView::sendProgressDialogCancelled()
{
    // Must use delayed editor view destruction so that dialog
    // gets time to complete, closes also nmail.
    QMetaObject::invokeMethod(&mApplication, "popView", Qt::QueuedConnection);
}

/*!
   Sets all toolbar and VKB buttons dimmed state. All actions that have the
   availability condition NmSendable set, will be enabled/disabled.
*/
void NmEditorView::setButtonsDimming(bool enabled)
{
    NM_FUNCTION;

    // Set the toolbar action states
    HbToolBar *tb = toolBar();
    if (tb) {
        QList<QAction *> toolbarList = tb->actions();
        int count = toolbarList.count();
        for (int i = 0; i < count; i++) {
            NmAction *action = static_cast<NmAction *>(toolbarList[i]);
            if (action->availabilityCondition() == NmAction::NmSendable) {
                action->setEnabled(enabled);
            }
        }

        // Set the VKB action states
        // All editors of the view share the same action, so it is enough to set
        // this only to one of them.
        HbEditorInterface editorInterface(mContent->editor());
        QList<HbAction *> vkbList = editorInterface.actions();
        count = vkbList.count();
        for (int i = 0; i < count; i++) {
            NmAction *action = static_cast<NmAction *>(vkbList[i]);
            if (action->availabilityCondition() == NmAction::NmSendable) {
                action->setEnabled(enabled);
            }
        }
    }
}

/*!
    Initialize the Virtual Keyboard to show the "Send" button
    for all editors of the view.
*/
void NmEditorView::initializeVKB()
{
    NM_FUNCTION;

    if (!mStartParam) {
        return;
    }

    NmActionRequest request(this, NmActionVKB, NmActionContextViewEditor,
         NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
    NmUiExtensionManager &extMngr = mApplication.extManager();
    if (&extMngr) {
        QList<NmAction *> list;
        extMngr.getActions(request, list);

        // VKB only supports one application key, but the responsibility of giving only one
        // action is left to the extension plugin. The rest are still attached to the VKB, but
        // they are not shown (unless VKB starts supporting more than one).
        for (int i = 0; i < list.count(); i++) {
            if( list[i]->availabilityCondition() == NmAction::NmSendable ) {
                list[i]->setEnabled(false);
            }
            list[i]->setIcon(NmIcons::getIcon(NmIcons::NmIconSend));

            // Link VKB to the action. This must be done to all
            // editors that show the button in VKB.
            HbEditorInterface editorInterface(mContent->editor());
            editorInterface.addAction(list[i]);
            HbEditorInterface toEditorInterface(mContent->header()->toEdit());
            toEditorInterface.addAction(list[i]);
            HbEditorInterface ccEditorInterface(mContent->header()->ccEdit());
            ccEditorInterface.addAction(list[i]);
            HbEditorInterface bccEditorInterface(mContent->header()->bccEdit());
            bccEditorInterface.addAction(list[i]);
            HbEditorInterface subjectEditorInterface(mContent->header()->subjectEdit());
            subjectEditorInterface.addAction(list[i]);
        }
    }
}

/*!
    Set mailbox name to title
*/
void NmEditorView::setMailboxName()
{
    NM_FUNCTION;

    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta){
            setTitle(meta->name());
        }
    }
}

/*!
   Adds a prefix to the subject for reply or forward.
   Strips other occurrences of the prefix from the beginning.
*/
QString NmEditorView::addSubjectPrefix( NmUiEditorStartMode startMode, const QString &subject )
{
    NM_FUNCTION;

    QString newSubject(subject.trimmed());

    if (startMode == NmUiEditorReply || startMode == NmUiEditorReplyAll ||
        startMode == NmUiEditorForward) {
        QString rePrefix(hbTrId("txt_mail_reply_subject_prefix"));
        QString fwPrefix(hbTrId("txt_mail_forward_subject_prefix"));

        // strip extra prefixes from beginning
        int rePrefixLength(rePrefix.length());
        int fwPrefixLength(fwPrefix.length());

        bool startswithRe(newSubject.startsWith(rePrefix, Qt::CaseInsensitive));
        bool startswithFw(newSubject.startsWith(fwPrefix, Qt::CaseInsensitive));

        while (startswithRe || startswithFw) {
            if (startswithRe) {
                newSubject.remove(0,rePrefixLength);
                newSubject = newSubject.trimmed();
            }
            else if (startswithFw) {
                newSubject.remove(0,fwPrefixLength);
                newSubject = newSubject.trimmed();
            }
            startswithRe = newSubject.startsWith(rePrefix, Qt::CaseInsensitive);
            startswithFw = newSubject.startsWith(fwPrefix, Qt::CaseInsensitive);
        }

        if (startMode == NmUiEditorReply || startMode == NmUiEditorReplyAll) {
            newSubject = rePrefix + " " + newSubject;
        }
        else if (startMode == NmUiEditorForward) {
            newSubject = fwPrefix + " " + newSubject;
        }
    }

    return newSubject;
}


/*!
    This slot is called when 'attachment picker' request has been performed succesfully
    Parameter 'value' contains file currently one file name but later list of the files.
*/
void NmEditorView::onAttachmentReqCompleted(const QVariant &value)
{
    NM_FUNCTION;

    //temporary fix for music picker back button:
    //it shouldn't emit requestOk signal when nothing is selected
	if (value.canConvert<QStringList>()) {
	    QStringList list = value.toStringList();
        if (!list.at(0).isEmpty()) {
            addAttachments(list);
        }
    }
}

/*!
    This slot is called when 'attachment picker' request has been unsuccesfull
    Parameter 'errorCode' is the error code returned by the service
    Parameter 'errorMessage' is the error message returned by the service
*/
void NmEditorView::onAttachmentsFetchError(int errorCode, const QString& errorMessage)
{
    NM_FUNCTION;
    Q_UNUSED(errorCode);
    Q_UNUSED(errorMessage);
    NM_COMMENT(QString("Error code: %1").arg(errorCode));
    NM_COMMENT(QString("Error message: %1").arg(errorMessage));
}

/*!
    Closes the wait dialog if one exists.

    This slot is called if the mail application has been started as a service
    and is about to close. Closing the application while still sending a message
    may cause unwanted cancelling of the operation.
*/
void NmEditorView::handleSendOperationCompleted()
{
    NM_FUNCTION;

    if (mServiceSendingDialog) {
        mServiceSendingDialog->close();
    }
    // Must use delayed editor view destruction so that dialog
    // gets time to complete, closes also nmail.
    QMetaObject::invokeMethod(&mApplication, "popView", Qt::QueuedConnection);
}

/*!
    Add list of attachments
*/
void NmEditorView::addAttachments(const QStringList& fileNames)
{
    NM_FUNCTION;
    NM_TIMESTAMP("Add attachments.");
    
    // Add attachment name into UI
    foreach (QString fileName, fileNames)  {
        // At this phase attachment size and nmid are not known
        mHeaderWidget->addAttachment(fileName, QString("0"), NmId(0));
        NM_COMMENT(fileName);
    }
    //  Cancel previous operation if it's not running.
    if (mAddAttachmentOperation) {
        if (!mAddAttachmentOperation->isRunning()) {
            mAddAttachmentOperation->cancelOperation();
        }
    }
    // Start operation to attach file or list of files into mail message.
    // This will also copy files into message store.
    mAddAttachmentOperation = mUiEngine.addAttachments(*mMessage, fileNames);

    if (mAddAttachmentOperation) {
        enableToolBarAttach(false);
        // Signal to inform completion of one attachment
        connect(mAddAttachmentOperation,
                SIGNAL(operationPartCompleted(const QString &, const NmId &, int)),
                this,
                SLOT(oneAttachmentAdded(const QString &, const NmId &, int)));

        // Signal to inform the completion of the whole operation
        connect(mAddAttachmentOperation,
                SIGNAL(operationCompleted(int)),
                this,
                SLOT(allAttachmentsAdded(int)));
    }
}

/*!
    This slot is called to create context menu when attachment has been selected
    from UI by longpress.
*/
void NmEditorView::attachmentLongPressed(NmId attachmentPartId, QPointF point)
{
    NM_FUNCTION;

    // Store id of the attachment to be removed into member.
    // It is used by removeAttachmentTriggered later if 'remove' selected.
    mSelectedAttachment = attachmentPartId;

    if (!mAttachmentListContextMenu) {
        mAttachmentListContextMenu = new HbMenu();
    }
    mAttachmentListContextMenu->clearActions();
    NmActionRequest request(this, NmActionContextMenu, NmActionContextViewEditor,
        NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
        mStartParam->messageId());
    NmUiExtensionManager &extensionManager = mApplication.extManager();
    if (&extensionManager) {
        QList<NmAction*> actionList;
        extensionManager.getActions(request, actionList);
        for (int i = 0; i < actionList.count(); ++i) {
            mAttachmentListContextMenu->addAction(actionList[i]);
        }
    }

    // Add menu position check here, so that it does not go outside of the screen
    QPointF menuPos(point.x(),point.y());
    mAttachmentListContextMenu->setPreferredPos(menuPos);
    mAttachmentListContextMenu->open();
}

/*!
    This is signalled by mAddAttachmentOperation when the operation is
    completed for one attachment.
*/
void NmEditorView::oneAttachmentAdded(const QString &fileName, const NmId &msgPartId, int result)
{
    NM_FUNCTION;

    if (result == NmNoError && mMessage) {
        // Need to get the message again because new attachment part has been added.
        NmId mailboxId = mMessage->envelope().mailboxId();
        NmId folderId = mMessage->envelope().folderId();
        NmId msgId = mMessage->envelope().messageId();
        NmMessagePriority messagePriority = mMessage->envelope().priority();

        delete mMessage;
        mMessage = NULL;

        mMessage = mUiEngine.message(mailboxId, folderId, msgId);

        if (mMessage) {
            mMessage->envelope().setPriority(messagePriority);
            mMessage->envelope().setHasAttachments(true);
            // Get attachment list from the message
            QList<NmMessagePart*> attachmentList;
            mMessage->attachmentList(attachmentList);

            // Search newly added attachment from the list
            for (int i=0; i<attachmentList.count(); i++) {
                if (attachmentList[i]->partId() == msgPartId) {
                    // Get attachment file size and set it into UI
                    mHeaderWidget->setAttachmentParameters(fileName,
                        msgPartId,
                        QString().setNum(attachmentList[i]->size()),
                        result);
                }
            }
        }
    }
    else {
        // Attachment adding failed. Show an error note and remove from UI attachment list.
        NM_ERROR(1,QString("nmailui: attachment adding into message failed: %1").arg(fileName));
        mHeaderWidget->removeAttachment(fileName);
    }
}

/*!
    This is signalled by mAddAttachmentOperation when the operation is
    completed totally.
*/
void NmEditorView::allAttachmentsAdded(int result)
{
    NM_FUNCTION;
    NM_TIMESTAMP("All attachments added.");
    
    enableToolBarAttach(true);
    if (result != NmNoError) {
        NmUtilities::displayWarningNote(hbTrId("txt_mail_dialog_unable_to_add_attachment"));
    }
}

/*!
   Sets priority for the message object that is being edited
*/
void NmEditorView::setPriority(NmActionResponseCommand priority)
{
    NM_FUNCTION;

    mHeaderWidget->setPriority(priority);

    if (mMessage) {
        NmMessagePriority messagePriority = NmMessagePriorityNormal;
        mHiddenPriorityName = NmPriorityNormal;

        if (priority == NmActionResponseCommandPriorityHigh) {
            messagePriority = NmMessagePriorityHigh;
            mHiddenPriorityName = NmPriorityHigh;
        }
        else if (priority == NmActionResponseCommandPriorityLow) {
            messagePriority = NmMessagePriorityLow;
            mHiddenPriorityName = NmPriorityLow;
        }
        mMessage->envelope().setPriority(messagePriority);
    }
}


/*!
    Extracts the addresses from the given list into a string separated with a
    delimiter.

    \param list The list containing the addresses.
    \return String containing the addresses.
*/
QString NmEditorView::addressListToString(const QList<NmAddress*> &list) const
{
    NM_FUNCTION;

    QString addressesString;
    foreach (NmAddress *nmAddress,list) {
        if (nmAddress->displayName().length() > 0) {
            addressesString += nmAddress->displayName();
        }
        else {
            addressesString += nmAddress->address();
        }
        addressesString += NmDelimiter;
    }
    return addressesString;
}


/*!
    Extracts the addresses from the given list into a string separated with a
    delimiter.

    \param list The list containing the addresses.
    \return String containing the addresses.
*/
QString NmEditorView::addressListToString(const QList<NmAddress> &list) const
{
    NM_FUNCTION;

    QString addressesString;
    QList<NmAddress>::const_iterator i = list.constBegin();

    while (i != list.constEnd()) {
        if (i > list.constBegin()) {
            // Add the delimiter.
            addressesString += NmDelimiter;
        }

        addressesString += (*i).address();
        ++i;
    }

    return addressesString;
}

/*!
    This slot is called when 'remove' is selected from attachment list context menu.
*/
void NmEditorView::removeAttachmentTriggered()
{
    NM_FUNCTION;

    // Cancel will delete previous operation
    if (mRemoveAttachmentOperation) {
        if (!mRemoveAttachmentOperation->isRunning()) {
            mRemoveAttachmentOperation->cancelOperation();
        }
    }
    // Remove from UI
    mHeaderWidget->removeAttachment(mSelectedAttachment);
    // Remove from message store
    mRemoveAttachmentOperation = mUiEngine.removeAttachment(*mMessage, mSelectedAttachment);
    if (mRemoveAttachmentOperation) {
        // Signal to inform the remove operation completion
        connect(mRemoveAttachmentOperation,
                SIGNAL(operationCompleted(int)),
                this,
                SLOT(attachmentRemoved(int)));
    }
}

/*!
    This slot is called by mRemoveAttachmentOperation when the operation is
    completed. There is no need to update UI because it was already updated.
 */
void NmEditorView::attachmentRemoved(int result)
{
    NM_FUNCTION;

    // It is not desided yet what to do if operation fails
    Q_UNUSED(result);

    if (mMessage) {
        // Reload message because one attachment has been removed
        NmId mailboxId = mMessage->envelope().mailboxId();
        NmId folderId = mMessage->envelope().folderId();
        NmId msgId = mMessage->envelope().messageId();
        NmMessagePriority messagePriority = mMessage->envelope().priority();

        delete mMessage;
        mMessage = NULL;

        mMessage = mUiEngine.message(mailboxId, folderId, msgId);

        if(mMessage) {
            // Set the correct priority
            mMessage->envelope().setPriority(messagePriority);

            // If there is no more attachments in the message, set the correct value
            QList<NmMessagePart*> attachmentList;
            mMessage->attachmentList(attachmentList);
            if(attachmentList.count() == 0) {
                mMessage->envelope().setHasAttachments(false);
            }
        }
    }
}

/*!
    This slot is called when 'open' is selected from attachment list context menu.
*/
void NmEditorView::openAttachmentTriggered(NmId attachmentId)
{
    NM_FUNCTION;
    NmId mailboxId = mMessage->envelope().mailboxId();
    NmId folderId = mMessage->envelope().folderId();
    NmId msgId = mMessage->envelope().messageId();

    QList<NmMessagePart *> attachments;
    mMessage->attachmentList(attachments);
    foreach (NmMessagePart *part, attachments) {
        if (part->partId() == attachmentId) {
            // We need to fill the part before opening the attachment.
            mUiEngine.contentToMessagePart(mailboxId,folderId,msgId, *part);
            int error = NmUtilities::openAttachment(part,mTempFiles);
            if ( error == NmGeneralError ) {
                HbMessageBox *box = NmUtilities::displayWarningNote(hbTrId("txt_mail_dialog_unable_to_open_attachment_file_ty"));
                box->setAttribute(Qt::WA_DeleteOnClose);
            }
            break;
        }
    }
}

/*!
   Enables/disables toolbar extension for attach
*/
void NmEditorView::enableToolBarAttach(bool enable)
{
    NM_FUNCTION;

    HbToolBar *tb = toolBar();
    if (tb) {
        QList<QAction *> toolbarList = tb->actions();
        int count = toolbarList.count();
        for (int i = 0; i < count; i++) {
            NmAction *action = static_cast<NmAction *>(toolbarList[i]);
            if (action->availabilityCondition() == NmAction::NmAttachable) {
                action->setEnabled(enable);
                if (enable) {
                    // For some reason 'Add attachment' toolbar button stays dimmed sometimes,
                    // showItems will fix the situation.
                    showItems(Hb::ToolBarItem);
                }
            }
        }
    }
}
