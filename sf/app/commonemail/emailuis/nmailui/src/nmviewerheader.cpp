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
* Description: NMail viewer view header widget implementation
*
*/


#include "nmuiheaders.h"

static const qreal NmHeaderLineOpacity = 0.4;

/*!
    \class NmViewerHeader
    \brief Mail viewer header area class
*/

/*!
    Constructor
*/
NmViewerHeader::NmViewerHeader(QGraphicsItem *parent) :
    HbWidget(parent),
    mMessage(NULL),
    mSubject(NULL),
    mSent(NULL),
    mPrioIcon(NULL),
    mHeaderBox(NULL),
    mRecipientsBox(NULL),
    mViewerView(NULL)
{
    NM_FUNCTION;
    
    loadWidgets();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

/*!
    Destructor
*/
NmViewerHeader::~NmViewerHeader()
{
    NM_FUNCTION;
}

/*!
    setter for nmviewer view
*/
void NmViewerHeader::setView(NmViewerView* view)
{
    NM_FUNCTION;
    
    mViewerView = view;
}

/*!
    Load widgets from XML for the header
*/
void NmViewerHeader::loadWidgets()
{
    NM_FUNCTION;
    
    // Scale header widget to screen width
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding); 

    // Add header box
    if (!mHeaderBox) {
        mHeaderBox = new HbGroupBox(this);
        HbStyle::setItemName(mHeaderBox, "headergroupbox");
        mHeaderBox->setObjectName("ViewerHeaderRecipients");
        mHeaderBox->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
        mHeaderBox->setCollapsable(true);
    }

    // Add sent time
    if (!mSent){
        mSent = new HbTextItem(this);
        HbStyle::setItemName(mSent, "sent");
        mSent->setObjectName("ViewerHeaderSent");
    }

    // Add priority icon
    if (!mPrioIcon){
        mPrioIcon = new HbIconItem(this);
        mSent->setObjectName("ViewerHeaderPrioIcon");
        HbStyle::setItemName(mPrioIcon, "prioicon");
    }

    // Add subject
    if (!mSubject){
        mSubject = new HbTextItem(this);
        HbStyle::setItemName(mSubject, "subject");
        mSubject->setObjectName("ViewerHeaderSubject");
        mSubject->setTextWrapping(Hb::TextWordWrap);
    }
}


/*!
    Reimplementation to do some extra painting
*/
void NmViewerHeader::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    NM_FUNCTION;
    
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (painter) {
        painter->save();
        painter->setOpacity(NmHeaderLineOpacity);
        QLineF line1( rect().topLeft().x(), rect().bottomRight().y(),
                     rect().bottomRight().x(), rect().bottomRight().y());
        painter->drawLine(line1);
        if (mHeaderBox){
            QRectF headerBoxGeometry = mHeaderBox->geometry();
            QLineF line2( headerBoxGeometry.topLeft().x(), headerBoxGeometry.bottomRight().y(),
                          headerBoxGeometry.bottomRight().x(), headerBoxGeometry.bottomRight().y());
            painter->drawLine(line2);        
        } 
        painter->restore();      
    }
}


/*!
    Sets the message and updates the UI according to the given message data.

    \param message The message to set.
*/
void NmViewerHeader::setMessage(NmMessage* message)
{
    NM_FUNCTION;
    
    mMessage = message;
    setHeaderData();
}


/*!
    Updates data in already created objects. The given message pointer comes
    from the viewer view and the ownership of the instance is not transferred.
    This method gets called when the message body is fetched and recipient
    (to/cc/bcc) data needs to be updated.

    Note that if the message has been changed, this method should not be called
    since this method _only_ updates the recipient data! If the message has been
    changed, call setMessage() instead.

    \param message The message data.
*/
void NmViewerHeader::updateMessageData(NmMessage* message)
{
    NM_FUNCTION;
    
    if (message) {
        mMessage = message;

        // Set recipients to the text edit field as html. 
        NmAddress sender = mMessage->envelope().sender();

        if (mRecipientsBox) {  
            mRecipientsBox->setHtml(
                formatRecipientList(addressToDisplay(sender),
                                    mMessage->envelope().toRecipients(), 
                                    mMessage->envelope().ccRecipients()));
        }
    }
}


