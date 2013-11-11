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

#include "testitemlist.h"

TestItemList::TestItemList()
    {
    }

TestItemList::~TestItemList()
    {
    removeAll();
    }

void TestItemList::removeAll()
    {
    for (QList<TestItem*>::iterator iter = listItems.begin(); iter
            < listItems.end(); ++iter)
        {
        delete (*iter);
        }
    listItems.clear();
    }

bool TestItemList::findItem(const QString& item, TestItem*& testItem) const
    {
    for (QList<TestItem*>::const_iterator iter = listItems.begin(); iter
            < listItems.end(); ++iter)
        {
        if ((*iter)->itemName.compare(item) == 0)
            {
            testItem = *iter;
            return true;
            }
        }
    return false;
    }

void TestItemList::removeItem(const QString& item)
    {
    TestItem* temp;
    if (findItem(item, temp))
        {
        listItems.removeOne(temp);
        delete temp;
        }
    }

void TestItemList::removeOneSubItem(const QString& item,
        const QString& subItem)
    {
    TestItem* temp;
    if (findItem(item, temp))
        {
        temp->subItemNames.removeOne(subItem);
        if (temp->subItemNames.count() == 0)
            {
            removeItem(item);
            }
        }
    }

void TestItemList::addItemExclusively(const QString& item,
        const QStringList& subItem)
    {
    TestItem* temp;
    if (!findItem(item, temp))
        {
        listItems.append(new TestItem(item, subItem));
        }
    }

void TestItemList::addItemExclusively(const QString& item)
    {
    TestItem* temp;
    if (!findItem(item, temp))
        {
        QStringList subItem;
        listItems.append(new TestItem(item, subItem));
        }
    }

void TestItemList::addOneSubItem(const QString& item, const QString& subItem)
    {
    TestItem* temp;
    if (!findItem(item, temp))
        {
        QStringList subItems;
        subItems << subItem;
        addItemExclusively(item, subItems);
        }
    else if (!temp->subItemNames.contains(subItem))
        {
        (temp->subItemNames) << subItem;
        }
    }

int TestItemList::count() const
    {
    return listItems.count();
    }

QStringList TestItemList::items() const
    {
    QStringList temp;
    for (QList<TestItem*>::const_iterator iter = listItems.begin(); iter
            < listItems.end(); ++iter)
        {
        temp << (*iter)->itemName;
        }
    return temp;
    }

QStringList TestItemList::subItems() const
    {
    QStringList temp;
    for (QList<TestItem*>::const_iterator iter = listItems.begin(); iter
            < listItems.end(); ++iter)
        {
        temp << (*iter)->subItemNames;
        }
    return temp;
    }

bool TestItemList::empty() const
    {
    return listItems.count() == 0;
    }

bool TestItemList::exists(const QString& itemName) const
    {
    TestItem* temp;
    return findItem(itemName, temp);
    }

bool TestItemList::isEqual(const TestItemList* itemList)
    {
    bool ret = this->count() == itemList->count();
    if (ret)
        {
        QStringList subItems = this->subItems();
        ret = subItems.count() == itemList->subItems().count();
        if (ret)
            {
            QStringList items = this->items();
            foreach(QString tempItem, itemList->items())
                    {
                    if (!items.contains(tempItem, Qt::CaseInsensitive))
                        {
                        return false;
                        }
                    }
            foreach(QString tempSubItem, itemList->subItems())
                    {
                    if (!subItems.contains(tempSubItem, Qt::CaseInsensitive))
                        {
                        return false;
                        }
                    }
            }
        }
    return ret;
    }

TestItemList::TestItem::TestItem()
    {
    }

TestItemList::TestItem::TestItem(QString name, QStringList cases) :
    itemName(name), subItemNames(cases)
    {
    }

TestItemList::TestItem::~TestItem()
    {
    }

QString TestItemList::TestItem::getFullQulifiedCaseName(const QString& str)
    {
    // TODO
    return QString(str);
    }

bool TestItemList::TestItem::isEqual(const TestItem& item)
    {
    // TODO: to be extended.
    return itemName.compare(item.itemName) == 0;
    }
