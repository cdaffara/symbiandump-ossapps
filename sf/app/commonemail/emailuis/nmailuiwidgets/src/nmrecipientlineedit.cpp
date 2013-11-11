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
* Description: implementation of recipient field (to, cc, bcc) line editor
*
*/

#include "nmailuiwidgetsheaders.h"

static const QString NmRecipientLineEditSemicolon(";");
static const QString NmRecipientLineEditDelimiter("; ");
static const QRegExp NmRecipientLineEditCommaOrSemicolon("[,;]");

static const int NmRecipientLineEditFadedAlpha(125);

static const int NmRecipientLineEditForegroundZValue(200);
static const int NmRecipientLineEditBackgroundZValue(0);

static const QString FILE_PATH_WIDGETML = ":nmrecipientlineedit.widgetml";
static const QString FILE_PATH_CSS = ":nmrecipientlineedit.css";
static const QString FILE_PATH_AUTOFILL_DOCML = ":nmautofilllistview.docml";
static const QString FILE_PATH_AUTOFILLITEM_WIDGETML = ":nmautofilllistviewitem.widgetml";
static const QString FILE_PATH_AUTOFILLITEM_CSS = ":nmautofilllistviewitem.css";
/*!
   Constructor
*/
NmRecipientLineEdit::NmRecipientLineEdit(QGraphicsItem *parent)
    : NmHtmlLineEdit(parent),
    mNeedToGenerateEmailAddressList(true),
    mContactHistoryModel(NULL),
    mAutoFillPopup(NULL),
    mAutofillListView(NULL),
    mAutofillListViewItem(NULL),
    mPopupBackground(NULL)
{
    NM_FUNCTION;

    HbStyleLoader::registerFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::registerFilePath(FILE_PATH_CSS);
    HbStyleLoader::registerFilePath(FILE_PATH_AUTOFILLITEM_WIDGETML);
    HbStyleLoader::registerFilePath(FILE_PATH_AUTOFILLITEM_CSS);

    createAutofillComponents();

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(handleTextChanged(QString)));
}

/*!
   Destructor
*/
NmRecipientLineEdit::~NmRecipientLineEdit()
{
    NM_FUNCTION;
    delete mPopupBackground;
    delete mContactHistoryModel;
    delete mAutofillListView;

    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_CSS);
    HbStyleLoader::unregisterFilePath(FILE_PATH_AUTOFILLITEM_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_AUTOFILLITEM_CSS);
}

/*!
   Get the emailaddress list generated from the content of the lineedit.
*/
QList<NmAddress> NmRecipientLineEdit::emailAddressList()
{
    NM_FUNCTION;

    if (mNeedToGenerateEmailAddressList) {
        // Empty mEmailAddressList.
        mEmailAddressList.clear();
        // Generate mEmailAddressList from the lineedit content.
        generateEmailAddressList();
        mNeedToGenerateEmailAddressList = false;
    }

    return mEmailAddressList;
}