/*!
    Sets the data of the header area based on message data.
*/
void NmViewerHeader::setHeaderData()
{
    NM_FUNCTION;
    
    if (mMessage) {
        // Background is all white always, so force text color to black.
        QColor textColor(Qt::black);
        
        // Get the message envelope.
        const NmMessageEnvelope &envelope = mMessage->envelope();

        // Set the sender.
        const QString dispName = envelope.sender().displayName();

        if (dispName.length()) {
            mSenderName = NmUtilities::cleanupDisplayName(dispName);
        }
        else {
            // Use the address instead.
            mSenderName = envelope.sender().address();
        }

        if (mHeaderBox) {
            mHeaderBox->setHeading(mSenderName);
        }

        // Set the subject text.
        if (mSubject){
            QString subjectText = envelope.subject();

            if (subjectText.length()) {
                mSubject->setText(subjectText);
            }
            else{
                mSubject->setText(hbTrId("txt_mail_dblist_val_no_subject"));
            }

            mSubject->setTextColor(textColor);
        }

        if (mSent) {
            HbExtendedLocale locale = HbExtendedLocale::system();
            QDateTime localTime =
                envelope.sentTime().addSecs(locale.universalTimeOffset());
            QTime time = localTime.time();
            QDate sentLocalDate = localTime.date();
            QString shortDateSpec = r_qtn_date_without_year;
            QString shortTimeSpec = r_qtn_time_usual;
            QString text = locale.format(sentLocalDate, shortDateSpec);
            text += " ";
            text += locale.format(time, shortTimeSpec);
            mSent->setText(HbStringUtil::convertDigits(text));
            mSent->setTextColor(textColor);
        }

        if (mPrioIcon) {
            switch (envelope.priority()) {
                case NmMessagePriorityLow: {
                    mPrioIcon->setObjectName("ViewerHeaderPriorityLow");
                    mPrioIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityLow));
                    mPrioIcon->show();
                    break;
                }
                case NmMessagePriorityHigh: {
                    mPrioIcon->setObjectName("ViewerHeaderPriorityHigh");
                    mPrioIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
                    mPrioIcon->show();
                    break;
                }
                case NmMessagePriorityNormal:
                default: {
                    // Normal priority has no icon so hide the label hide.
                    mPrioIcon->setObjectName("ViewerHeaderPriorityNormal");
                    break;
                }
            }
        }
    }

    createExpandableHeader();
}


/*!
    Function can be used to rescale the header area.
*/ 
void NmViewerHeader::rescaleHeader(const QSizeF layoutReso)
{  
    NM_FUNCTION;
    
    setMinimumWidth(layoutReso.width());
    setMaximumWidth(layoutReso.width());
}

/*!
    Function creates expandable header area with group box
*/
void NmViewerHeader::createExpandableHeader()
{
    NM_FUNCTION;
    
    if (mHeaderBox) {        // Initialize recipient box
        if (!mRecipientsBox) {
            mRecipientsBox = new HbTextEdit();
            mRecipientsBox->setReadOnly(true);
            mRecipientsBox->document()->
                    defaultTextOption().setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            mRecipientsBox->setObjectName("toField");
            mRecipientsBox->setBackgroundItem(NULL);
            mRecipientsBox->setFontSpec(HbFontSpec(HbFontSpec::Secondary)); 
            mRecipientsBox->setContextMenuFlags(0); 
            connect(mRecipientsBox, SIGNAL(anchorTapped(const QString &)),
                    this, SLOT(anchorTapped(const QString &)));                    
        }              
        if (mMessage) {
            // Set recipients to text edit field as html 
            NmAddress sender = mMessage->envelope().sender(); 
            mRecipientsBox->setHtml(formatRecipientList(addressToDisplay(sender),
                                    mMessage->envelope().toRecipients(), 
                                    mMessage->envelope().ccRecipients()));
        }
        mHeaderBox->setContentWidget(mRecipientsBox);
        // Set box collapsed as default
        mHeaderBox->setCollapsed(true);
    }
}

