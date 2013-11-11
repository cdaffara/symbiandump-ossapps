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

#include "nmuiheaders.h"

/*!
    \class NmMessageListViewItem
    \brief List view item for message list view.
 */

static const int NmFolderTypeRole = Qt::UserRole+1; 

/*!
    Constructor.
 */
NmMessageListViewItem::NmMessageListViewItem(QGraphicsItem *parent)
: HbTreeViewItem(parent),
  mSender(NULL),
  mSubject(NULL),
  mTime(NULL),
  mDividerTitle(NULL),
  mNewMsgIcon(NULL),
  mIcon1(NULL),
  mIcon2(NULL),
  mPrimarySize(0),
  mSecondarySize(0)   
{
}

/*!
    Destructor.
 */
NmMessageListViewItem::~NmMessageListViewItem()
{
}

/*!
    Create item.
 */
HbTreeViewItem *NmMessageListViewItem::createItem()
{
    return new NmMessageListViewItem(*this);
}

/*!
    Boolean value indicating model index availability. Always true since
    this item prototype is used for both divider and message.
 */
bool NmMessageListViewItem::canSetModelIndex(const QModelIndex &index) const
{
    Q_UNUSED(index);
    // This item class can handle all items in message list.
    return true;
}

/*!
    Update child items. List calls this function whenever item is needed
    to be updated.
 */
void NmMessageListViewItem::updateChildItems()
{
    // Get model pointer
    NmMessageListModelItem *msgModelItem = modelIndex().data(
        Qt::DisplayRole).value<NmMessageListModelItem*>();
    // Check whether item is message item or title divider
    // and set the layout accordingly. Dividers are not currently used.
    if (msgModelItem && msgModelItem->itemType() ==
        NmMessageListModelItem::NmMessageItemMessage){
        NmMessageListModel *model = static_cast<NmMessageListModel*>(msgModelItem->model());
        if (model){
            // First set item layout.
            createMessageItemLayout();
            // Set content data to item after layout is created.
            setContentsToMessageItem(msgModelItem->envelope(), model->dividersActive());
        }
    }
    HbTreeViewItem::updateChildItems();
}

/*!
    Set item layout.
    Function does not take ownership of model or model item.
 */
void NmMessageListViewItem::createMessageItemLayout()
{
    getFontSizes();
    // Create sender label and set name from widgetml.
    if (!mSender) {
        mSender = new HbTextItem(this);
        HbStyle::setItemName(mSender, "sender");
        mSender->setObjectName("ListViewItemMessageSender");
    }
    // Create time label and set name from widgetml.
    if (!mTime) {
        mTime = new HbTextItem(this);
        HbStyle::setItemName(mTime, "time");
        mTime->setObjectName("ListViewItemMessageTime");
    }
    // Create subject label and set name from widgetml.
    if (!mSubject) {
        mSubject = new HbTextItem(this);
        HbStyle::setItemName(mSubject, "subject");
        mSubject->setObjectName("ListViewItemMessageSubject");
    }
    // Set new message icon.
    if (!mNewMsgIcon) {
        mNewMsgIcon = new HbFrameItem(this);
    }
    // Create priority icon and set name from widgetml.
    if (!mIcon1) {
        mIcon1 = new HbIconItem(this);
        HbStyle::setItemName(mIcon1, "icon1");
    }
    // Create attachment icon and set name from widgetml.
    if (!mIcon2) {
        mIcon2 = new HbIconItem(this);
        HbStyle::setItemName(mIcon2, "icon2");
    }
    setObjectName("ListViewItemMessage");
}

/*!
    Sets the item text label contents, icons, etc. The method does not take
    ownership of model or model item.

    \param envelope The message envelope.
    \param dividersActive 
*/
void NmMessageListViewItem::setContentsToMessageItem(const NmMessageEnvelope &envelope, 
                                                     bool dividersActive)
{
    // Member variables are created in previous function.
    // Sender.
    mSender->setText(senderFieldText(envelope));

    // Time.
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDateTime localTime = envelope.sentTime().addSecs(locale.universalTimeOffset());
    QDate sentLocalDate = localTime.date();
    QDate currentdate = QDate::currentDate();

    if (dividersActive || sentLocalDate == currentdate) {
        QString shortTimeSpec = r_qtn_time_usual;
        QTime time = localTime.time();
        mTime->setText( HbStringUtil::convertDigits(locale.format(time, shortTimeSpec)) );
    } else {
        QString shortDateSpec = r_qtn_date_without_year;
        mTime->setText( HbStringUtil::convertDigits(locale.format(sentLocalDate, shortDateSpec)) );
    }

    // Subject.
    QString subjectText = envelope.subject();

    if (subjectText.length()) {
        mSubject->setText(subjectText);
    } else {
        mSubject->setText(hbTrId("txt_mail_dblist_val_no_subject"));
    }

    // Priority.
    bool priorityIs(false);

    mIcon1->setIcon(HbIcon());
    mIcon1->setObjectName(QString());
    mIcon1->hide();
    mIcon2->setIcon(HbIcon());
    mIcon2->setObjectName(QString());
    mIcon2->hide();

    switch (envelope.priority()) {
        case NmMessagePriorityLow: {
            priorityIs = true;
            mIcon1->setObjectName("ListViewItemMessageIconPriorityLow");
            mIcon1->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityLow));
            mIcon1->show();
            break;
        }
        case NmMessagePriorityHigh: {
            priorityIs = true;
            mIcon1->setObjectName("ListViewItemMessageIconPriorityHigh");
            mIcon1->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
            mIcon1->show();
            break;
        }
        case NmMessagePriorityNormal: {
            break;
        }
        default:
            break;
    }
    // Attachments.
    HbIconItem *attaIcon = (priorityIs ? mIcon2 : mIcon1);

    if (envelope.hasAttachments()) {
        HbIcon &icon = NmIcons::getIcon(NmIcons::NmIconAttachment);
        attaIcon->setIcon(icon);
        attaIcon->setObjectName("ListViewItemMessageIconAttachment");
        attaIcon->show();
    }
    else {
        attaIcon->setIcon(HbIcon());
        attaIcon->setObjectName(QString());
    }

    // Message read status.
    bool msgReadStatus(envelope.isRead());
    HbFrameDrawer *drawer(NULL);
    HbStyle::setItemName(mNewMsgIcon, "msgicon");

    if (!msgReadStatus) {
        setFontsUnread();
        mNewMsgIcon->setObjectName("ListViewItemMessageIconUnread");
        drawer = new HbFrameDrawer("qtg_fr_list_new_item",
                HbFrameDrawer::ThreePiecesVertical);
        drawer->setFillWholeRect(false);
        // Set drawer. Takes ownership of the drawer and deletes previous drawer.
        mNewMsgIcon->setFrameDrawer(drawer);
        mNewMsgIcon->show();
    } else {
        setFontsRead();
        mNewMsgIcon->setObjectName("ListViewItemMessageIconRead");
        drawer = new HbFrameDrawer(); // Create empty drawer.
        drawer->setFillWholeRect(false);
        // Set drawer. Takes ownership of the drawer and deletes previous drawer.
        mNewMsgIcon->setFrameDrawer(drawer);
        mNewMsgIcon->hide();        
    }
}

