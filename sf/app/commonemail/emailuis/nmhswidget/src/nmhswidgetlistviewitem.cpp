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

#include <qregexp.h>
#include <hbtextitem.h>
#include <hbiconitem.h>
#include <hbframeitem.h>
#include <hbframebackground.h>
#include <hbextendedlocale.h>
#include <hbcolorscheme.h>
#include <nmmessageenvelope.h>
#include <nmicons.h>
#include <hbstringutil.h>
#include "nmhswidgetlistviewitem.h"

/*!
    \class NmHsWidgetListViewItem
    \brief Mail widget list view item inherited from HbListViewItem
*/

/*!
    Constructor
*/
NmHsWidgetListViewItem::NmHsWidgetListViewItem(QGraphicsItem *parent)
: HbListViewItem(parent),
        mSeparator(NULL),
        mSender(NULL),
        mSubject(NULL),
        mTime(NULL),
        mNewMsgIcon(NULL),
        mIcon1(NULL),
        mIcon2(NULL)
{
    NM_FUNCTION; 
}

/*!
    Destructor
*/
NmHsWidgetListViewItem::~NmHsWidgetListViewItem()
{
    NM_FUNCTION;
}

/*!
    Item creation
*/
HbAbstractViewItem* NmHsWidgetListViewItem::createItem()
{
    NM_FUNCTION;
    
    return new NmHsWidgetListViewItem(*this);
}

/*!
    update child items
*/
void NmHsWidgetListViewItem::updateChildItems()
{
    NM_FUNCTION;

    NmMessageEnvelope *envelope =
           modelIndex().data(Qt::DisplayRole).value<NmMessageEnvelope*>();
  
    createMessageItemLayout();
    setContentsToMessageItem(*envelope);
    
    HbListViewItem::updateChildItems();   
}

/*!
    Set item layout.
    Function does not take ownership of model or model item.
 */
void NmHsWidgetListViewItem::createMessageItemLayout()
{
    NM_FUNCTION;
    getFontSizes();
    // Create separator icon and set name from widgetml.
    if (!mSeparator) {
        mSeparator = new HbIconItem(this);
        HbStyle::setItemName(mSeparator, "separator");
        mSeparator->setObjectName("ListViewItemMessageSeparator");
        mSeparator->setIcon(HbIcon("qtg_graf_divider_h_thin"));   
    }
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
        HbStyle::setItemName(mNewMsgIcon, "msgicon");
    }
    
    // Create priority icon and set name from widgetml.
    if(!mIcon1){
        mIcon1 = new HbIconItem(this);
        HbStyle::setItemName(mIcon1, "icon1");
        mStatusIcons.append(mIcon1);
    }

    // Create attachment icon and set name from widgetml.
    if(!mIcon2){
        mIcon2 = new HbIconItem(this);
        HbStyle::setItemName(mIcon2, "icon2");
        mStatusIcons.append(mIcon2);
    }
        
    //set empty frame as list item only shows background when selected
    setDefaultFrame(HbFrameBackground("", HbFrameDrawer::OnePiece));
}

