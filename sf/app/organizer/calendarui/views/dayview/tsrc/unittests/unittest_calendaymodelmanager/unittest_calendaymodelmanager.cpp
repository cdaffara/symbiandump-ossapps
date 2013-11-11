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
 * Description: Test class for CalenDayContainerWidget
 *
 */
#include <QtTest/QtTest>

#include "calenservices.h"
#include "calendaymodel.h"
#include "calendaymodelmanager.h"

QDateTime CURRENT_DATE;

class TestCalenDayModelManager : public QObject
{
Q_OBJECT

public:
    TestCalenDayModelManager();
    virtual ~TestCalenDayModelManager();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testGetModel();
    void testRefreshAllModels();
    void testRefreshSingleModel();
    void testMoveBackward();
    void testMoveForward();
    //refreshAllModels
    
    void testConstructors();

private:
	MCalenServices services;
    CalenDayModelManager *mManager;
};

/*!
 Constructor
 */
TestCalenDayModelManager::TestCalenDayModelManager() :
mManager(NULL)
{
	
}

/*!
 Destructor
 */
TestCalenDayModelManager::~TestCalenDayModelManager()
{

}

/*!
 Called before testcase
 */
void TestCalenDayModelManager::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayModelManager::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayModelManager::init()
{
    CURRENT_DATE = QDateTime(QDate(2000, 10, 10));
	mManager = new CalenDayModelManager(services, false);
}

/*!
 Called after everyfunction
 */
void TestCalenDayModelManager::cleanup()
{
    if (mManager) {
        delete mManager;
        mManager = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if container is not initialized
 2. Test if container is correcty created
 */
void TestCalenDayModelManager::testConstructors()
{
    //1)
	MCalenServices services;

    CalenDayModelManager *testManager = 0;
    CalenDayModelManager *testManager2 = 0;
    QVERIFY(!testManager);
    QVERIFY(!testManager2);
    
    //2)
    testManager = new CalenDayModelManager(services, false, 0);
    testManager2 = new CalenDayModelManager(services, true, 0);
    QVERIFY(testManager);
    QVERIFY(testManager2);
    
    delete testManager;
    delete testManager2;
}

/*!
   Test function for getModel.
   
   It check is model has date setted if yes, then it is created.
 */
void TestCalenDayModelManager::testGetModel()
{
    CalenDayModel* model = static_cast<CalenDayModel*> (&(mManager->getModel(
        CalenDayModelManager::CurrentDay)));
    QCOMPARE(model->modelDate(), CURRENT_DATE);
}

/*!
   Test function for RefreshAllModels
   1)Check that, all models are not refreshed
   2)Check that all folders are refreshed
 */
void TestCalenDayModelManager::testRefreshAllModels()
{
    //1)
    CalenDayModel* model1;
    CalenDayModel* model2;
    CalenDayModel* model3;
    
    model1 = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::PreviousDay)));
    model2 = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::CurrentDay)));
    model3 = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::NextDay)));
    QVERIFY(model1->modelRefreshed() == 0);
    QVERIFY(model2->modelRefreshed() == 0);
    QVERIFY(model3->modelRefreshed() == 0);
    
    //2)
    mManager->refreshAllModels();
    model1 = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::PreviousDay)));
    model2 = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::CurrentDay)));
    model3 = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::NextDay)));
    QVERIFY(model1->modelRefreshed() == 1);
    QVERIFY(model2->modelRefreshed() == 1);
    QVERIFY(model3->modelRefreshed() == 1);
}

/*!
  Test functuion for  RefreshSingleModel
  1) test refresh model for current day
  2) test refresh model for previous day
  3) test refresh model for next day
  4) test refresh model for no model selected
 */
void TestCalenDayModelManager::testRefreshSingleModel()
{
    CalenDayModel* model;

    //1)
    mManager->refreshSingleModel(CalenDayModelManager::CurrentDay);
    model = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::CurrentDay)));
    QVERIFY(model->modelRefreshed() > 0);

    //2)
    mManager->refreshSingleModel(CalenDayModelManager::PreviousDay);
    model = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::PreviousDay)));
    QVERIFY(model->modelRefreshed() > 0);

    //3)
    mManager->refreshSingleModel(CalenDayModelManager::NextDay);
    model = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::NextDay)));
    QVERIFY(model->modelRefreshed() > 0);

    //4) it is equal 1 because each model was refreshed
    mManager->refreshSingleModel(CalenDayModelManager::NumberOfDays);
    model = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::CurrentDay)));
    QVERIFY(model->modelRefreshed() == 1);
    model = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::PreviousDay)));
    QVERIFY(model->modelRefreshed() == 1);
    model = static_cast<CalenDayModel*> (&(mManager->getModel(CalenDayModelManager::NextDay)));
    QVERIFY(model->modelRefreshed() == 1);
}

/*!
  test function for moveBackward()
  1)test if model for current date is good
  2)test if model change after bacward
  
  It test also CalenDayModelManager::viewsScrollingFinished
 */
void TestCalenDayModelManager::testMoveBackward()
{
    //1)
    CalenDayModel* model = static_cast<CalenDayModel*>(&(mManager->getModel(CalenDayModelManager::CurrentDay)));
    QCOMPARE(model->modelDate(), CURRENT_DATE);
    
    //2)
    mManager->viewsScrollingFinished(ECalenScrollToPrev);
    
    model = static_cast<CalenDayModel*>(&(mManager->getModel(CalenDayModelManager::CurrentDay)));
    QCOMPARE(model->modelDate(), CURRENT_DATE.addDays(-1));    
}

/*!
  test function for moveBackward()
  1)test if model for current date is good
  2)test if model change after forward
  
  It test also CalenDayModelManager::viewsScrollingFinished
 */
void TestCalenDayModelManager::testMoveForward()
{
    //1)
    CalenDayModel* model = static_cast<CalenDayModel*> (&(mManager->getModel(
        CalenDayModelManager::CurrentDay)));
    QCOMPARE(model->modelDate(), CURRENT_DATE);

    //2)
    mManager->viewsScrollingFinished(ECalenScrollToNext);

    model = static_cast<CalenDayModel*> (&(mManager->getModel(
        CalenDayModelManager::CurrentDay)));
    QCOMPARE(model->modelDate(), CURRENT_DATE.addDays(1));     
}

QTEST_MAIN(TestCalenDayModelManager);
#include "unittest_calendaymodelmanager.moc"