/*!
   This Slot appends the selected contacts to the end of the lineedit content.
*/
void NmRecipientLineEdit::addSelectedContacts(const QVariant &selectedContacts)
{
    NM_FUNCTION;

    // If user selected contact
    if (!selectedContacts.isNull()) {

        // If the lineedit is not empty and if there is no ";" or "; " at the end,
        // add a delimiter("; ") at the end.
        if (text().length() != 0 && !(text().endsWith(NmRecipientLineEditSemicolon)) &&
            !(text().endsWith(NmRecipientLineEditDelimiter))){

            // Move cursor to the end of the lineedit.
            setCursorPosition(text().length());
            QTextCursor textCursor(this->textCursor());
            // Append delimiter("; ") to the end of the lineedit
            textCursor.insertText(NmRecipientLineEditDelimiter);
        }

        CntServicesContactList contactList = qVariantValue<CntServicesContactList>(selectedContacts);

        // Loop through all the selected contacts.
        for (int i = 0; i < contactList.count(); ++i) {
            QString contactName = contactList[i].mDisplayName;
            QString contactEmailAddress = contactList[i].mEmailAddress;

            // If this contact has no name, use it's emailaddress as the display name
            if (contactName.isEmpty()) {
                // Move cursor to the end of the lineedit.
                setCursorPosition(text().length());
                insertContactText(contactEmailAddress);
            }
            // If this contact has name, use the name as the display name
            else {
                // Handle a rare case: there are contacts has same name but different emailaddress.
                for (int i = 0; i != mRecipientsAddedFromContacts.count(); ++i) {
                    if (mRecipientsAddedFromContacts.at(i).displayName() == contactName &&
                  	    mRecipientsAddedFromContacts.at(i).address() != contactEmailAddress) {
                        // Differentiate this contact by supplying it's emailaddress
                        contactName.append("<");
                        contactName.append(contactEmailAddress);
                        contactName.append(">");
                    }
                }

                // Move cursor to the end of the lineedit.
                setCursorPosition(text().length());
                insertContactText(contactName);
            }

            // Form the contact into NmAddress format.
            NmAddress contact;
            contact.setAddress(contactEmailAddress);
            contact.setDisplayName(contactName);

            // Add this NmAddress formated contact into mRecipientsAddedFromContacts.
            mRecipientsAddedFromContacts.append(contact);
        }
    }
    else {
        //Request returned NULL
        NM_COMMENT("ContactsPicker request returned NULL.");
    }
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

/*!
   keyPressEvent handles different keypress events, and reacts to them
*/
void NmRecipientLineEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    NM_FUNCTION;

    if (keyEvent) {
        switch (keyEvent->key()) {
        case Qt::Key_Comma:
        case Qt::Key_Semicolon:
            keyPressEventSemicolon(keyEvent);
            break;
            
        case Qt::Key_Backspace:
        case Qt::Key_Delete:
            keyPressEventDelete(keyEvent);
            break;
            
        case Qt::Key_Left:
            keyPressEventLeft(keyEvent);
            break;

        case Qt::Key_Right:
            keyPressEventRight(keyEvent);
            break;

        default:
            NmHtmlLineEdit::keyPressEvent(keyEvent);
            break;
        } // switch
    keyEvent->accept();
    }
}

/*!
 * subroutine of keyPressEvent that handles semicolon and comma key presses
 */
void NmRecipientLineEdit::keyPressEventSemicolon(QKeyEvent *event)
{
    NM_FUNCTION;
    QString textBeforeCursor = (text()).left(cursorPosition());

    // No action when the lineedit is empty or cursor is after a Delimiter("; ")
    // or a Semicolon (";").
    if ((text()).isEmpty() || textBeforeCursor.endsWith(NmRecipientLineEditDelimiter)
        || textBeforeCursor.endsWith(NmRecipientLineEditSemicolon)) {
        event->ignore();
    }
    else {
        // Generate custom keyevent for Delimiter("; ") and
        // forward to the base class to handle.
        QKeyEvent delimiterKeyEvent(event->type(), event->key(),
                                    event->modifiers(), NmRecipientLineEditDelimiter);
        NmHtmlLineEdit::keyPressEvent(&delimiterKeyEvent);
    }
}

/*!
 * subroutine of keyPressEvent that handles delete and backspace key presses
 */
void NmRecipientLineEdit::keyPressEventDelete(QKeyEvent *event)
{
    NM_FUNCTION;
    int pos = cursorPosition();

    // 1) if selection exists, delete it
    if (hasSelectedText()) {
        // if already selected delete it
        disconnect(this, SIGNAL(textChanged(QString)),
                this, SLOT(handleTextChanged(QString)));
        QKeyEvent eve(event->type(), Qt::Key_Delete, Qt::NoModifier);
        HbLineEdit::keyPressEvent(&eve);
        connect(this, SIGNAL(textChanged(QString)),
                this, SLOT(handleTextChanged(QString)));
        setCursorPosition(text().length());
        handleTextChanged(text());
    } else { // no selection..
        // peek if earlier chars are underlined
        setCursorPosition(pos-2);
        bool isContact(textCursor().charFormat().fontUnderline());
        setCursorPosition(pos);
        if (isContact) {
            // contact
            setCursorPosition(pos-3);
            setHighlight(pos-3); // creates a selection for the current contact
        } else {
            // plain text
            HbLineEdit::keyPressEvent(event);
        }
    }
}

/*!
 * subroutine of keyPressEvent that handles left key presses
 */
