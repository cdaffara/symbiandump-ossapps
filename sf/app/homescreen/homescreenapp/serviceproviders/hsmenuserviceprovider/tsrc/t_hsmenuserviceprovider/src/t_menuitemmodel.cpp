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

#include <qcoreapplication.h>
#include <qstandarditemmodel.h>
#include "caquery.h"
#include <hstest_global.h>
#include "hsmenuservice.h"
#include "hsmenucollectionsitemmodel.h"
#include "hsmenuitemmodel.h"
#include "hsmenuserviceutils.h"
#include "t_hsmenuserviceprovider.h"
#include "hsiconsidleloader.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuItemModel_construction()
{
    QScopedPointer<QObject> parent(new QObject);
    CaQuery query;
    query.mQueryId = "__HsMenuItemModel_construction_CaQuery";
    HsMenuItemModel model(query, parent.data());
    QCOMPARE(model.mCaItemModelQuery.mQueryId, query.mQueryId);
    QCOMPARE(model.QObject::parent(), parent.data());
    QVERIFY(model.getIconSize().isValid());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuItemModel_setSort()
{
    HsMenuItemModel model(CaQuery(), this);
    
    {
        const Hs::HsSortAttribute sortAttribute = Hs::NoHsSortAttribute;
    
        model.setSort(sortAttribute);
        QCOMPARE(model.mSetSortAttribute, HsMenuServiceUtils::sortBy(sortAttribute));
        QCOMPARE(model.mSetSortOrder, HsMenuServiceUtils::sortOrder(sortAttribute));
    }
    {
        const Hs::HsSortAttribute sortAttribute = Hs::DescendingNameHsSortAttribute;
    
        model.setSort(sortAttribute);
        QCOMPARE(model.mSetSortAttribute, HsMenuServiceUtils::sortBy(sortAttribute));
        QCOMPARE(model.mSetSortOrder, HsMenuServiceUtils::sortOrder(sortAttribute));        
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuItemModel_getData()
{
    HsMenuItemModel model(CaQuery(), this);
    
    Qt::ItemDataRole role = Qt::DecorationRole;
    const QModelIndex index = model.index(0, 0);
    const QVariant expectedDecoration = QVariant(HbIcon(Hs::newIconId));
    
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->mEntryTypeNameResult = Hs::collectionDownloadedTypeName;
    model.setSecondLineVisibility(true);
    model.mDataResult = expectedDecoration;

    QList<QVariant> actualResult = model.data(index, role).toList();

    QCOMPARE(model.mDataIndex,index);
    QCOMPARE(model.mDataRole,(int)role);
    QCOMPARE(actualResult.empty(),false);
    QCOMPARE(actualResult.at(0).value<HbIcon>().iconName(), 
        QString(Hs::newIconId));

    QCOMPARE(model.data(index), model.mDataResult);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuItemModel_newIconNeeded()
{
    HsMenuItemModel model(CaQuery(), this);
    
    const QModelIndex index = model.index(0, 0);
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->mEntryTypeNameResult = Hs::collectionDownloadedTypeName;
    model.setSecondLineVisibility(true);
    QVERIFY(model.newIconNeeded(index));
    
    model.mRootResult = QModelIndex(); // invalid
    QVERIFY(!model.root().isValid() && 
        !model.newIconNeeded(index));
    
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = UsedEntryFlag;
    QVERIFY((model.entry(index)->flags() & UsedEntryFlag) && 
        !model.newIconNeeded(index));
    
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->mEntryTypeNameResult = "";
    QVERIFY((model.entry(index)->entryTypeName() != Hs::collectionDownloadedTypeName ) && 
        !model.newIconNeeded(index));
    
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuItemModel_uninstallChange()
{
    HsMenuItemModel model(CaQuery(), this);
    const QModelIndex index = model.index(0, 0);
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->setAttribute(QString("component_id"), 
            QString().setNum(1));
    
    model.uninstallChange(1, 5);
    QCOMPARE(model.mComponentId, 1);
    QCOMPARE(model.mIds.count(), 2);
    QCOMPARE(model.mEntryResult->attribute(
            QString("uninstall_progress")), QString("5"));
    
    model.uninstallChange(1, 20);
    QCOMPARE(model.mEntryResult->attribute(
                QString("uninstall_progress")), QString("20"));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuCollectionsItemModel_construction()
{
    QScopedPointer<QObject> parent(new QObject);
    CaQuery query;
    query.mQueryId = "__HsMenuCollectionItemModel_construction_CaQuery";
    HsMenuCollectionsItemModel model(query, parent.data());
    QCOMPARE(model.mCaItemModelQuery.mQueryId, query.mQueryId);
    QCOMPARE(model.QObject::parent(), parent.data());
    QVERIFY(model.getIconSize().isValid());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuCollectionsItemModel_getData()
{
    HsMenuCollectionsItemModel model(CaQuery(), this);
    
    const QModelIndex index = model.index(0, 0);
    const QString expectedText("entryText");
    const QString expectedSecondLine(hbTrId("txt_applib_dblist_downloaded_val_empty"));
    
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->mTextResult = expectedText;
    model.setSecondLineVisibility(true);

    QList<QVariant> actualResult = model.data(index).toList();
    
    QCOMPARE(actualResult.at(0).toString(), 
        expectedText);
    QCOMPARE(actualResult.at(1).toString(), 
        expectedSecondLine);

    
    Qt::ItemDataRole role = Qt::DecorationRole;
    const QVariant expectedData;

    model.mDataResult = expectedData;
    QCOMPARE(model.data(index, role), expectedData);

    model.setSecondLineVisibility(false);
    QCOMPARE(model.data(index), expectedData);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuCollectionsItemModel_getSecondLine()
{    
    HsMenuCollectionsItemModel model(CaQuery(), this);
    Qt::ItemDataRole role = Qt::DisplayRole;
    model.setSecondLineVisibility(true);
    const QModelIndex index = model.index(0, 0);
    
    // set up mock data    
   
    // make up 2 items to be stored in a collection
    QSharedPointer<CaEntry> collectionItemA(new CaEntry);
    QString collectionItemAName("A");
    collectionItemA->mTextResult = collectionItemAName;
    QSharedPointer<CaEntry> collectionItemB(new CaEntry);
    QString collectionItemBName("B");
    collectionItemB->mTextResult = collectionItemBName;
    
    // make sure CaService will return the 2 entries on getEntries call
    CaService::instance()->mGetEntriesResult.append(collectionItemA);
    CaService::instance()->mGetEntriesResult.append(collectionItemB);
    
    QString expectedSecondLine(hbTrId("txt_applib_dblist_val_ln_new_applications",
        CaService::instance()->mGetEntriesResult.count()));
    
    // make data() call getSecondLine for 'downloaded' collection
    model.mEntryResult->setEntryTypeName(Hs::collectionDownloadedTypeName);
    model.mEntryResult->mIdResult = 5; // just to have any id
    
    QString actualSecondLine = model.data(index, role).toList().at(1).toString();
    
    QCOMPARE(actualSecondLine, expectedSecondLine);    

    // check what CaQuery was prepared by getSecondLine()
    CaQuery resultQuery = CaService::instance()->mGetEntriesQuery;
    QCOMPARE(resultQuery.mSetParentId, model.mEntryResult->mIdResult);
    QCOMPARE(resultQuery.mSetEntryRoles, ItemEntryRole);
    const EntryFlags expectedCollectionDownloadedFlags = 
        RemovableEntryFlag | VisibleEntryFlag;
    QCOMPARE(resultQuery.mSetFlagsOn, expectedCollectionDownloadedFlags);
   
    
    // case for static collection
    const EntryFlags expectedCollectionFlag = VisibleEntryFlag;
    expectedSecondLine = collectionItemAName + "," + collectionItemBName;
    
    model.mEntryResult->setEntryTypeName(Hs::collectionTypeName);
    
    actualSecondLine = model.data(index, role).toList().at(1).toString();
    
    QCOMPARE(actualSecondLine, expectedSecondLine);

    resultQuery = CaService::instance()->mGetEntriesQuery;
    QCOMPARE(resultQuery.mSetFlagsOn, expectedCollectionFlag);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuItemModel_preloadIcons()
{
    HsMenuItemModel model(CaQuery(), this);
    
    // set up mock data
    Qt::ItemDataRole role = Qt::DecorationRole;
    const QModelIndex index = model.index(0, 0);
    const QVariant expectedDecoration = QVariant(HbIcon(Hs::newIconId));
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->mEntryTypeNameResult = Hs::collectionDownloadedTypeName;
    model.setSecondLineVisibility(true);
    model.mDataResult = QVariant(expectedDecoration);
    model.mRowCountResult = 1; // i.e. how many rows the mock model pretends to have
    QList<QVariant> actualResult = model.data(index, role).toList();
    model.mEntryResult->mMakeIconResult = HbIcon(Hs::newIconId);
    
    // verify mock setup
    QCOMPARE(model.mDataIndex,index);
    QCOMPARE(model.mDataRole,(int)role);
    QCOMPARE(actualResult.empty(),false);
    QCOMPARE(actualResult.at(0).value<HbIcon>().iconName(), 
        QString(Hs::newIconId));
    QCOMPARE(model.data(index), model.mDataResult);
    
    // invoke icon preloader construction
    model.preloadIcons();
    QVERIFY(model.mIconsIdleLoader);
    
    // invoke icon preloading

    model.mIconsIdleLoader->processWhenIdle();
    
    // compare what has been logged in mock object with referece data
    QCOMPARE(model.mEntryResult->mMakeIconSize, model.getIconSize());
    QCOMPARE(model.mEntryResult->mMakeIconResult.iconName(),
        QString(Hs::newIconId));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsIconsIdleLoader_construction()
{
    // precondtitons - prepare model
    HsMenuItemModel model(CaQuery(), this);
    
    // set up mock data
    Qt::ItemDataRole role = Qt::DecorationRole;
    const QModelIndex index = model.index(0, 0);
    const QVariant expectedDecoration = QVariant(HbIcon(Hs::newIconId));
    model.mRootResult = index;
    model.mEntryResult->mFlagsResult = 0;
    model.mEntryResult->mEntryTypeNameResult = Hs::collectionDownloadedTypeName;
    model.setSecondLineVisibility(true);
    model.mDataResult = QVariant(expectedDecoration);
    model.mRowCountResult = 1; // i.e. how many rows the mock model pretends to have
    QList<QVariant> actualResult = model.data(index, role).toList();
    model.mEntryResult->mMakeIconResult = HbIcon(Hs::newIconId);

    QCOMPARE(model.mDataIndex,index);
    QCOMPARE(model.mDataRole,(int)role);
    QCOMPARE(actualResult.empty(),false);
    QCOMPARE(actualResult.at(0).value<HbIcon>().iconName(), 
        QString(Hs::newIconId));
    
    QCOMPARE(model.data(index), model.mDataResult);
    
    // test
    HsIconsIdleLoader *iconsIdleLoader = new HsIconsIdleLoader(&model);
    
    QVERIFY(iconsIdleLoader->mModel);
    QVERIFY(iconsIdleLoader->mTimer);
    QVERIFY(iconsIdleLoader->mTimer->isActive());
    QTest::qWait(10000);
    QVERIFY(!iconsIdleLoader->mTimer->isActive());
    
    delete iconsIdleLoader;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
int MenuServiceTest::getId(HsMenuItemModel *model , int idx)
{
    return model->data(model->index(idx), CaItemModel::IdRole).toInt();
}

