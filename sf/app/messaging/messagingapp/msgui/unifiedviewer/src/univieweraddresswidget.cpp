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
 * Description:Custom widget derived from HbTextEdit which provides rich text
 *              processing
 *
 */

#include "univieweraddresswidget.h"

// SYSTEM INCLUDES
#include <QTextCursor>
#include <QTextBlock>
#include <QApplication>
#include <QClipboard>

#include <HbMenu>
#include <HbAction>
#include <HbTapGesture>

#include <xqservicerequest.h>
#include <xqappmgr.h>
#include <cntservicescontact.h>
#include <qtcontacts.h>
#include <xqservicerequest.h>
#include <xqaiwrequest.h>
#include "msgcontacthandler.h"

// LOCAL CONSTANTS
const QString ADDRESS_SEPARATOR("; ");
const QString ADDRESS_OPEN(" (");
const QString ADDRESS_CLOSE(")");
const QString SPACE(" ");

//localization
#define LOC_CONTACT_INFO      hbTrId("txt_messaging_menu_contact_info")
#define LOC_CALL              hbTrId("txt_common_menu_call_verb")
#define LOC_SEND_MESSAGE      hbTrId("txt_common_menu_send_message")
#define LOC_SAVE_TO_CONTACTS  hbTrId("txt_common_menu_save_to_contacts")
#define LOC_COPY              hbTrId("txt_common_menu_copy")

const QString BG_FRAME_GRAPHICS("qtg_fr_lineedit_normal");

//---------------------------------------------------------------
// UniViewerAddressWidget::UniViewerAddressWidget
// @see header file
//---------------------------------------------------------------
UniViewerAddressWidget::UniViewerAddressWidget(QGraphicsItem * parent) :
HbTextEdit(parent),
mCursorPos(-1)
{
    this->setReadOnly(true);    
    this->setScrollable(false);
    this->setFlag(QGraphicsItem::ItemIsFocusable,false);
    this->setCursorVisibility(Hb::TextCursorHidden);
    this->setBackgroundItem(0);   

    HbFontSpec fontSpec(HbFontSpec::Secondary);
    QFont font = fontSpec.font();
    this->setFont(font);

    mFormatNormal.setForeground(palette().link());
    mFormatNormal.setBackground(Qt::transparent);

    mFormatHighlight.setBackground(palette().highlight());
    mFormatHighlight.setForeground(palette().highlightedText());

    connect(this, SIGNAL(aboutToShowContextMenu(HbMenu*,QPointF)),
            this, SLOT(aboutToShowContextMenu(HbMenu*,QPointF)));
}

//---------------------------------------------------------------
// UniViewerAddressWidget::~UniViewerAddressWidget
// @see header file
//---------------------------------------------------------------
UniViewerAddressWidget::~UniViewerAddressWidget()
{
}

//---------------------------------------------------------------
//UniViewerAddressWidget :: gestureEvent
// @see header file
//---------------------------------------------------------------
void UniViewerAddressWidget::gestureEvent(QGestureEvent* event)
{
    //handle gesture to highlight and dehighlight find item.
    
    if(HbTapGesture *tap = qobject_cast<HbTapGesture*>(event->gesture(Qt::TapGesture)))
    {
        //capturing gesture position, and map to local co-ordinates.
        QPointF pos = mapFromScene(tap->scenePosition());
        
        switch (tap->state()) 
        {
            case Qt::GestureStarted:
            {
                //highlight find item.
                QTextDocument* doc = this->document();
                mCursorPos = doc->documentLayout()->hitTest(pos, Qt::ExactHit);
                highlightText(true);
                break;
            }  
            
            case Qt::GestureFinished:
            {
                if (HbTapGesture::Tap == tap->tapStyleHint()) 
                {
                    //gesture is finshed dehighlight text.
                    highlightText(false);
                    
                    QString anchor = this->anchorAt(pos);
                    
                    //do short tap action.
                    if (!anchor.isEmpty() && !this->textCursor().hasSelection())
                    {
                        shortTapAction(anchor,tap->scenePosition());
                    }
                }
                break;
            }
            
            case Qt::GestureCanceled:
            {
                //gesture is canceled due to pan or swipe, dehighlight text.
                if (HbTapGesture::Tap == tap->tapStyleHint()) 
                {
                highlightText(false);
                break;
                }
            }
            default:
                break;
        }
        
        event->accept();
    }
    else
    {
        event->ignore();
    }
    
    //passing gesture event to base class.
    HbTextEdit::gestureEvent(event);
}