void NmRecipientLineEdit::keyPressEventLeft(QKeyEvent *event)
{
    NM_FUNCTION;
    //look ahead left.
    int pos = cursorPosition();
    QString text = this->text();
    text = text.left(pos);

    //look for next seperator while going left.
    int semicolonPos = text.lastIndexOf(NmRecipientLineEditSemicolon);
    bool selectedText = hasSelectedText();

    if (selectedText) {
        int selectionStart = textCursor().selectionStart();
        if (selectionStart>=2) {
            setCursorPosition(selectionStart-2);
        }
        else {
            setCursorPosition(0);
        }
        textCursor().clearSelection();
    }
    else { // not selected

        setCursorPosition(pos-2);
        bool isContact = textCursor().charFormat().fontUnderline();
        setCursorPosition(pos);

        if (isContact) {
            setHighlight(pos-3);
        }
        else {
            if (pos-2  == semicolonPos) {
                setCursorPosition(pos-1); // jump over one extra char
            }
            HbLineEdit::keyPressEvent(event);
        }
    }
}

/*!
 * subroutine of keyPressEvent that handles right key presses
 */
void NmRecipientLineEdit::keyPressEventRight(QKeyEvent *event)
{
    NM_FUNCTION;
    bool selectedText = hasSelectedText();

    if (selectedText) {
        int selectionEnd = textCursor().selectionEnd();
        setCursorPosition(selectionEnd+1);
        textCursor().clearSelection();
    }
    else {
        int pos = cursorPosition();

        //look ahead
        setCursorPosition(pos+3);
        bool isContact = textCursor().charFormat().fontUnderline();
        setCursorPosition(pos);

        if (isContact) {
            setHighlight(pos+3);
        }
        else {
            int nextSemicolon = text().indexOf(NmRecipientLineEditSemicolon,pos);
            if (nextSemicolon==pos) {
                setCursorPosition(pos+1); // jump over one extra char
            }
        HbLineEdit::keyPressEvent(event);
        }
    }
}

/*!
   inputMethodEvent handles replacing user inputs "," or ";" from virtual keyboard with "; ".
   P.S. keyPressEvent can only catch QKeyEvent "," or ";" typed from physical keyboard
*/
void NmRecipientLineEdit::inputMethodEvent(QInputMethodEvent *event)
{
    NM_FUNCTION;

	bool eventHandled(false);

    if (event) {
        QString eventText = event->commitString();

        if (!eventText.isEmpty() || event->replacementLength()) {
            // If typed charater from virtual keyboard is "," or ";"
            if (eventText.contains(NmRecipientLineEditCommaOrSemicolon)) {
                QString textBeforeCursor = text().left(cursorPosition());

                // No action when the lineedit is empty or cursor is after a Delimiter("; ")
                // or Semicolon (";").
                if (text().isEmpty() || textBeforeCursor.endsWith(NmRecipientLineEditDelimiter) || textBeforeCursor.endsWith(NmRecipientLineEditSemicolon)) {
                    event->ignore();
                    eventHandled = true;
                }
                else {
                    // Modify event with Delimiter("; ") and forward to the base class to handle.
                    event->setCommitString(NmRecipientLineEditDelimiter, event->replacementStart(),
                                           event->replacementLength());
                    NmHtmlLineEdit::inputMethodEvent(event);
                    eventHandled = true;
                }
            }
        }
    }

    // If event is not handled, forward to the base class to handle.
    if (!eventHandled) {
        NmHtmlLineEdit::inputMethodEvent(event);
    }
}

/*!
 signal received from user screen input
 */
void NmRecipientLineEdit::gestureEvent(QGestureEvent* event)
{
    //passing gesture event to base class.
    HbLineEdit::gestureEvent(event);

    if (HbTapGesture *tap = qobject_cast<HbTapGesture*>(event->gesture(Qt::TapGesture))) {
        //capturing gesture position, and map to local co-ordinates.
        QPointF pos = mapFromScene(tap->scenePosition());

        switch (tap->state()) {
        case Qt::GestureFinished:
            if (HbTapGesture::Tap == tap->tapStyleHint()) {
                handleTap();
            }
            break;
        default:
            break;
        }
        event->accept();
    }
    else {
        event->ignore();
    }
}

/*!
 user has tapped the recipient field on the screen
 */
void NmRecipientLineEdit::handleTap()
{
    int currentPos = cursorPosition();
    QString txt = text();

    QString leftTxt = txt.left(currentPos+2);
    int previousSemicolonIndex = leftTxt.lastIndexOf(NmRecipientLineEditSemicolon,currentPos);
    if ((currentPos>0) &&
       (currentPos==previousSemicolonIndex || currentPos==previousSemicolonIndex+1)) {
        //pressed just on seperator
        setCursorPosition(previousSemicolonIndex+2);
    }
    else
    {
        // pressed in middle of an address
        setCursorPosition(currentPos);
        if (textCursor().charFormat().fontUnderline()) {
            // This entry is a "contact"
            setHighlight(currentPos);
        }
    }
    update();
}

