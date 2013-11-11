/*
 * unittest_
 *
 */

#ifndef UNITTEST_NMCONTACTHISTORYMODEL_H_
#define UNITTEST_NMCONTACTHISTORYMODEL_H_

#include <QtTest/QtTest>

class NmContactHistoryModel;

class TestNmContactHistoryModel : public QObject
{
Q_OBJECT

public:

    TestNmContactHistoryModel();
    ~TestNmContactHistoryModel();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test slot for testing signal.
    void test_modelCompleted(int err);

    //tests
    void testConstructor();
    void testDestructor();
    void testRowCount();
    void testData();
    void testQuery();
    void testHandleQueryCompleted();

private:

    NmContactHistoryModel *mModel;
    bool mSlotCalled;
};

#endif /* UNITTEST_NMCONTACTHISTORYMODEL_H_ */
