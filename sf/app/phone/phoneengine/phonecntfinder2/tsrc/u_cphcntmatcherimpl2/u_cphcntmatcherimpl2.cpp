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

#include <QtGui>
#include <QtTest/QtTest>
#include <QDebug>
#include <qcontactfields.h>
#include <qcontact.h>
#include "qcontactmanager_moc.h"
#include "cphcntmatch2.h"
#include "cphcntcontactidimpl2.h"
#include "cphcntmatcherimpl2.h"




//---- test class

class U_CPhCntMatcherImpl2: public QObject
{
Q_OBJECT

public:
    U_CPhCntMatcherImpl2();
	~U_CPhCntMatcherImpl2();

private slots:
	void initTestCase();
    void cleanupTestCase();
    void test_MatchNumberFalse(); //test basic match
    void test_MatchNumberExact(); //1 exact match
    void test_MatchNumberSeveralReturned(); //returns 2 matches
    void test_MatchNumber1(); //test match when contacid is given
    void test_MatchNumber2(); //test match 3 overload
    void test_MatchVoipNumber();
    void test_MatchVoipNumber1(); // test 2nd overload
    void test_HasCSNumbers();
	void test_MatchNumberWithContactId(); //now happy day scenario, cannot test if the contact is valid
	
private:
	
    CPhCntMatcherImpl2* mMatcher; // implementation under test
    QContactManager_Moc* mManager;

};

//--- testing

U_CPhCntMatcherImpl2::U_CPhCntMatcherImpl2()
    {
    }

U_CPhCntMatcherImpl2::~U_CPhCntMatcherImpl2()
    {
    }

void U_CPhCntMatcherImpl2::initTestCase()
    {
    mManager = new QContactManager_Moc();
    TRAP_IGNORE( mMatcher = CPhCntMatcherImpl2::NewL(mManager) ); //the ownership is transferred
	
    }

void U_CPhCntMatcherImpl2::cleanupTestCase()
    {
	delete mMatcher;
    }

void U_CPhCntMatcherImpl2::test_MatchNumberFalse()
    {
    MPhCntMatch* match = NULL;
    TInt err = KErrNotSupported;
    mManager->setExpectedResult(0); //no match
    _LIT(KNumber, "123456");
    TRAP_IGNORE( err = mMatcher->MatchNumber(match, KNumber()) );
    
    Q_ASSERT(err == KErrNotFound);
    Q_ASSERT(!match);
    }

void U_CPhCntMatcherImpl2::test_MatchNumberExact()
    {
    MPhCntMatch* match = NULL;
    TInt err = KErrNotSupported;
    mManager->setExpectedResult(1); //1 exact match
    _LIT(KNumber, "123456");
    TRAP_IGNORE( err = mMatcher->MatchNumber(match, KNumber()));
     Q_ASSERT(err==KErrNone);
     Q_ASSERT(match);
    }
void U_CPhCntMatcherImpl2::test_MatchNumberSeveralReturned()
    {
    MPhCntMatch* match = NULL;
    TInt err = KErrNotSupported;
    mManager->setExpectedResult(2); //2 matches returned
    _LIT(KNumber, "123456");
    TRAP_IGNORE( err = mMatcher->MatchNumber(match, KNumber()) );
     Q_ASSERT(err==KErrNotFound);
     Q_ASSERT(!match);   
    }

void U_CPhCntMatcherImpl2::test_MatchNumber1()
    {
    
    MPhCntMatch* match = NULL;
    TInt err = KErrNotFound;
    _LIT(KNumber, "1234567890");
    CPhCntContactIdImpl2* cnt = NULL;
    QContact contact;
    TRAP_IGNORE( cnt = CPhCntContactIdImpl2::NewL(contact) );
    TRAP_IGNORE( err = mMatcher->MatchNumber(match, KNumber(), *cnt ) );
    Q_ASSERT(err == KErrNone);
    Q_ASSERT(!match);
    delete cnt;
    }

void U_CPhCntMatcherImpl2::test_MatchNumber2()
    {
    MPhCntMatch* match = NULL;
    TInt err = KErrNotFound;
    TFieldId field (989);
    _LIT(KNumber, "1234567890");
    CPhCntContactIdImpl2* cnt = NULL;
    QContact contact;
    TRAP_IGNORE( cnt = CPhCntContactIdImpl2::NewL(contact) );
    TRAP_IGNORE( err = mMatcher->MatchNumber(match, KNumber(), *cnt, field ) );
    Q_ASSERT(err == KErrNone);
    Q_ASSERT(!match);
    delete cnt;
    }


void U_CPhCntMatcherImpl2::test_MatchVoipNumber()
    {
    MPhCntMatch* match = NULL;
    TInt err = KErrNotSupported;
    _LIT(KNumber, "sip:user@domain.com");
    TRAP_IGNORE( err = mMatcher->MatchVoipNumber(match, KNumber(), ETrue) );
    Q_ASSERT(err == KErrNotFound);
    Q_ASSERT(!match);
    }

void U_CPhCntMatcherImpl2::test_MatchVoipNumber1()
    {
    MPhCntMatch* match = NULL;
    TInt err = KErrNotSupported;
    CPhCntContactIdImpl2* cnt = NULL;
    QContact contact;
    TRAP_IGNORE( cnt = CPhCntContactIdImpl2::NewL(contact) );  
    TRAP_IGNORE( err = mMatcher->MatchVoipNumber(match, *cnt) );
    Q_ASSERT(err == KErrNotFound);
    Q_ASSERT(!match);
    delete cnt;
    }

void U_CPhCntMatcherImpl2::test_HasCSNumbers()
    {
    CPhCntContactIdImpl2* cnt = NULL;
    QContact contact;
    TRAP_IGNORE(cnt = CPhCntContactIdImpl2::NewL(contact) );
    TBool has = ETrue;
    TRAP_IGNORE( has = mMatcher->HasCSNumbers(*cnt) );
    Q_ASSERT(has==EFalse);
    delete cnt;
    }
	
void U_CPhCntMatcherImpl2::test_MatchNumberWithContactId()
	{
	 MPhCntMatch* match = NULL;
    _LIT(KNumber, "123456");
	 TInt id = 765;
	 TInt err = KErrNotFound;
	 TRAP_IGNORE( err = mMatcher->MatchNumber(match, KNumber(), id) );
     Q_ASSERT(match);
	 Q_ASSERT(match->Number() == KNumber());
	 Q_ASSERT(err == KErrNone);

	}

QTEST_MAIN(U_CPhCntMatcherImpl2)
#include "u_cphcntmatcherimpl2.moc"