/*!
    Set item text label contents, icons, etc.
    Function does not take ownership of model or model item.
*/
void NmHsWidgetListViewItem::setContentsToMessageItem(const NmMessageEnvelope &envelope)
{
    NM_FUNCTION;
    // Member variables are created in previous function.
    // Sender.
    QString senderDisplayName = envelope.sender().displayName();
    if (!senderDisplayName.isNull() && !senderDisplayName.isEmpty()) {
        mSender->setText(cleanupDisplayName(senderDisplayName));
    }
    else {
        mSender->setText(envelope.sender().address());
    }

    // Time.
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDateTime localTime = envelope.sentTime().addSecs(locale.universalTimeOffset());
    QDate sentLocalDate = localTime.date();
    QDate currentdate = QDate::currentDate();
    if (sentLocalDate == currentdate) {
        QString shortTimeSpec = r_qtn_time_usual;
        QTime time = localTime.time();
        mTime->setText(HbStringUtil::convertDigits(locale.format(time, shortTimeSpec)));
    } else {
        QString shortDateSpec = r_qtn_date_without_year;
        mTime->setText(HbStringUtil::convertDigits(locale.format(sentLocalDate, shortDateSpec)));
    }
    // Subject.
    QString subjectText = envelope.subject();
    if (subjectText.length()) {
        mSubject->setText(subjectText);
    } else {
        mSubject->setText(hbTrId("txt_mailwdgt_dblist_val_no_subject"));
    }
    
    // Here we have list for priority and attachment icons. Later it is easy to add
    // followup icon or something else if needed.
    QList<HbIcon> iconList;
    
    //make sure icons are not shown yet
    for (int i = 0; i < mStatusIcons.count(); i++) {
        mStatusIcons.at(i)->hide();
    }
    
    // Priority icon is added to list first thus it is always shown most right.
    switch (envelope.priority()) {
        case NmMessagePriorityLow:
            iconList.append(NmIcons::getIcon(NmIcons::NmIconPriorityLow));
            break;
        case NmMessagePriorityHigh:
            iconList.append(NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
            break;
        default:
            // Normal priority has no icon
            break;
    }

    // Attachment icon is always shown on the left side of priority icon if it
    // exists. Otherwise in the most right.
    if (envelope.hasAttachments()) {
        iconList.append(NmIcons::getIcon(NmIcons::NmIconAttachment));
    }

    // Here we show icons added to the iconList in the order they have been added.
    for (int count = 0; count < iconList.count(); count++) {
        mStatusIcons.at(count)->setIcon(iconList.at(count));
        mStatusIcons.at(count)->show();
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
   repolish();
}

/*!
    Boolean value indicating model index availability. Always true since
    this item prototype is used for both divider and message.
 */
bool NmHsWidgetListViewItem::canSetModelIndex(const QModelIndex &index) const
{
    NM_FUNCTION;
    Q_UNUSED(index);
    // This item class can handle all items in message list.
    return true;
}

/*!
    Set fonts unread.
 */
void  NmHsWidgetListViewItem::setFontsUnread()
{
    NM_FUNCTION;
    static QColor colorRole = HbColorScheme::color("qtc_list_item_title_normal");
    HbFontSpec fontSpec(HbFontSpec::Primary);
    setFonts(colorRole, fontSpec);
}

/*!
    Set fonts read.
 */
void  NmHsWidgetListViewItem::setFontsRead()
{
    NM_FUNCTION;
    static QColor colorRole = HbColorScheme::color("qtc_list_item_content_normal");
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    setFonts(colorRole, fontSpec);
}

/*!
    Get font sizes.
 */
void  NmHsWidgetListViewItem::getFontSizes()
{
    NM_FUNCTION;
    // Get font sizes from style.
    qreal currentSize;
    bool found = style()->parameter(QString("hb-param-text-height-secondary"), currentSize );
    if (found) {
        mPrimarySize = currentSize;
    }      
    found = style()->parameter(QString("hb-param-text-height-tiny"), currentSize );
    if (found) {
        mSecondarySize = currentSize;
    }        
}

/*!
    Set fonts.
 */
void  NmHsWidgetListViewItem::setFonts(const QColor &colorRole, 
        HbFontSpec &fontSpec)
{
    NM_FUNCTION;
    // Change sizes explicitly since css is overwritten in polish now.    
    if (mSender && mSubject && mTime) {

        fontSpec.setTextHeight(mPrimarySize);
        
        mSender->setFontSpec(fontSpec);
        mSender->setTextColor(colorRole);

        mSubject->setFontSpec(fontSpec);
        mSubject->setTextColor(colorRole);

        fontSpec.setTextHeight(mSecondarySize);

        mTime->setFontSpec(fontSpec);        
        mTime->setTextColor(colorRole);
    }
}

/*!
  Cleans up display name by stripping extra characters from the beginning and end of the string.
*/
QString NmHsWidgetListViewItem::cleanupDisplayName( const QString &displayName )
{
    NM_FUNCTION;

    // find the first and last position that is NOT one of the characters below
    QRegExp rx("[^\\s\"<>]");
    int firstPos = std::max(rx.indexIn(displayName), 0);
    int lastPos = rx.lastIndexIn(displayName);

    if (lastPos < 0) {
        lastPos = displayName.length() - 1;
    }

    return displayName.mid(firstPos, lastPos - firstPos + 1);
}