/*!
   Generate a list of all the email addresses from the content of the lineedit.
*/
void NmRecipientLineEdit::generateEmailAddressList()
{
    NM_FUNCTION;

    // Remove whitespace from the start and the end of the lineedit content.
    QString contentOfLineedit = text().trimmed();

    // Split the lineedit content by semicolon(";").
    QStringList itemsOfLineeditContent = contentOfLineedit.split(NmRecipientLineEditSemicolon, QString::SkipEmptyParts);

    // Loop through all the items of the lineedit content.
    for (int i = 0; i != itemsOfLineeditContent.count(); ++i) {
        // Remove whitespace from the start and the end of the item.
        QString itemInLineedit = itemsOfLineeditContent.at(i).trimmed();

        // Get the count of the recipients added from Contacts.
        int countOfRecipientsAddedFromContacts = mRecipientsAddedFromContacts.count();

        // If there is recipient added from Contacts.
        if (countOfRecipientsAddedFromContacts > 0) {
            QStringList listOfAddedContactsName;
            QStringList listOfAddedContactsAddress;

            // Loop through all the recipients added from Contacts.
            for (int j = 0; j != countOfRecipientsAddedFromContacts; ++j) {
                NmAddress contact = mRecipientsAddedFromContacts.at(j);
                listOfAddedContactsName.append(contact.displayName());
                listOfAddedContactsAddress.append(contact.address());
            }

            int indexInAddedContactsName = listOfAddedContactsName.indexOf(itemInLineedit);
            int indexInAddedContactsAddress = listOfAddedContactsAddress.indexOf(itemInLineedit);

            // If this itemInLineedit matches the name of one added contact.
            if (indexInAddedContactsName >= 0) {
                // Add the recipient into mEmailAddressList.
                mEmailAddressList.append(mRecipientsAddedFromContacts.at(indexInAddedContactsName));
            }
            // If this itemInLineedit matches the emailaddress of one added contact.
            else if (indexInAddedContactsAddress >= 0) {
                // Add the recipient into mEmailAddressList.
                mEmailAddressList.append(mRecipientsAddedFromContacts.at(indexInAddedContactsAddress));
            }
            // This itemInLineedit is not added from Contacts
            else {
                // Form the item into NmAddress format.
                NmAddress recipient;
                recipient.setAddress(itemInLineedit);
                // There is no display name info available, so leave display name empty.
                recipient.setDisplayName(QString());
                // Add this NmAddress formated lineedit item into mEmailAddressList.
                mEmailAddressList.append(recipient);
            }
        }
        else { // There is no recipient is added from Contacts
            // Form the item into NmAddress format.
            NmAddress recipient;
            recipient.setAddress(itemInLineedit);
            // There is no display name info available, so leave display name emapty.
            recipient.setDisplayName(QString());
            // Add this NmAddress formated lineedit item into mEmailAddressList.
            mEmailAddressList.append(recipient);
        }
    }
}


/*!
   This Slot is called when the lineedit text changes.
*/
void NmRecipientLineEdit::handleTextChanged(const QString &text)
{
    NM_FUNCTION;

    mNeedToGenerateEmailAddressList = true;

    //if there is no text, hide popup already
    if (document()->isEmpty()) {
        hideAutofillPopup();
    }

    if (mContactHistoryModel) {
        int startPos(-1), length(-1);
        currentTextPart(startPos, length);
        QString t = text.mid(startPos, length);
        if (t.length()) {
            mContactHistoryModel->query(t);
        }
    }
}

/*!
    If recipient is added from Contacts by Contacts, we need to add it to the list.
 */
void NmRecipientLineEdit::addContacts(QList<NmAddress *> contacts)
{
    foreach (NmAddress *nmContact,contacts) {
        if (nmContact && nmContact->displayName().length() > 0) {
            mRecipientsAddedFromContacts.append(*nmContact);
            mNeedToGenerateEmailAddressList = true;
        }
    }
}

