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
 * Message body preview class.
 * has in built logic for finding phone number, email, url.
 *
 */

#include "univiewertextitem.h"
#include "msgcontacthandler.h"

#include <QRegExp>
#include <QApplication>
#include <QClipboard>
#include <QTextBlock>
#include <QUrl>
#include <QDesktopServices>
#include <HbTapGesture>
#include <HbMenu>
#include <HbAction>
#include <cntservicescontact.h>
#include <xqservicerequest.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>


//consts

//regexp
const QString NUMBER_PATTERN("(\\(?(\\+|\\d))((?:\\d)((?:[\\s-/.\\)\\(])*(?:(\\d+|\\))))*(?:\\d?[^\\D]|\\)))|((\\*#)(?:\\d+(\\*|#)(?:\\d+#)?))");

const QString EMAIL_PATTERN("[a-zA-Z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-zA-Z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]*[a-zA-Z0-9])?\\.)+[a-zA-Z0-9](?:[a-zA-Z0-9-]*[a-zA-Z0-9])?");

const QString URL_PATTERN("(((ht|f|rt)(tp|sp)(s?)\\:\\/\\/)|(www|wap)(?:\\.))(([-\\w]*[0-9a-zA-Z])+(:(0-9)*)*(\\/?)([a-zA-Z0-9\\-\\?\'\\/\\\\+&amp;%\\$#_=~]*)(\\.)([-\\w]*[0-9a-zA-Z])+(:(0-9)*)*(\\/?)([a-zA-Z0-9\\-\\?\'\\/\\\\+&amp;%\\$#_=~]*))+[a-zA-Z0-9/]");

//rules
const QString URL_RULE("0_URL_RULE");
const QString EMAIL_RULE("1_EMAIL_RULE");
const QString NUMBER_RULE("2_NUMBER_RULE");


//localization
#define LOC_CONTACT_INFO hbTrId("txt_messaging_menu_contact_info")
#define LOC_CALL              hbTrId("txt_common_menu_call_verb")
#define LOC_SEND_MESSAGE      hbTrId("txt_common_menu_send_message")
#define LOC_SAVE_TO_CONTACTS  hbTrId("txt_common_menu_save_to_contacts")
#define LOC_COPY              hbTrId("txt_common_menu_copy")
#define LOC_CREATE_EMAIL      hbTrId("txt_messaging_menu_create_mail")
#define LOC_OPEN_LINK         hbTrId("txt_messaging_menu_open_link")
#define LOC_ADD_TO_BOOKMARKS  hbTrId("txt_messaging_menu_add_to_bookmarks")
#define LOC_COPY_LINK         hbTrId("txt_messaging_menu_copy_link")

UniViewerTextItem::UniViewerTextItem(QGraphicsItem* parent):
HbTextEdit(parent),
mFindOn(true),
mCursorPos(-1)
{
    this->setReadOnly(true);
    this->setScrollable(false);
    this->setCursorVisibility(Hb::TextCursorHidden);
    this->setFlag(QGraphicsItem::ItemIsFocusable,false);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setBackgroundItem(0);   

    //inserting rules and patterns to map.
    mRules.insert(NUMBER_RULE,NUMBER_PATTERN);
    mRules.insert(EMAIL_RULE,EMAIL_PATTERN);
    mRules.insert(URL_RULE,URL_PATTERN);

    mFormatNormal.setForeground(palette().link());
    mFormatNormal.setBackground(Qt::transparent);

    mFormatHighlight.setBackground(palette().highlight());
    mFormatHighlight.setForeground(palette().highlightedText());

    connect(this, SIGNAL(aboutToShowContextMenu(HbMenu*,QPointF)),
            this, SLOT(aboutToShowContextMenu(HbMenu*,QPointF)));
}

UniViewerTextItem::~UniViewerTextItem()
    {
    }

void UniViewerTextItem::setFindOn(bool on)
{
    mFindOn = on;   
}

void UniViewerTextItem::setText(const QString& text)
{
    
    this->setPlainText(text);
    applyRule();
}

