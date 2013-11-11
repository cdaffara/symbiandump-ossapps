/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __UNITTEST_NMCONTACTHISTORYMODELPRIVATE_H__
#define __UNITTEST_NMCONTACTHISTORYMODELPRIVATE_H__

#include <QtTest/QtTest>

class NmContactHistoryModelPrivate;

class TestNmContactHistoryModelPrivate : public QObject
{
    Q_OBJECT

signals:

private slots:
    /**
     * called before anything else when test starts
     */
    void initTestCase();

  /**
     * will be called before each testfunction is executed.
     *
     */
    void init();

    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();

    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();

    // Signal

    void test_queryContactDatabase();
    void test_queryMruDatabase();

    void test_populateListWithContactItems();
    void test_populateListWithMruItems();

    void test_rowCount();
    void test_privateDataCount();
    void test_dataFunc();

    void test_refreshDataModel();
    void test_fillMruMatchList();
    void test_obeyContactOrder();

    void test_queryDatabases();

    void test_queryCompleted();
    void test_handleQueryCompleted(int err);

private:

    /**
     * object under test
     */
    NmContactHistoryModelPrivate *mTestObject;

    QStringList mTestNames;
    bool        mSignalEmitted;
};

#endif //__UNITTEST_NMCONTACTHISTORYMODELPRIVATE_H__