/*!
    Set fonts unread.
 */
void  NmMessageListViewItem::setFontsUnread()
{
    QColor colorRole = HbColorScheme::color("qtc_list_item_title_normal");
    HbFontSpec fontSpec(HbFontSpec::Primary);
    setFonts(colorRole, fontSpec);
}

/*!
    Set fonts read.
 */
void  NmMessageListViewItem::setFontsRead()
{
    QColor colorRole = HbColorScheme::color("qtc_list_item_content_normal");
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    setFonts(colorRole, fontSpec);
}

/*!
    Get font sizes.
 */
void  NmMessageListViewItem::getFontSizes()
{
    // Get font sizes from style.
    qreal currentSize;
    bool found = style()->parameter(QString("hb-param-text-height-primary"), currentSize );
    if (found) {
        mPrimarySize = currentSize;
    }      
    found = style()->parameter(QString("hb-param-text-height-secondary"), currentSize );
    if (found) {
        mSecondarySize = currentSize;
    }        
}

/*!
    Set fonts.
 */
void  NmMessageListViewItem::setFonts(const QColor &colorRole, 
        HbFontSpec &fontSpec)
{
    // Change sizes explicitly since css is overwritten in polish now.    
    if (mSender && mSubject && mTime) {
        if (mPrimarySize) {
            fontSpec.setTextHeight(mPrimarySize);
        }      
        mSender->setFontSpec(fontSpec);
        mSender->setTextColor(colorRole);
        if (mSecondarySize) {
            fontSpec.setTextHeight(mSecondarySize);
        }        
        mSubject->setFontSpec(fontSpec);
        mSubject->setTextColor(colorRole);
        mTime->setFontSpec(fontSpec);        
        mTime->setTextColor(colorRole);
    }
}

/*!
    Get sender field text. Function returns sender field text from
    envelope based on currently used function.
 */
QString NmMessageListViewItem::senderFieldText(const NmMessageEnvelope &envelope)
{
    QString ret;  
    QVariant folderType = modelIndex().data(
            NmFolderTypeRole).value<QVariant>();
    switch (folderType.toInt()) {
        // Outbox, drafts and sent folder, sender name is 
        // replaced with first recipient from to/cc list.
        case NmFolderOutbox:
        case NmFolderDrafts:
        case NmFolderSent:
        {
            QList<NmAddress>& toRecipients = envelope.toRecipients();
            QList<NmAddress>& ccRecipients = envelope.ccRecipients();
            NmAddress addressToUse;
            bool foundAddress(false);
            if (toRecipients.count()){
                addressToUse=toRecipients[0];
                foundAddress=true;
            }
            else if (ccRecipients.count()) {
                addressToUse=ccRecipients[0]; 
                foundAddress=true;         
            }
            if (foundAddress){
                QString displayName = addressToUse.displayName();
                if (displayName.length()) {
                    ret += NmUtilities::cleanupDisplayName(displayName);
                } 
                else {
                    ret += addressToUse.address();                    
                }               
            }
        }
        break;    
        // All other folders will show sender display name.
        default: 
        {
            QString displayName = envelope.sender().displayName();
            if (displayName.length()) {
                ret += NmUtilities::cleanupDisplayName(displayName);
            } 
            else {
                ret += envelope.sender().address();                    
            }
        }
        break;
    }
    return ret;
}

/*!
    Handle font color changes in the case of theme change
 */ 
bool NmMessageListViewItem::event(QEvent *event)
{
    bool ret = HbTreeViewItem::event(event);    
    if (event && event->type() == HbEvent::ThemeChanged) {
        NmMessageListModelItem *msgModelItem(NULL);
        msgModelItem = modelIndex().data(
            Qt::DisplayRole).value<NmMessageListModelItem*>();
        if (msgModelItem && !msgModelItem->envelope().isRead()) {
            setFontsUnread();
        }
        else if (msgModelItem) {
            setFontsRead();
        }
    }
    return ret;
}