//----------------------------------------------------------------------------
// UniViewerAddressWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::populate(const QString &label,
                                      const QString &address,
                                      const QString &alias)
{
    QString labelText = label;
    labelText.trimmed();
    labelText += SPACE;
    
    //Font.
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    qreal fontHeight = 0.0;
    style()->parameter("hb-param-text-height-tiny", fontHeight);
    fontSpec.setTextHeight(fontHeight);    
    QFont font = fontSpec.font();
    
    QTextCharFormat labelFormat;
    labelFormat.setFont(font);
    
    QTextCharFormat addressFormat;
    addressFormat.setForeground(palette().link());
    addressFormat.setFontUnderline(true);

    // Insert the label then the addresses
    QTextCursor cursor(this->textCursor());
    cursor.insertText(labelText,labelFormat);
    
    QString address1 = QString();
    if (!(alias.isEmpty()))
    {
        address1.append(alias);
        QString alias1 = QString();
        
        int totalNumbers = 0;
        MsgContactHandler::resolveContactDisplayName(
                        address,
                        alias1,
                        totalNumbers);
        if (totalNumbers > 1)
        {
            address1.append(ADDRESS_OPEN);
            address1.append(address);
            address1.append(ADDRESS_CLOSE);
        }
    }
    else
    {
        address1.append(address);
    }
    addressFormat.setAnchorHref(address);
    cursor.insertText(address1, addressFormat);
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::populate(const QString &label,
                                      ConvergedMessageAddressList addressList)
{
    QString labelText = label;
    labelText.trimmed();
    labelText += SPACE;
    
    //Font.
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    qreal fontHeight = 0.0;
    style()->parameter("hb-param-text-height-tiny", fontHeight);
    fontSpec.setTextHeight(fontHeight);    
    QFont font = fontSpec.font();
    
    QTextCharFormat labelFormat;
    labelFormat.setFont(font);
    
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(palette().link());

    QTextCharFormat addressFormat;
    addressFormat.setForeground(palette().link());
    addressFormat.setFontUnderline(true);

    // Insert the label then the addresses
    QTextCursor cursor(this->document());
    cursor.insertText(labelText,labelFormat);

    int addressCount = addressList.count();

    for (int i = 0; i < addressCount; ++i)
    {

        QString address = QString();
        if (! (addressList[i]->alias().isEmpty()))
        {
            address.append(addressList[i]->alias());
            QString alias = QString();
            
            int totalNumbers = 0;
            MsgContactHandler::resolveContactDisplayName(
                            addressList[i]->address(),
                            alias,
                            totalNumbers);
            if (totalNumbers > 1)
            {
                address.append(ADDRESS_OPEN);
                address.append(addressList[i]->address());
                address.append(ADDRESS_CLOSE);
            }
        }
        else
        {
            address.append(addressList[i]->address());
        }

        addressFormat.setAnchorHref(addressList[i]->address());
        cursor.insertText(address, addressFormat);

        if (addressCount - 1 != i)
        {
            cursor.insertText(ADDRESS_SEPARATOR, defaultFormat);
        }
        
    }
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::clearContent
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::clearContent()
{
    this->document()->clear();
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::menuClosed
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::menuClosed()
{
    highlightText(false);
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::highlightText
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::highlightText(bool highlight)
{
    QTextBlock textBlock = this->document()->findBlock(mCursorPos);

    QTextBlock::iterator it;

    for (it = textBlock.begin(); !(it.atEnd()); ++it)
    {
        QTextFragment currentFragment = it.fragment();
        
        if (currentFragment.isValid() && currentFragment.contains(mCursorPos)
            && currentFragment.charFormat().fontUnderline())
        {
            int start = currentFragment.position();
            int length = currentFragment.length();

            QTextCursor cursor = this->textCursor();
            cursor.clearSelection();
            cursor.setPosition(start);
            cursor.setPosition(start + length,QTextCursor::KeepAnchor);

            if(highlight)
            {
                cursor.mergeCharFormat(mFormatHighlight);
            }
            else
            {
                cursor.mergeCharFormat(mFormatNormal);
            }

            cursor.clearSelection();
            break;
        }
    }
}

void UniViewerAddressWidget::aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos)
{
    //remove default actions.
    contextMenu->clearActions();
    
    // Check if there is an anchor at this pos
    QString  anchor = this->anchorAt(pos);

    if(!anchor.isEmpty() && !this->textCursor().hasSelection())
    {
        populateMenu(contextMenu,anchor);
    }
}

void UniViewerAddressWidget::populateMenu(HbMenu* contextMenu,const QString& data)
{
    HbAction* action = NULL;

    int contactId = MsgContactHandler::resolveContactDisplayName(
                                 data, 
                                 QContactPhoneNumber::DefinitionName,
                                 QContactPhoneNumber::FieldNumber); 
    
    if(contactId > 0)
    {
        action = contextMenu->addAction(LOC_CONTACT_INFO, this, SLOT(openContactInfo()));
        action->setData(data);
    }
    else
    {
        action = contextMenu->addAction(LOC_SAVE_TO_CONTACTS, this, SLOT(saveToContacts()));
        action->setData(data);  
    }

    action = contextMenu->addAction(LOC_CALL, this, SLOT(call()));
    action->setData(data);

    action = contextMenu->addAction(LOC_SEND_MESSAGE, this, SLOT(sendMessage()));
    action->setData(data);

    action = contextMenu->addAction(LOC_COPY, this, SLOT(copyToClipboard()));
    action->setData(data);
    
    connect(contextMenu,SIGNAL(aboutToClose()),this,SLOT(menuClosed())); 
}

void UniViewerAddressWidget::shortTapAction(QString anchor,const QPointF& pos)
{
    HbAction action;
    action.setData(anchor);


    int contactId = MsgContactHandler::resolveContactDisplayName(
        anchor, 
        QContactPhoneNumber::DefinitionName,
        QContactPhoneNumber::FieldNumber);        

    if(contactId > 0 )
    {
        //if resolved conatct open contact card 
        connect(&action,SIGNAL(triggered()),this,SLOT(openContactInfo()));
    }
    else
    {
        //unresolved contact show popup.  
        highlightText(true);

        HbMenu* contextMenu = new HbMenu();
        contextMenu->setDismissPolicy(HbPopup::TapAnywhere);
        contextMenu->setAttribute(Qt::WA_DeleteOnClose, true);
        contextMenu->setPreferredPos(pos); 

        populateMenu(contextMenu,anchor);
        
        contextMenu->show();
    }

    action.trigger();
}

void UniViewerAddressWidget::copyToClipboard()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QMimeData* data = new QMimeData();
        QString str = action->data().toString();
        data->setText(str);
        QApplication::clipboard()->setMimeData(data);
    }
}

void UniViewerAddressWidget::call()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString phoneNumber = action->data().toString();
        
        //invoke dialer service and pass phoneNumber.        
        QString serviceName("com.nokia.symbian.ICallDial");
        QString operation("dial(QString)");
        
        XQServiceRequest* serviceRequest = new XQServiceRequest(serviceName,operation,false);
        
        connect(serviceRequest, SIGNAL(requestCompleted(QVariant)),
                this, SLOT(onServiceRequestCompleted()));
        
        connect(serviceRequest, SIGNAL(requestError(int)),
                this, SLOT(onServiceRequestCompleted()));
        
        *serviceRequest << phoneNumber;
        serviceRequest->send();
    }
}

