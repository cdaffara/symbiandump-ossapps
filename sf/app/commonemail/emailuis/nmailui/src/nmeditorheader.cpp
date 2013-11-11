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
* Description: Message editor header container class. Collects the header widgets.
*
*/

#include "nmuiheaders.h"

// Layout
// These match to the defintions in nmeditorview.docml
static const char *NMUI_EDITOR_CONTAINER = "scrollAreaContents";
static const char *NMUI_EDITOR_SUBJECT_FIELD = "editorSubjectField";
static const char *NMUI_EDITOR_SUBJECT_EDIT = "editorSubjectEdit";
static const char *NMUI_EDITOR_SUBJECT_LABEL = "editorSubjectLabel";
static const char *NMUI_EDITOR_TO_FIELD = "editorToField";
static const char *NMUI_EDITOR_CC_FIELD = "editorCcField";
static const char *NMUI_EDITOR_BCC_FIELD = "editorBccField";
static const char *NMUI_EDITOR_PRIORITY_ICON = "editPriorityIcon";
static const char *NMUI_EDITOR_FOLLOWUP_ICON = "editFollowUpIcon";
static const char *NMUI_EDITOR_ATTACHMENT_LIST = "attachmentListWidget";
static const char *NMUI_EDITOR_PREFIX_TO = "editorTo";
static const char *NMUI_EDITOR_PREFIX_CC = "editorCc";
static const char *NMUI_EDITOR_PREFIX_BCC = "editorBcc";

static const int NmMaxRows = 10000;

// this timeout seems to be long enough for all cases. see sendDelayedHeaderHeightChanged
static const int NmLayoutSystemWaitTimer = 500; // 0.5 sec

/*!
    Constructor
*/
NmEditorHeader::NmEditorHeader(
    QObject *parent, NmApplication &application, HbDocumentLoader *documentLoader) :
    QObject(parent),
    mApplication(application),
    mDocumentLoader(documentLoader),
    mHeaderHeight(0),
    mIconVisible(false),
    mSubjectEdit(NULL),
    mRecipientFieldsEmpty(true),
    mAttachmentList(NULL),
    mToField(NULL),
    mCcField(NULL),
    mBccField(NULL),
    mCcBccFieldVisible(false)
{
    NM_FUNCTION;
    
    loadWidgets();
    createConnections();
}

/*!
    Destructor
*/
NmEditorHeader::~NmEditorHeader()
{
    NM_FUNCTION;
}