void UniViewerTextItem::applyRule()
{

    if(mFindOn == false)
    {
        return;
    }

    QMap<QString, QString>::const_iterator i;
    for (i = mRules.constBegin(); i != mRules.constEnd(); ++i)
    {
        QString ruleName = i.key();
        QString rule = i.value();
        QRegExp ruleExp(rule);
        ruleExp.setCaseSensitivity(Qt::CaseInsensitive);

        QTextCursor cursor = this->document()->find(ruleExp);

        while(cursor.hasSelection())
        {
            int p = cursor.position();
            QString  anchor = this->anchorAt(p-1);
            
            //not identified yet.
            if(anchor.isEmpty())
            {
                // Insert anchor in the document
                QTextCharFormat f;
                f.setFontUnderline(true);
                f.setForeground(palette().link());

                //prepending rule name to identiy different fragment to which
                //catagory it belongs to.
                QString txt = cursor.selectedText().prepend(ruleName);

                if(ruleName == NUMBER_RULE)
                {
                    //removing special char(s) from phone numbers.
                    QRegExp numberCharExp("[\\s-/.\\(\\)]");
                    txt = txt.remove(numberCharExp);
                }

                f.setAnchorHref(txt);
                f.setAnchor(true);
                cursor.mergeCharFormat(f);
            }

            // Find next
            cursor = this->document()->find(ruleExp, cursor);
        }
    }
}


void UniViewerTextItem::aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos)
{
    // Check if there is an anchor at this pos
    QString  anchor = this->anchorAt(pos);

    if(!anchor.isEmpty() && !this->textCursor().hasSelection())
    {
        //remove default actions.
        contextMenu->clearActions();

        if(anchor.contains(QRegExp("^"+NUMBER_RULE)))
        {
            addNumberMenu(contextMenu,anchor);
        }
        else if(anchor.contains(QRegExp("^"+EMAIL_RULE)))
        {
            addEmailMenu(contextMenu,anchor);
        }
        else if(anchor.contains(QRegExp("^"+URL_RULE)))
        {
            addUrlMenu(contextMenu,anchor);
        }
    }

    connect(contextMenu,SIGNAL(aboutToClose()),this,SLOT(menuClosed()));
    
}

void UniViewerTextItem::gestureEvent(QGestureEvent* event)
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
                        handleShortTap(anchor,tap->scenePosition());
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

void UniViewerTextItem::addNumberMenu(HbMenu* contextMenu,const QString& data)
{
    HbAction* action = NULL;

    QString number = data;
    number.remove(NUMBER_RULE);
    int contactId = MsgContactHandler::resolveContactDisplayName(
                                 number, 
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
}

void UniViewerTextItem::addEmailMenu(HbMenu* contextMenu,const QString& data)
{
    HbAction* action = NULL;
    
    action = contextMenu->addAction(LOC_CREATE_EMAIL, this, SLOT(createEmail()));
    action->setData(data);
    
    action = contextMenu->addAction(LOC_CONTACT_INFO, this, SLOT(openContactInfo()));
    action->setData(data);

    action = contextMenu->addAction(LOC_SAVE_TO_CONTACTS, this, SLOT(saveToContacts()));
    action->setData(data);

    action = contextMenu->addAction(LOC_COPY, this, SLOT(copyToClipboard()));
    action->setData(data);
}

void UniViewerTextItem::addUrlMenu(HbMenu* contextMenu,const QString& data)
{
    HbAction* action = NULL;
    
    action = contextMenu->addAction(LOC_OPEN_LINK, this, SLOT(openLink()));
    action->setData(data);

    action = contextMenu->addAction(LOC_ADD_TO_BOOKMARKS, this, SLOT(addToBookmarks()));
    action->setData(data);

    action = contextMenu->addAction(LOC_COPY_LINK, this, SLOT(copyToClipboard()));
    action->setData(data);
}

void UniViewerTextItem::handleShortTap(QString anchor,const QPointF& pos)
{
    HbAction action;
    action.setData(anchor);

    if(anchor.contains(NUMBER_RULE))
    {
        QString data = anchor;
        data.remove(NUMBER_RULE);
        int contactId = MsgContactHandler::resolveContactDisplayName(
                                     data, 
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
            connect(contextMenu,SIGNAL(aboutToClose()),this,SLOT(menuClosed()));
            
            addNumberMenu(contextMenu,anchor);
            
            contextMenu->show();
        }
    }
    else if(anchor.contains(EMAIL_RULE))
    {
        //Create email
        connect(&action,SIGNAL(triggered()),this,SLOT(createEmail()));
    }
    else if(anchor.contains(URL_RULE))
    {
        //Open link in the browser
        connect(&action,SIGNAL(triggered()),this,SLOT(openLink()));
    }
    
    action.trigger();
}


void UniViewerTextItem::copyToClipboard()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QMimeData* data = new QMimeData();
        QString str = action->data().toString();

        //removing rule name before setting it to clipboard.
        str.remove(QRegExp("^"+NUMBER_RULE));
        str.remove(QRegExp("^"+EMAIL_RULE));
        str.remove(QRegExp("^"+URL_RULE));

        data->setText(str);
        QApplication::clipboard()->setMimeData(data);
    }
}