/*!
    Function formats recipient list to be displayed in HTML format.
*/
QString NmViewerHeader::formatRecipientList(const QString &sender,
                                            const QList<NmAddress> &to,
                                            const QList<NmAddress> &cc)
{
    NM_FUNCTION;
    
    QString result;
    result.append("<html><body link=\"blue\" topmargin=\"0\" leftmargin=\"0\" marginheight=\"0\"");
    result.append("marginwidth=\"0\">");    
    result.append("<font color=\"black\" face=\"");
    result.append("Nokia Sans");
    result.append("\"size=\"3\">"); 
    // Set text in HTML format based on layout direction
    if (qApp->layoutDirection()==Qt::RightToLeft){
        result.append("<p style=\"margin-right: '0'; margin-left: '0'\" dir=\"rtl\"><b>"); 
        result.append(hbTrId("txt_mail_list_from"));
        result.append(" </b>"); 
        result.append(sender);
        result.append("<br><b>"); 
        result.append(hbTrId("txt_mail_list_to"));
        result.append(" </b>");
        int reciCount = to.count();
        for (int i=0; i < reciCount; i++) { 
            result.append(addressToDisplay(to[i]));
            if (i!=reciCount-1) {
                result.append(" ");
            }
        } 
        reciCount = cc.count();
        if (reciCount) {
            result.append("<br><b>");
            result.append(hbTrId("txt_mail_list_cc"));
            result.append(" </b>");
            for (int i=0; i < reciCount; i++) {
                result.append(addressToDisplay(cc[i]));
                if (i!=reciCount-1) {
                    result.append(" ");
                }                
            }
        }   
    }
    else{
        result.append("<p style=\"margin-right: '0'; margin-left: '0'\" dir=\"ltr\"><b>"); 
        result.append(hbTrId("txt_mail_list_from"));
        result.append(" </b>"); 
        result.append(sender);
        result.append("<br><b>"); 
        result.append(hbTrId("txt_mail_list_to"));
        result.append(" </b>");
        int reciCount = to.count();
        for (int i=0; i < reciCount; i++) { 
            result.append(addressToDisplay(to[i]));
            if (i!=reciCount-1) {
                result.append("; ");
            }
        }
        reciCount = cc.count();
        if (reciCount) {
            result.append("<br><b>");
            result.append(hbTrId("txt_mail_list_cc"));
            result.append(" </b>");
            for (int i=0; i < reciCount; i++) {
                result.append(addressToDisplay(cc[i]));
                if (i!=reciCount-1) {
                    result.append("; ");
                }
            }
        }   
    } 
    result.append("</p></font></body></html>");     
    return result;
}

/*!
    Function retursn address string from NmAddress to
    be displayed in HTML format
*/
QString NmViewerHeader::addressToDisplay(const NmAddress &addr)
{
    NM_FUNCTION;
    
    QString dispName;
    if (addr.displayName().length()!=0){
        dispName.append(NmUtilities::cleanupDisplayName(addr.displayName()));
    }
    else{
        dispName.append(addr.address());
    }
    QString ret="<a href=mailto:";
    ret.append(addr.address());
    ret.append(">");
    ret.append(dispName);
    ret.append("</a>");
    return ret;
}

/*!
    Function handles cursor position changes in header group box.
    E.g link handler.
*/
void NmViewerHeader::anchorTapped(const QString &anchor)
{
    if (mViewerView&&anchor.contains("mailto:",Qt::CaseSensitive)){
        QUrl url(anchor);     
        mViewerView->linkClicked(url);
    }
}

