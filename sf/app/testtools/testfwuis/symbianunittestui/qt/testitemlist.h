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
 * Description: class to setup mapping of individual test cases and dll files.
 *
 */

#ifndef TESTITEMLIST_H_
#define TESTITEMLIST_H_
#include <QString>
#include <QStringList>
#include <QList>

class TestItemList
    {    
public:
    TestItemList();
    ~TestItemList();

public:
    void removeItem(const QString&);
    void removeAll();
    void removeOneSubItem(const QString&, const QString&);
    void addItemExclusively(const QString&, const QStringList&);
    void addItemExclusively(const QString&);
    void addOneSubItem(const QString&, const QString&);
    int count() const;
    bool isEqual(const TestItemList*);
    bool empty() const;
    bool exists(const QString&) const;
    QStringList items() const;
    QStringList subItems() const;
    
private:
    class TestItem
        {
    public:
        TestItem();
        TestItem(QString, QStringList);
        ~TestItem();

    public:
        QString getFullQulifiedCaseName(const QString&);
        bool isEqual(const TestItem&);

    public:
        QString itemName;
        QStringList subItemNames;
        };
    
private:
    bool findItem(const QString&, TestItem*&) const;
    
private:
    QList<TestItem*> listItems;
    
    };

#endif /* TESTITEMLIST_H_ */
