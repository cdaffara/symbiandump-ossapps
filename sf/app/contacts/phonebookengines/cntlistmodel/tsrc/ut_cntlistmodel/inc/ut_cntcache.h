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

#include <QtTest/QtTest>
#include <QObject>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

static const int CntTestContacts = 6;

class TestCntCache : public QObject
{
    Q_OBJECT

private:
    void cleanDatabase();
    QContact createContact(QString firstName, QString lastName, QString phoneNumber, QString imageName);

private slots:
	void initTestCase();
	void cleanupTestCase();
    
    void construction();
    void fetchContactInfo();
    void clearCache();
	
private:
    QContactManager *mContactManager;
    QContact mContacts[CntTestContacts];
    QList<int> mContactOrder;
};