void UniViewerTextItem::call()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString phoneNumber = action->data().toString();
        phoneNumber.remove(NUMBER_RULE);
        
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

void UniViewerTextItem::sendMessage()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString phoneNumber = action->data().toString();
        phoneNumber.remove(NUMBER_RULE);
        
        //invoke editor & pass phoneNumber.
        emit sendMessage(phoneNumber);
    }
}

void UniViewerTextItem::createEmail()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if ( action )
    {
        QString emailId = action->data().toString();
        emailId.remove(EMAIL_RULE);

        // Launch email editor
        QString interfaceName("com.nokia.symbian.IEmailMessageSend");
        QString operation("send(QVariant)");
        XQApplicationManager appManager;
        XQAiwRequest* request = appManager.create(interfaceName, 
			operation, true); 
        if ( request == NULL )
            {
            return;       
            }
    
        // Fill args
        QStringList recipients;
        recipients.append(emailId);
    
        QMap<QString,QVariant> map;
        map.insert(QString("to"),recipients);
    
        QList<QVariant> args;
        args.append(map);
        
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

void UniViewerTextItem::openLink()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString url = action->data().toString();
        url.remove(URL_RULE);
        //invoke browser service  & pass url.
        QUrl uri(url);
        QDesktopServices::openUrl(uri);
    }
}

void UniViewerTextItem::addToBookmarks()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString url = action->data().toString();
        url.remove(URL_RULE);
        //invoke browser service to save url.
    }
}

void UniViewerTextItem::openContactInfo()
{
    HbAction* action = qobject_cast<HbAction*>(sender());
    
    if(action)
    {
        //service stuff.
        QString service("phonebookservices");
        QString interface;
        QString operation;
        QList<QVariant> args;
        
        QString data = action->data().toString();
        
        if(data.contains(QRegExp("^"+NUMBER_RULE)))
        {
            data.remove(NUMBER_RULE);
    
            int contactId = MsgContactHandler::resolveContactDisplayName(
                                         data, 
                                         QContactPhoneNumber::DefinitionName,
                                         QContactPhoneNumber::FieldNumber);
    
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
        }
        else if(data.contains(QRegExp("^"+EMAIL_RULE)))
        {
            data.remove(EMAIL_RULE);
    
            int contactId = MsgContactHandler::resolveContactDisplayName(
                    data,
                    QContactEmailAddress::DefinitionName,
                    QContactEmailAddress::FieldEmailAddress);
            
            if(contactId > 0)
                {
                //open contact card
                interface = QString("com.nokia.symbian.IContactsView");
                operation = QString("openContactCard(int)");
                args << contactId;
                }
            else
                {
                //save to contacts with e-mail field prefilled.
                interface = QString("com.nokia.symbian.IContactsEdit");
                operation = QString("editCreateNew(QString,QString)");
                QString type = QContactEmailAddress::DefinitionName;
                args << type;
                args << data;
                }
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


void UniViewerTextItem::saveToContacts()
{
     openContactInfo();
}

void UniViewerTextItem::onServiceRequestCompleted()
    {
    //service request is now complete. delete it.
    
    XQServiceRequest* request = qobject_cast<XQServiceRequest*>(sender());
    
    if(request)
        {
        delete request;
        }
    }

void UniViewerTextItem::menuClosed()
{
    highlightText(false);
}

void UniViewerTextItem::highlightText(bool highlight)
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

//---------------------------------------------------------------
// UniViewerTextItem::handleOk
//
//---------------------------------------------------------------
void UniViewerTextItem::handleOk(const QVariant& result)
    {
    Q_UNUSED(result)
    }

//---------------------------------------------------------------
// UniViewerTextItem::handleError
// 
//---------------------------------------------------------------
void UniViewerTextItem::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
    }