/*!
   This Slot appends the selected contact to the end of the lineedit content.
*/
void NmRecipientLineEdit::addSelectedContactFromHistory(const QModelIndex &modelIndex)
{
    NM_FUNCTION;

    // ignore text changes during handling of text
    disconnect(this, SIGNAL(textChanged(QString)),
               this, SLOT(handleTextChanged(QString)));

    // Get address from contact history model
    NmAddress contact;
    getChosenAddressFromModel(modelIndex, contact);

    if (!contact.address().isEmpty()) {
        // Add the recipient to internal list of addresses
        mRecipientsAddedFromContacts.append(contact);

        // Find the text part to be replaced
        int startPos(-1), length(-1);
        currentTextPart(startPos, length);

        // delete currently entered characters before adding replacement
        QTextCursor cursor(textCursor());
        cursor.setPosition(startPos);
        for(int i=0; i<length; i++) {
            cursor.deleteChar();
        }

        // Insert replacement text
        if (contact.displayName().isEmpty()) {
            insertContactText(contact.address());
        }
        else {
            insertContactText(contact.displayName());
        }
    }
    // Continue accepting text changes..
    connect(this, SIGNAL(textChanged(QString)),
            this, SLOT(handleTextChanged(QString)));

    //hide popup
    hideAutofillPopup();
}
/*!
 reads the chosen data from contact history model
 */
void NmRecipientLineEdit::getChosenAddressFromModel(const QModelIndex &modelIndex, NmAddress &address)
{
    NM_FUNCTION;
    // Dig out the chosen contact data
    // Get the activated item with given modelIndex
    NmContactHistoryModelItem item = mContactHistoryModel->data(modelIndex, Qt::DisplayRole).value<NmContactHistoryModelItem>();
    if (item.subItemCount()) {
        QList<NmContactHistoryModelSubItem> itemlist = item.subEntries();
        if (itemlist.count() == 2) {
            address.setDisplayName(itemlist[0].mItemText);
            address.setAddress(itemlist[1].mItemText);
        } else if (itemlist.count() == 1) {
            // only emailaddress found (no display name)
            address.setAddress(itemlist[0].mItemText);
        }
    }
}

/*!
 finds the current "entry" (startindex+length) based on current cursor position.
 Excludes possible trailing delimiter
 */
void NmRecipientLineEdit::currentTextPart(int &startIndex, int &length)
{
    NM_FUNCTION;

    // Get current cursor position in the text
    int currentPos = textCursor().position();

    QString tmp_debug = text();
    startIndex = qMax(text().lastIndexOf(NmRecipientLineEditSemicolon,currentPos-1), 0);
    if (startIndex>0) {
        startIndex+=2; // in case entry is not the first, advance the start index by amount of "; ".
    }


    int nextDelimiterIndex=text().indexOf(NmRecipientLineEditDelimiter,currentPos);
    if (nextDelimiterIndex==-1) {
        // no delim after current pos
        length=text().length()-startIndex;
    }
    else {
        // delim after current pos found
        length=nextDelimiterIndex-startIndex;
    }
}

/*!
    Helper function for creating autofill component.
*/
void NmRecipientLineEdit::createAutofillComponents()
{
    NM_FUNCTION;
    
    //create model
    mContactHistoryModel = new NmContactHistoryModel(EmailAddressModel);
    connect(mContactHistoryModel,SIGNAL(modelCompleted(int)),this,SLOT(modelCompleted(int)));

    //create popup list components
    HbDocumentLoader loader;
    bool loadingOk(false);
    loader.load(FILE_PATH_AUTOFILL_DOCML, &loadingOk);
    if (loadingOk){
        mAutofillListView = dynamic_cast<HbListView*>(loader.findWidget("listview"));
    }
    else {
        NM_COMMENT("mAutofillListView loading from docML failed.");
        return; 
    }
    
    connect(mAutofillListView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(addSelectedContactFromHistory(const QModelIndex &)));

    mAutofillListViewItem = new NmAutoFillListViewItem();
    mAutofillListView->setModel(static_cast<QAbstractListModel*>(mContactHistoryModel),mAutofillListViewItem);

    //create popup
    createAutoFillPopup();

    //set layout for popup
    QGraphicsLinearLayout *linLayout = new QGraphicsLinearLayout(Qt::Horizontal,
                                                                 mAutoFillPopup);
    linLayout->addItem(mAutofillListView);

}