void UniViewerAddressWidget::onServiceRequestCompleted()
    {
    //service request is now complete. delete it.    
    XQServiceRequest* request = qobject_cast<XQServiceRequest*>(sender());
    
    if(request)
        {
        delete request;
        }
    }


void UniViewerAddressWidget::openContactInfo()
{
    HbAction* action = qobject_cast<HbAction*>(sender());
    
    if(action)
    {
        QString data = action->data().toString();        
    
        int contactId = MsgContactHandler::resolveContactDisplayName(
                data,
                QContactPhoneNumber::DefinitionName,
                QContactPhoneNumber::FieldNumber);

        // if contact is unresolved on phone number field
        // then, try resolving it on email address field
        if(contactId <= 0)
        {
            contactId = MsgContactHandler::resolveContactDisplayName(
                data,
                QContactEmailAddress::DefinitionName,
                QContactEmailAddress::FieldEmailAddress);
        }

        //service stuff.
        QString service("phonebookservices");
        QString interface;
        QString operation;
        QList<QVariant> args;

        if(contactId > 0)
        {
            //open contact card
            interface = QString("com.nokia.symbian.IContactsView");
            operation = QString("openContactCard(int)");
            args << contactId;
        }
        else
        {
            //save to contacts with phone number field prefilled.
            interface = QString("com.nokia.symbian.IContactsEdit");
            operation = QString("editCreateNew(QString,QString)");
            QString type = QContactPhoneNumber::DefinitionName;
            args << type;
            args << data;
        }
        XQAiwRequest* request;
        XQApplicationManager appManager;
        request = appManager.create(service, interface, operation, true); // embedded
        if ( request == NULL )
            {
            return;       
            }

        // Result handlers
        connect (request, SIGNAL(requestOk(const QVariant&)), 
            this, SLOT(handleOk(const QVariant&)));
        connect (request, SIGNAL(requestError(const QVariant&)), 
            this, SLOT(handleError(const QVariant&)));
        
        request->setArguments(args);
        request->send();
        delete request;
    }
}

void UniViewerAddressWidget::handleOk(const QVariant& result)
    {
    Q_UNUSED(result)
    }

void UniViewerAddressWidget::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
    }

void UniViewerAddressWidget::saveToContacts()
{
    openContactInfo();
}

void UniViewerAddressWidget::sendMessage()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString phoneNumber = action->data().toString();
        QString alias;

        QTextBlock textBlock = this->document()->findBlock(mCursorPos);

        QTextBlock::iterator it;

        for (it = textBlock.begin(); !(it.atEnd()); ++it)
        {
            QTextFragment currentFragment = it.fragment();

            if (currentFragment.isValid() && currentFragment.contains(mCursorPos)
                && currentFragment.charFormat().fontUnderline())
            {
                QString txt = currentFragment.text();
                if(txt != phoneNumber)
                {
                    alias = txt;  
                }
                break;
            }
        }
        

        //invoke editor & pass phoneNumber.
        emit sendMessage(phoneNumber,alias);
    }
}

// EOF