/*!
    Load widgets from XML for the header.
*/
void NmEditorHeader::loadWidgets()
{
    NM_FUNCTION;
	
    // Load widgets from docml and construct handlers. Those widgets that are not shown by default
    // are hidden and removed from the layout at this phase.    
    HbWidget *contentWidget =
        qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_CONTAINER));
    if (contentWidget) {
        mLayout = static_cast<QGraphicsLinearLayout *>(contentWidget->layout());
    
        // base class QObject takes the deleting responsibility
        mToField = new NmRecipientField(this, *mDocumentLoader, NMUI_EDITOR_PREFIX_TO);
        mCcField = new NmRecipientField(this, *mDocumentLoader, NMUI_EDITOR_PREFIX_CC);
        mBccField = new NmRecipientField(this, *mDocumentLoader, NMUI_EDITOR_PREFIX_BCC);
        // Connection for adjusting recipient field labels. Connection done earlier than other connections
        // so that we don't miss signal.
        connect(mToField->editor(), SIGNAL(layoutChanged ()), this, SLOT(toFieldLayoutReady()));
    
        // Sets up editor properties like no prediction, lower case preferred etc.
        HbEditorInterface toEditorInterface(mToField->editor());
        HbEditorInterface ccEditorInterface(mCcField->editor());
        HbEditorInterface bccEditorInterface(mBccField->editor());
        toEditorInterface.setUpAsLatinAlphabetOnlyEditor();
        ccEditorInterface.setUpAsLatinAlphabetOnlyEditor();
        bccEditorInterface.setUpAsLatinAlphabetOnlyEditor();

        mToWidget = qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_TO_FIELD));
        
        // Cc field is not shown by default. It needs to be both hidden and removed from the layout.
        mCcWidget = qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_CC_FIELD));
        mCcWidget->hide();
        mLayout->removeItem(mCcWidget);
          
        // Bcc field is not shown by default. It needs to be both hidden and removed from the layout.
        mBccWidget = qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_BCC_FIELD));
        mBccWidget->hide();
        mLayout->removeItem(mBccWidget);
    
        mSubjectWidget =
            qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_FIELD));
        mSubjectLayout = static_cast<QGraphicsLinearLayout *>(mSubjectWidget->layout());
        mSubjectLabel =
            qobject_cast<HbLabel *>(mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_LABEL));
          
        // Add Subject: field
        mSubjectEdit = qobject_cast<NmHtmlLineEdit *>
            (mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_EDIT));
        mSubjectEdit->setMaxRows(NmMaxRows);
    
        // Add attachment list
        mAttachmentListWidget = qobject_cast<NmAttachmentListWidget *>
            (mDocumentLoader->findWidget(NMUI_EDITOR_ATTACHMENT_LIST));
        // Create attachment list handling object
        mAttachmentList = new NmAttachmentList(*mAttachmentListWidget);
        mAttachmentList->setParent(this); // ownership changes
        mAttachmentListWidget->hide();
        mLayout->removeItem(&mAttachmentList->listWidget());
    
        // Add priority icon
        mPriorityIcon = qobject_cast<HbLabel *>
            (mDocumentLoader->findWidget(NMUI_EDITOR_PRIORITY_ICON));
        mPriorityIcon->hide();
        mSubjectLayout->removeItem(mPriorityIcon);
    
        // follow-up icon is not yet supported
        HbLabel *followUpIcon = qobject_cast<HbLabel *>
            (mDocumentLoader->findWidget(NMUI_EDITOR_FOLLOWUP_ICON));
        followUpIcon->hide();
        mSubjectLayout->removeItem(followUpIcon);    
    }
}

/*!
    Create signal - slot connections.
*/
void NmEditorHeader::createConnections()
{
    NM_FUNCTION;
    
    // Signals for checking if the recipient fields have text.
    connect(mToField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));

    // Signals for handling the recipient field expanding
    connect(mToField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendDelayedHeaderHeightChanged()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendDelayedHeaderHeightChanged()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendDelayedHeaderHeightChanged()));
    connect(mSubjectEdit, SIGNAL(contentsChanged()), this, SLOT(sendDelayedHeaderHeightChanged()));

    // Signals for handling the attachment list
    connect(&mAttachmentList->listWidget(), SIGNAL(itemActivated(int)),
            this, SLOT(attachmentActivated(int)));
    connect(&mAttachmentList->listWidget(), SIGNAL(longPressed(int, QPointF)),
            this, SLOT(attachmentLongPressed(int, QPointF)));
}

/*!
   Show or hide recipient field
*/
void NmEditorHeader::setFieldVisibility(bool isVisible)
{
	if ( mCcBccFieldVisible != isVisible ) {
		mCcBccFieldVisible = isVisible;
		if (mCcBccFieldVisible) {
            mLayout->insertItem(1, mBccWidget);
			mLayout->insertItem(1, mCcWidget);
			mCcWidget->show();
            mBccWidget->show();
		}
		else {
            mCcWidget->hide();
            mBccWidget->hide();
			mLayout->removeItem(mCcWidget);
			mLayout->removeItem(mBccWidget);
		}
        sendDelayedHeaderHeightChanged();
	}
}

/*!
    Return the sum of the header widget heights. This contains all the spacings a well. 
 */