/*!
  Helper function for creating popup component.
*/
void NmRecipientLineEdit::createAutoFillPopup()
{
    NM_FUNCTION;
    mAutoFillPopup = new HbPopup(this);
    mAutoFillPopup->setVisible(false);
    mAutoFillPopup->setFlag(QGraphicsItem::ItemIsPanel, true);
    mAutoFillPopup->setActive(false);
    mAutoFillPopup->setFocusPolicy(Qt::NoFocus);
    mAutoFillPopup->setBackgroundFaded(false);
    mAutoFillPopup->setDismissPolicy(HbPopup::TapOutside);
    mAutoFillPopup->setTimeout(HbPopup::NoTimeout);
    mAutoFillPopup->setFrameType(HbPopup::Weak); //uses qtg_fr_popup_secondary graphics
    HbStyle::setItemName(mAutoFillPopup, QString("autoFillPopup"));

    mPopupBackground = new NmPopupBackground(mAutoFillPopup, this);
    mPopupBackground->setVisible(false);
    mPopupBackground->setZValue(mAutoFillPopup->zValue() - 1);
}

/*!
  Helper function for showing autofill popup.
*/
void NmRecipientLineEdit::showAutofillPopup()
{
    NM_FUNCTION;
    if (!mAutoFillPopup->isVisible()) {
        parentItem()->setZValue(NmRecipientLineEditForegroundZValue);
        mAutoFillPopup->show();
        mPopupBackground->setVisible(true);
    }
}

/*!
  Helper function for hiding autofill popup.
*/
void NmRecipientLineEdit::hideAutofillPopup()
{
    NM_FUNCTION;
    if (mAutoFillPopup->isVisible()) {
        mPopupBackground->setVisible(false);
        parentItem()->setZValue(NmRecipientLineEditBackgroundZValue);
        mAutoFillPopup->hide();
    }
}

/*!
  Slot for listening when model is ready with a new data.
*/
void NmRecipientLineEdit::modelCompleted(int err)
{
    NM_FUNCTION;
    if (err == 0) {
        //show model if there was findings
        int count = mContactHistoryModel->rowCount(QModelIndex());

        if (count > 0) {
            showAutofillPopup();
        } else {
            hideAutofillPopup();
        }
    }
}

/*!
 finds the current "entry" in the editor field based on current cursor position and creates a selection of it.
 Includes possible trailing delimiter
 */
void NmRecipientLineEdit::setHighlight(int currentPos)
{
    QString txt = text();

    int startPos = qMax(txt.lastIndexOf(NmRecipientLineEditDelimiter,currentPos), 0);
    if (startPos) {
        startPos+=2; // in case entry is not the first, advance the start index by amount of "; "
    }
    int endPos = qMax(txt.indexOf(NmRecipientLineEditDelimiter,currentPos), currentPos);

    if (startPos >= 0 && endPos >= 0 && startPos != endPos) {
        setSelection(startPos, endPos - startPos + 2);
    }
    else {
        deselect();
    }

    update();
}

/*!
 inserts a new contact text (underlined) + trailing delimiter to current cursor position in editor field
 */
void NmRecipientLineEdit::insertContactText(const QString &text)
    {
    NM_FUNCTION;

    // Insert contact text as underlined
    QTextCharFormat colorFormat(textCursor().charFormat());
    QColor fgColor = colorFormat.foreground().color();
    fgColor.setAlpha(NmRecipientLineEditFadedAlpha);
    colorFormat.setUnderlineColor(fgColor);
    colorFormat.setFontUnderline(true);
    textCursor().insertText(text, colorFormat);

    // Insert delimiter (not underlined)
    colorFormat.setFontUnderline(false);
    textCursor().insertText(NmRecipientLineEditDelimiter,colorFormat);
    }

/*!
    Popup's background item. Used to close properly popup if clicked to outside of the popup.
 */
NmPopupBackground::NmPopupBackground(HbPopup * popup, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    mPopup(popup)
{
    // This is needed to be able to block moving the focus to items behind background item by
    // clicking on them.
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsPanel);
}

/*!
    Paint. Not used but since this is pure virtual it must be here. 
 */
void NmPopupBackground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED(option)
    Q_UNUSED(widget);
    Q_UNUSED(painter);
}

/*!
    Mouse press events are checked here. If background is clicked, popup must be closed. 
 */
bool NmPopupBackground::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress) {
        mPopup->close();
        setVisible(false);
        event->accept();
        return true;
    }
    else {
        event->ignore();
        return QGraphicsItem::sceneEvent(event);
    }
}
