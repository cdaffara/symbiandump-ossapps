/*
 * unittest_emailmru.h
 *
 */

#ifndef UNITTEST_EMAILMRU_H_
#define UNITTEST_EMAILMRU_H_

#include <QtTest/QtTest>

class EmailMRU;

class TestEmailMru : public QObject
{
Q_OBJECT

public:

    TestEmailMru();
    ~TestEmailMru();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    //tests
    void testMruStrings();
    void testMruStrings_data();
    void testUpperLimit();
    void testUpdateEntry();
    void testReset();
    void testNameOrder();

private:
    EmailMRU *iMru;

};

#endif /* UNITTEST_EMAILMRU_H_ */