qreal NmEditorHeader::headerHeight() const
{
    NM_FUNCTION;

    // get the layout's vertical spacing
    qreal spacing = 0;
    HbInstance::instance()->style()->parameter("hb-param-margin-gene-middle-vertical", spacing);

    // calculate the height
    qreal height = 0;
    
    height += mToField->height(); // returns widget's geometry height
    height += spacing;
    
    if (mCcBccFieldVisible) {
        height += mCcField->height(); // returns widget's geometry height
        height += spacing;
        
        height += mBccField->height(); // returns widget's geometry height
        height += spacing;
    }

    height += mSubjectWidget->geometry().height();
    height += spacing;

    if (mAttachmentList->listWidget().isVisible()) {
        height += mAttachmentList->listWidget().geometry().height();
        height += spacing;
    }

    return height;
}

/*!
    This is called when the contents of some of the header widgets have been changed. When the
    contents change the widget's actual size may also change. The header area height is needed to
    calculate the size hints of the body and the scroll area widgets. We need to use a timer to let 
    the Orbit FW adjust the heights eg. if the subject and recipient fields are expanded by the FW.
    It would be best to find a solution which doesn't depend on the header area's actual height size
    information.
 */
void NmEditorHeader::sendDelayedHeaderHeightChanged()
{
    NM_FUNCTION;
	QTimer::singleShot(NmLayoutSystemWaitTimer, this, SLOT(sendHeaderHeightChanged()));
}

/*!
    Send a signal that the header area height has been changed if necessary. This is needed for the
    body and scroll area widgets. See NmEditorTextEdit::setHeaderHeight for more info.
 */
void NmEditorHeader::sendHeaderHeightChanged()
{
    qreal hHeight = headerHeight();
    if (mHeaderHeight != hHeight) {
        mHeaderHeight = hHeight;
        emit headerHeightChanged(mHeaderHeight);
    }
}

/*!
    Because header filds have fixed size policy. This function must be called to set
    new width for every header field when orintation has been changed.
 */
void NmEditorHeader::adjustHeaderWidth()
{
    NM_FUNCTION;
    
    int newWidth = mApplication.screenSize().width();
    mToWidget->setPreferredWidth(newWidth);
    mCcWidget->setPreferredWidth(newWidth);
    mBccWidget->setPreferredWidth(newWidth);
    mSubjectWidget->setPreferredWidth(newWidth);
    mAttachmentListWidget->setPreferredWidth(newWidth);
    mSubjectWidget->setPreferredWidth(newWidth);
}

/*!
    Return pointer to to edit
 */
NmRecipientLineEdit* NmEditorHeader::toEdit() const
{
    NM_FUNCTION;
    
    return mToField->editor();
}

/*!
    Return pointer to cc edit
 */
NmRecipientLineEdit* NmEditorHeader::ccEdit() const
{
    NM_FUNCTION;
    
    return mCcField->editor();
}

/*!
    Return pointer to bcc edit
 */
NmRecipientLineEdit* NmEditorHeader::bccEdit() const
{
    NM_FUNCTION;
    
    return mBccField->editor();
}

/*!
    Return pointer to subject field
 */
NmHtmlLineEdit* NmEditorHeader::subjectEdit() const
{
    NM_FUNCTION;
    
    return mSubjectEdit;
}

/*!
    Checks if recipient editors are empty and emits a signal if
    the state is changed.
*/
void NmEditorHeader::editorContentChanged()
{
    NM_FUNCTION;
    
    bool recipientsFieldsEmpty(true);
    if (mToField->text().length()) {
        recipientsFieldsEmpty = false;
    }
    else if (mCcField->text().length()) {
        recipientsFieldsEmpty = false;
    }
    else if (mBccField->text().length()) {
        recipientsFieldsEmpty = false;
    }
    if (mRecipientFieldsEmpty != recipientsFieldsEmpty) {
        mRecipientFieldsEmpty = recipientsFieldsEmpty;
        emit recipientFieldsHaveContent(!mRecipientFieldsEmpty);
    }
}

/*!
    Sets the icon for priority
 */
void NmEditorHeader::setPriority(NmMessagePriority priority)
{
    NM_FUNCTION;
    
    switch (priority) {
    case NmMessagePriorityHigh:
        setPriority(NmActionResponseCommandPriorityHigh);
        break;
    case NmMessagePriorityLow:
        setPriority(NmActionResponseCommandPriorityLow);
        break;
    default:
        setPriority(NmActionResponseCommandNone);
        break;
    }
}

