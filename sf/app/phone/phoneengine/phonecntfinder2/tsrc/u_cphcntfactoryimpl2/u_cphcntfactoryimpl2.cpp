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
#include <cprofilechangenotifyhandler.h>
#include <mprofilechangeobserver.h>
#include "cphcntfactoryimpl2.h"
#include "cphcntmatcherimpl2.h"

//---- test stub
class CProfileChangeObserverImp: public CBase, public MProfileChangeObserver
    {
public:
    static CProfileChangeObserverImp* NewL();
    ~CProfileChangeObserverImp();
    void HandleActiveProfileEventL(
                TProfileEvent aProfileEvent, TInt aProfileId );
private:
    CProfileChangeObserverImp();
        
    };

CProfileChangeObserverImp::CProfileChangeObserverImp()
    {
    }
CProfileChangeObserverImp* CProfileChangeObserverImp::NewL()
    {
    CProfileChangeObserverImp* self = NULL;
    self = new (ELeave) CProfileChangeObserverImp();
    return self;
    }

CProfileChangeObserverImp::~CProfileChangeObserverImp()
    {
    }

void CProfileChangeObserverImp::HandleActiveProfileEventL(
        TProfileEvent /*aProfileEvent*/, TInt /*aProfileId*/)
    {
    
    }

//---- test class

class U_CPhCntFactoryImpl2: public QObject
{
Q_OBJECT

public:
    U_CPhCntFactoryImpl2();
	~U_CPhCntFactoryImpl2();

private slots:
	void initTestCase();
    void cleanupTestCase();
	void test_CreateContactMatcherL();
	void test_CreateProfileEngineNotifyHandlerL();
    void test_CreateContactId1L();
    void test_CreateContactId2L();

	
private:
	
    CPhCntFactoryImpl2* mFactory; // implementation under test

};

U_CPhCntFactoryImpl2::U_CPhCntFactoryImpl2()
{
}

U_CPhCntFactoryImpl2::~U_CPhCntFactoryImpl2()
{
}

void U_CPhCntFactoryImpl2::initTestCase()
{

    TRAP_IGNORE( mFactory = CPhCntFactoryImpl2::NewL() );
	
}

void U_CPhCntFactoryImpl2::cleanupTestCase()
{
	delete mFactory;
}


void U_CPhCntFactoryImpl2::test_CreateContactMatcherL()
    {
    CPhCntMatcher* matcher = NULL;
    TRAP_IGNORE( matcher = mFactory->CreateContactMatcherL() );
    Q_ASSERT(matcher);
    delete matcher;   
    }


void U_CPhCntFactoryImpl2::test_CreateProfileEngineNotifyHandlerL()
    {
    CProfileChangeObserverImp* obs = NULL;
    obs = CProfileChangeObserverImp::NewL();
    CBase* engine = NULL;
    TRAP_IGNORE( engine = mFactory->CreateProfileEngineNotifyHandlerL(obs) );
    Q_ASSERT(engine);
    delete engine;
    delete obs;
    }

    
void U_CPhCntFactoryImpl2::test_CreateContactId1L()
    {
    Q_ASSERT(!mFactory->CreateContactIdL(999));
    }
void U_CPhCntFactoryImpl2::test_CreateContactId2L()
    {
    Q_ASSERT(!mFactory->CreateContactIdL(_L8("abcd")));
    }



QTEST_MAIN(U_CPhCntFactoryImpl2)
#include "u_cphcntfactoryimpl2.moc"
