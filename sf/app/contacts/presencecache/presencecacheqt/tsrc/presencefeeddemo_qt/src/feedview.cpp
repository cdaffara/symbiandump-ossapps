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

#include "feedview.h"

#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbmainwindow.h>

#include <QTimer>

#include <prcpresencebuddyinfo_qt.h>
#include <prcpresencereader_qt.h>
#include <prcpresencewriter_qt.h>

FeedView::FeedView(QObject *parent):
    QObject(parent),
    mView(NULL),
    iWriter(NULL),
    iReader(NULL),
    mManager(NULL),
    mUpdateTimer(NULL),
    mStarted(false)
{
    bool ok = false;
    mDocumentLoader.load(":/xml/feedview.docml", &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/feedview.docml");
    }

    HbPushButton *create = static_cast<HbPushButton*>(mDocumentLoader.findObject("createContactsButton"));
    connect(create, SIGNAL(clicked()), this, SLOT(createPressed()));

    HbPushButton *start = static_cast<HbPushButton*>(mDocumentLoader.findObject("startButton"));
    connect(start, SIGNAL(clicked()), this, SLOT(startPressed()));

    HbPushButton *stop = static_cast<HbPushButton*>(mDocumentLoader.findObject("stopButton"));
    connect(stop, SIGNAL(clicked()), this, SLOT(stopPressed()));

    iWriter = PrcPresenceWriter::createWriter();
    iReader = PrcPresenceReader::createReader();

    mManager = new QContactManager("symbian");

    mUpdateTimer = new QTimer();
    mUpdateTimer->setInterval(2000);
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateRandomPresence()));
}

FeedView::~FeedView()
{
    delete mView;
    delete iWriter;
    delete iReader;
	delete mManager;
    delete mUpdateTimer;
}

void FeedView::startPressed()
{
    if (!mStarted)
	{
        static_cast<HbPushButton*>(mDocumentLoader.findObject("createContactsButton"))->setEnabled(false);

        HbMainWindow* window = mView->mainWindow();
        window->setInteractive(false);
        HbLabel *label = static_cast<HbLabel*>(mDocumentLoader.findObject("textLabel"));
        label->setPlainText("Presence feeder active.");
    
        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);
    
        QList<QContactLocalId> contacts = mManager->contactIds(filter);
    
        foreach (QContactLocalId id, contacts)
        {
            QContact contact = mManager->contact(id);
    
            QList<QContactOnlineAccount> accounts = contact.details<QContactOnlineAccount>();
    
            foreach (QContactOnlineAccount account, accounts)
            {
                PrcPresenceBuddyInfoQt* buddy = PrcPresenceBuddyInfoQt::createInstance();
                
                QString fullAccount = account.serviceProvider() + ':' + account.accountUri();
                buddy->setIdentity(fullAccount);
                mBuddyList.append(fullAccount);
                
                if (qrand() % 2 == 0)
                {
                    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcNotAvailable, "hep");
                }
                else
                {
                    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcAvailable, "hep");
                }
                iWriter->writePresence( *buddy );
                
                delete buddy;
            }
        }

        window->setInteractive(true);
        mUpdateTimer->start();
        mStarted = true;
    }
}

void FeedView::stopPressed()
{
    HbLabel *label = static_cast<HbLabel*>(mDocumentLoader.findObject("textLabel"));
    label->setPlainText("Presence feeder not active.");
    mBuddyList.clear();
    mUpdateTimer->stop();
    mStarted = false;
    static_cast<HbPushButton*>(mDocumentLoader.findObject("createContactsButton"))->setEnabled(true);
}

void FeedView::createPressed()
{
    QContact c;
    QContactName name;
    name.setFirstName("sip");
    name.setLastName("test");
    c.saveDetail(&name);
    QContactPhoneNumber number;
    number.setNumber("1234567");
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number);
    QContactOnlineAccount account;
    account.setSubTypes(QStringList() << QContactOnlineAccount::SubTypeSip);
    account.setServiceProvider("sip");
    account.setAccountUri("test@test.com");
    c.saveDetail(&account);
    mManager->saveContact(&c);

    QContact c2;
    QContactName name2;
    name2.setFirstName("two");
    name2.setLastName("accounts");
    c2.saveDetail(&name2);
    QContactOnlineAccount account2;
    account2.setSubTypes(QStringList() << QContactOnlineAccount::SubTypeImpp);
    account2.setServiceProvider("yahoo");
    account2.setAccountUri("test@yahoo.com");
    c2.saveDetail(&account2);
    QContactOnlineAccount account3;
    account3.setSubTypes(QStringList() << QContactOnlineAccount::SubTypeImpp);
    account3.setServiceProvider("msn");
    account3.setAccountUri("test@msn.com");
    c2.saveDetail(&account3);
    mManager->saveContact(&c2);

    HbLabel *label = static_cast<HbLabel*>(mDocumentLoader.findObject("textLabel"));
    label->setPlainText("Contacts created.");
}

void FeedView::updateRandomPresence()
{
    int random = qrand() % mBuddyList.count();

    PrcPresenceBuddyInfoQt* buddy = iReader->presenceInfo(mBuddyList.at(random));
    
    if (qrand() % 2 == 0)
    {
        buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcNotAvailable, "hep");
    }
    else
    {
        buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcAvailable, "hep");
    }
    iWriter->writePresence( *buddy );
    
    delete buddy;
}