/*!
    Sets the icon for priority
 */
void NmEditorHeader::setPriority(NmActionResponseCommand prio)
{
    NM_FUNCTION;
    
    switch(prio) {
    case NmActionResponseCommandPriorityHigh:
        if (!mIconVisible) {
            mIconVisible = true;
            // icon widget is just after the subject line edit (see docml)
            mSubjectLayout->insertItem(2, mPriorityIcon);
            mPriorityIcon->show();
        }
        mPriorityIcon->setIcon(
            NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
        break;
    case NmActionResponseCommandPriorityLow:
        if (!mIconVisible) {
            mIconVisible = true;
            // icon widget is just after the subject line edit (see docml)
            mSubjectLayout->insertItem(2, mPriorityIcon);
            mPriorityIcon->show();
        }
        mPriorityIcon->setIcon(
            NmIcons::getIcon(NmIcons::NmIconPriorityLow));
        break;
    default:
        if (mIconVisible) {
            mIconVisible = false;
            HbIcon emptyIcon;
            mPriorityIcon->setIcon(emptyIcon);
            mSubjectLayout->removeItem(mPriorityIcon);
            mPriorityIcon->hide();
        }
        break;
    }
    sendDelayedHeaderHeightChanged();
}

/*!
   Adds an attachment to attachment list for mail.
 */
void NmEditorHeader::addAttachment(
    const QString &fileName, const QString &fileSize, const NmId &nmid)
{
    NM_FUNCTION;
    
    mAttachmentList->insertAttachment(fileName, fileSize, nmid);
    
    if (!mAttachmentList->listWidget().isVisible()) {
        // attachment list is inserted just before the body widget (see docml).
        mLayout->insertItem(mLayout->count() - 1, &mAttachmentList->listWidget());
        mAttachmentList->listWidget().show();
    }

    sendHeaderHeightChanged();
}

/*!
   Remove attachment from the list. This function is used when
   attachment adding has failed and attachment id is not known.
 */
void NmEditorHeader::removeAttachment(const QString &fileName)
{
    NM_FUNCTION;
    
    mAttachmentList->removeAttachment(fileName);
    if (mAttachmentList->count() == 0) {
        mAttachmentList->listWidget().hide();
        mLayout->removeItem(&mAttachmentList->listWidget());
    }
    sendDelayedHeaderHeightChanged();
}

/*!
   Remove attachment from the list. This function is used when
   attachment has been selected for remove.
 */
void NmEditorHeader::removeAttachment(const NmId &nmid)
{
    NM_FUNCTION;
    
    mAttachmentList->removeAttachment(nmid);
    if (mAttachmentList->count() == 0) {
        mAttachmentList->listWidget().hide();
        mLayout->removeItem(&mAttachmentList->listWidget());
    }
    sendDelayedHeaderHeightChanged();
}

/*!
    This function set messagePartId and fileSize for attachment.
 */
void NmEditorHeader::setAttachmentParameters(
    const QString &fileName,
    const NmId &msgPartId,
    const QString &fileSize,
    int result)
{
    NM_FUNCTION;
    
    if (result == NmNoError) {
        // Attachment adding succesful, set message part id and size for attachment
        mAttachmentList->setAttachmentPartId(fileName, msgPartId);
        mAttachmentList->setAttachmentSize(msgPartId, fileSize);
    }
}

/*!
   Slot attachment activated from attachment list by short tap.
 */
void NmEditorHeader::attachmentActivated(int arrayIndex)
{
    NM_FUNCTION;

    emit attachmentShortPressed(mAttachmentList->nmIdByIndex(arrayIndex));    
}

/*!
   Slot attachment selected from attachment list by longtap.
 */
void NmEditorHeader::attachmentLongPressed(int arrayIndex, QPointF point)
{
    NM_FUNCTION;
    
    // Remove selected attachment
    emit attachmentLongPressed(mAttachmentList->nmIdByIndex(arrayIndex), point);
}

/*!
    This function is called when scroll position has been changed.
    Function performs the pre calculated translation to set new positions for header fields
    so that header stays visible when body is scrolled horizontally.
 */
void NmEditorHeader::repositHeader(const QTransform &transform)
{
    NM_FUNCTION;
    
    mToWidget->setTransform(transform);
    mCcWidget->setTransform(transform);
    mBccWidget->setTransform(transform);
    mSubjectWidget->setTransform(transform);
    mAttachmentListWidget->setTransform(transform);
}

/*!
   This slot will set the geometry, margin and font for the To,Cc,Bcc and Subject
   labels located inside the edit fields. These adjustments are done to fix label text
   position so that it is in same lavel than first line of the edit widget.
   Slot is called once when 'To' field layout is ready after editor startup.
   Because all fields are similar it is enough to use only 'To' field information
   to adjust all header fields.
*/
void NmEditorHeader::toFieldLayoutReady()
{
    NM_FUNCTION;
    
    // Set same font for label and edit.
    QFont editorFont = mToField->editor()->font();
    mToField->label()->setFont(editorFont);
    mCcField->label()->setFont(editorFont);
    mBccField->label()->setFont(editorFont);
    mSubjectLabel->setFont(editorFont);

    // Set same height for label and edit.
    qreal editorWidgetHeight = mToField->editor()->geometry().height();
    mToField->label()->setPreferredHeight(editorWidgetHeight);
    mCcField->label()->setPreferredHeight(editorWidgetHeight);
    mBccField->label()->setPreferredHeight(editorWidgetHeight);
    mSubjectLabel->setPreferredHeight(editorWidgetHeight);
    
    // Set left margin for labels.
    qreal leftMargin;
    mToField->editor()->layout()->getContentsMargins(&leftMargin, 0, 0, 0);
    mToField->label()->setContentsMargins(leftMargin, 0, 0, 0);
    mCcField->label()->setContentsMargins(leftMargin, 0, 0, 0);
    mBccField->label()->setContentsMargins(leftMargin, 0, 0, 0);
    mSubjectLabel->setContentsMargins(leftMargin, 0, 0, 0);
    
    adjustIndentForEditFields();
}

/*!
   Set the length of the label and the indention of the edit fields (To,Cc,Bcc,Subject) so
   that there is enough space for the label field.
*/
void NmEditorHeader::adjustIndentForEditFields()
{
    NM_FUNCTION;

    // Get spacing between label and edit.
    qreal spacingBetweenLabelAndEdit = 0;
    HbStyle().parameter("hb-param-margin-gene-middle-horizontal", spacingBetweenLabelAndEdit);
    // Add more space for label right margin. Without this, last letter ':' of the label is not
    // fit into field.
    spacingBetweenLabelAndEdit *= 2;
    
    // Get font metrics (We assume that every label has same font).
    QFontMetrics fm(mToField->label()->font());
    
    // Set length of the label widgets based on the string width.
    // Also set indent for the first line of the edit widget.
    qreal textWidth = 0;
    textWidth = fm.width(mToField->label()->plainText()) + spacingBetweenLabelAndEdit;
    mToField->label()->setPreferredWidth(textWidth);    
    mToField->editor()->setIndentForLabel(textWidth);    
    
    textWidth = fm.width(mCcField->label()->plainText()) + spacingBetweenLabelAndEdit;
    mCcField->label()->setPreferredWidth(textWidth);
    mCcField->editor()->setIndentForLabel(textWidth);

    textWidth = fm.width(mBccField->label()->plainText()) + spacingBetweenLabelAndEdit;
    mBccField->label()->setPreferredWidth(textWidth);
    mBccField->editor()->setIndentForLabel(textWidth);
    
    textWidth = fm.width(mSubjectLabel->plainText()) + spacingBetweenLabelAndEdit;
    mSubjectLabel->setPreferredWidth(textWidth);
    mSubjectEdit->setIndentForLabel(textWidth);
}
