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
#include <QtGui>
#include <QDebug>
#include <e32base.h>
#include <qcontactid.h>
#include "qcontact.h"
#include "cphcntcontactidimpl2.h"



//---- test class

class U_CPhCntContactIdImpl2: public QObject
{
Q_OBJECT

public:
    U_CPhCntContactIdImpl2();
	~U_CPhCntContactIdImpl2();

private slots:
	void initTestCase();
    void cleanupTestCase();
    void testValidContact();
    void testClone();
    void testPackLC();
    void testInvalidate();

	
private:
	
    QContact mContact;
    CPhCntContactId* mContactId; //SUT 
};



U_CPhCntContactIdImpl2::U_CPhCntContactIdImpl2()
    {

    }

U_CPhCntContactIdImpl2::~U_CPhCntContactIdImpl2()
    {

    }

void U_CPhCntContactIdImpl2::initTestCase()
    {
    
    TRAP_IGNORE( mContactId = CPhCntContactIdImpl2::NewL(mContact) );
    }

void U_CPhCntContactIdImpl2::cleanupTestCase()
    {
	delete mContactId;
	
    }

void U_CPhCntContactIdImpl2::testValidContact()
    {
    Q_ASSERT(mContactId->IsValid());
    Q_ASSERT(mContactId->ContactId() == mContact.localId()); 
    }

void U_CPhCntContactIdImpl2::testClone()
    {
     CPhCntContactId* clone = NULL;
     clone = mContactId->CloneL();
     Q_ASSERT(clone->ContactId() == mContactId->ContactId());
     delete clone; 
    }

void U_CPhCntContactIdImpl2::testPackLC()
    {
    HBufC8* buf = NULL;
    qDebug() << "mContactId->PackLC() calling";
    TRAP_IGNORE( 
        buf = mContactId->PackLC(); 
        if (buf != NULL)
            {
            qDebug() << "mContactId->PackLC() done";
            CleanupStack::Pop(buf);
            TBuf8<100> buf1;
            buf1.AppendNum(mContactId->ContactId());
            Q_ASSERT(buf->Des() == buf1 );
            delete buf;
            buf = NULL;
            }
        else 
            {
            qDebug() << "mContactId->PackLC()returned NULL";
            }
    );
    }
 
void U_CPhCntContactIdImpl2::testInvalidate()
    {
    mContactId->Invalidate();
    Q_ASSERT(!mContactId->IsValid());
    Q_ASSERT(mContactId->ContactId() == KErrNotFound); 
    Q_ASSERT(mContactId->PackLC() == NULL);
    }

QTEST_MAIN(U_CPhCntContactIdImpl2)
#include "u_cphcntcontactidimpl2.moc"
