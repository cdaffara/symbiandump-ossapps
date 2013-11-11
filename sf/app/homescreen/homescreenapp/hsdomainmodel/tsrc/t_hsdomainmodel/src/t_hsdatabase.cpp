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

#include <QGraphicsWidget>
#include <QSqlDatabase>

#include <HbFeedback>

#include "t_hsdomainmodel.h"
#include "hsdatabase.h"
#include "hsdomainmodeldatastructures.h"

void TestHsDomainModel::testHsDatabaseConstruction()
{
    HsDatabase *db = new HsDatabase;
    QVERIFY(!db->parent());

    delete db;

    QObject *parent = new QObject;
    db = new HsDatabase(parent);
    QVERIFY(db->parent() == parent);
    delete parent;
}

void TestHsDomainModel::testHsDatabaseProperties()
{
    HsDatabase *db = new HsDatabase;

    QVERIFY(db->property("connectionName").toString().isEmpty());
    QVERIFY(db->connectionName().isEmpty());

    QVERIFY(db->property("databaseName").toString().isEmpty());
    QVERIFY(db->databaseName().isEmpty());

    db->setProperty("connectionName", "a");
    QCOMPARE(db->connectionName(), QLatin1String("a"));
    db->setConnectionName("b");
    QCOMPARE(db->connectionName(), QLatin1String("b"));

    db->setProperty("databaseName", "c");
    QCOMPARE(db->databaseName(), QLatin1String("c"));
    db->setDatabaseName("d");
    QCOMPARE(db->databaseName(), QLatin1String("d"));

    delete db;
}

void TestHsDomainModel::testHsDatabaseStaticInterface()
{
    HsDatabase *db = HsDatabase::takeInstance();
    QVERIFY(!HsDatabase::instance());
    HsDatabase::setInstance(db);
    QVERIFY(HsDatabase::instance() == db);
}

void TestHsDomainModel::testHsDatabaseTransaction()
{
    HsDatabase *db = HsDatabase::instance();

    QList<HsPageData> pages;
    db->pages(pages);
    QCOMPARE(pages.count(), 0);

    HsPageData page;

    db->transaction();
    page.indexPosition = 0;
    db->insertPage(page);
    db->rollback();
    db->pages(pages);
    QCOMPARE(pages.count(), 0);

    db->transaction();
    page.indexPosition = 0;
    db->insertPage(page);
    db->commit();
    db->pages(pages);
    QCOMPARE(pages.count(), 1);
}

void TestHsDomainModel::testHsDatabaseSceneMethods()
{
    HsDatabase *db = HsDatabase::instance();

    HsSceneData scene;
    db->scene(scene);
    QCOMPARE(scene.id, 1);
}

void TestHsDomainModel::testHsDatabasePageMethods()
{
    HsDatabase *db = HsDatabase::instance();

    HsPageData page;
    page.indexPosition = 0;
    db->insertPage(page);

    HsPageData page2;
    page2.id = page.id;
    db->page(page2);

    QCOMPARE(page2.indexPosition, page.indexPosition);

    page.indexPosition = 1;
    db->updatePage(page);

    db->page(page2);

    QCOMPARE(page2.indexPosition, page.indexPosition);
}

void TestHsDomainModel::testHsDatabaseWidgetMethods()
{
    HsDatabase *db = HsDatabase::instance();

    HsWidgetData widget;
    widget.uri = "widgeturi";
    widget.pageId = 1;
    db->insertWidget(widget);

    QList<HsWidgetData> widgets;
    db->widgets(1, widgets);

    QCOMPARE(widgets.count(), 1);

    db->deleteWidgets("widgeturi");
    db->widgets(1, widgets);

    QCOMPARE(widgets.count(), 0);
}

void TestHsDomainModel::testHsDatabaseWidgetPreferenceMethods()
{
    HsDatabase *db = HsDatabase::instance();

    HsWidgetData widget;
    widget.uri = "widgeturi";
    widget.pageId = 1;
    db->insertWidget(widget);

    QVariantHash preferences;
    preferences.insert("a", 1);
    preferences.insert("b", 2);

    db->setWidgetPreferences(widget.id, preferences);

    QVariantHash preferences2;
    db->widgetPreferences(widget.id, preferences2);

    QCOMPARE(preferences2.count(), 2);
    QCOMPARE(preferences2.value("a"), QVariant(1));
    QCOMPARE(preferences2.value("b"), QVariant(2));

    QVariant value;
    db->widgetPreference(widget.id, "a", value);

    QCOMPARE(value, QVariant(1));
    preferences.insert("a", QVariant());
    preferences.insert("b", QVariant());

    db->setWidgetPreferences(widget.id, preferences);
    db->widgetPreferences(widget.id, preferences2);

    QCOMPARE(preferences2.count(), 0);

}

void TestHsDomainModel::testHsDatabaseBadConnection()
{
	HsDatabase db;

	QVariantHash vh;
	QVariant v;
	Qt::Orientation orientation(Qt::Vertical);
	HsWidgetPresentationData pd;
	QString uri("foouri");
	QString key("fookey");
	HsWidgetData wd;
	QList<HsWidgetData> wds;
	HsPageData pad;
	QList<HsPageData> pads;
	HsSceneData sd;

	db.setConnectionName(QString("fooconnection"));
	QVERIFY(!db.open());

	QVERIFY(!db.configuration(vh));

	QVERIFY(!db.setWidgetPreferences(33, vh));

	QVERIFY(!db.widgetPreference(33, key, v));

	QVERIFY(!db.widgetPreferences(33, vh));

	QVERIFY(!db.deleteWidgetPresentation(33, orientation));

	QVERIFY(!db.setWidgetPresentation(pd));

	QVERIFY(!db.widgetPresentation(pd));

	QVERIFY(!db.deleteWidgets(uri));

	QVERIFY(!db.deleteWidget(33));

	QVERIFY(!db.updateWidget(wd));

	QVERIFY(!db.insertWidget(wd));

	QVERIFY(!db.widget(wd));

	QVERIFY(!db.widgets(uri, wds));

	QVERIFY(!db.widgets(22, wds));
    QVariantHash variantHash;
    int count=0;	
	QVERIFY(!db.widgets("uri", variantHash, count));

	QVERIFY(!db.deletePage(22));

	QVERIFY(!db.updatePage(pad));

	QVERIFY(!db.insertPage(pad));

	QVERIFY(!db.page(pad));

	QVERIFY(!db.pages(pads));

	QVERIFY(!db.scene(sd));

    QVERIFY(!db.updateWidgetZValues(QHash<int, qreal>(), Qt::Vertical));
}

Q_DECLARE_METATYPE(QVariantHash)
void TestHsDomainModel::testHsDatabaseMatchWidgetsPreferences_data()
{
    QTest::addColumn<QVariantHash>("matchesPreferences");
    QTest::addColumn<QStringList>("widgetsPreferencesKeys");
    QTest::addColumn<QStringList>("widgetsPreferencesValues");
    QTest::addColumn<bool>("expectedResult");

    QVariantHash preferences;
    preferences.insert("a", "1");
    QTest::newRow("searchCriteria1")
        << preferences
        << (QStringList() << "a" << "a" << "a" << "b" << "c" << "d")
        << (QStringList() << "1" << "2" << "3" << "1" << "1" << "1")
        << true;
        
    preferences.insert("b", "1");
    QTest::newRow("searchCriteria2")
        << preferences
        << (QStringList() << "a" << "a" << "a" << "b" << "c" << "d")
        << (QStringList() << "1" << "2" << "3" << "1" << "1" << "1")
        << true;
    preferences.insert("e", "1");
    QTest::newRow("searchCriteria2")
        << preferences
        << (QStringList() << "a" << "a" << "a" << "b" << "c" << "d")
        << (QStringList() << "1" << "2" << "3" << "1" << "1" << "1")
        << false;
    QTest::newRow("searchCriteria3")
        << preferences
        << (QStringList())
        << (QStringList())
        << false;
    preferences.clear();
    QTest::newRow("searchCriteria4")
        << preferences
        << (QStringList() << "a" << "a" << "a" << "b" << "c" << "d")
        << (QStringList() << "1" << "2" << "3" << "1" << "1" << "1")
        << true;
}

void TestHsDomainModel::testHsDatabaseMatchWidgetsPreferences()
{
    QFETCH(QVariantHash, matchesPreferences);
    QFETCH(QStringList, widgetsPreferencesKeys);
    QFETCH(QStringList, widgetsPreferencesValues);
    QFETCH(bool, expectedResult);       
    
    QMultiMap<QString, QString> widgetPreferences;
    for (int i=0; i<widgetsPreferencesKeys.count(); ++i) {
        widgetPreferences.insert(widgetsPreferencesKeys[i], widgetsPreferencesValues[i]);
    }

    HsDatabase *db = HsDatabase::instance();
    QCOMPARE(expectedResult, db->matchWidgetPreferences(matchesPreferences, widgetPreferences));
}

void TestHsDomainModel::testHsDatabaseWidgetsCount()
{
    HsDatabase *db = HsDatabase::instance();

    HsWidgetData widget;
    widget.uri = "widgeturi";
    widget.pageId = 1;
    db->insertWidget(widget);
    QVariantHash preferences;
    preferences.insert("a", "1");
    preferences.insert("b", "2");
    db->setWidgetPreferences(widget.id, preferences);
    
    widget.uri = "widgeturi2";
    widget.pageId = 1;
    db->insertWidget(widget);
    
    widget.uri = "widgeturi";
    widget.pageId = 1;
    db->insertWidget(widget);
    preferences.insert("a", "3");
    db->setWidgetPreferences(widget.id, preferences);

    QVariantHash searchCriteria;
    int count=0;
    QVERIFY(db->widgets("widgeturi", searchCriteria, count));
    QCOMPARE(count, 2);
    
    searchCriteria.insert("a", "1");
    QVERIFY(db->widgets("widgeturi", searchCriteria, count));
    QCOMPARE(count, 1);
    searchCriteria.insert("a", "2");
    QVERIFY(db->widgets("widgeturi", searchCriteria, count));
    QCOMPARE(count, 0);
    searchCriteria.insert("a", "1");
    QVERIFY(db->widgets("widgeturi2", searchCriteria, count));
    QCOMPARE(count, 0);
    QVERIFY(db->widgets("", searchCriteria, count));
    QCOMPARE(count, 0);
    searchCriteria.clear();
    QVERIFY(db->widgets("widgeturi2", searchCriteria, count));
    QCOMPARE(count, 1);
    searchCriteria.clear();
    QVERIFY(db->widgets("", searchCriteria, count));
    QCOMPARE(count, 0);
}

void TestHsDomainModel::testHsDatabaseUpdateWidgetZValues()
{
    HsDatabase *db = HsDatabase::instance();
    HsPageData page;
    page.indexPosition = 0;
    QVERIFY(db->insertPage(page));
    
    HsWidgetData widget1;
    widget1.pageId = page.id;
    widget1.uri = "uri1";
    QVERIFY(db->insertWidget(widget1));

    HsWidgetData widget2;
    widget2.pageId = page.id;
    widget2.uri = "uri2";
    QVERIFY(db->insertWidget(widget2));

    HsWidgetPresentationData presentation1;
    presentation1.widgetId = widget1.id;
    presentation1.zValue = 0;
    presentation1.orientation = Qt::Vertical;
    db->setWidgetPresentation(presentation1);

    HsWidgetPresentationData presentation2;
    presentation2.widgetId = widget2.id;
    presentation2.zValue = 0;
    presentation2.orientation = Qt::Vertical;
    db->setWidgetPresentation(presentation2);

    QHash<int, qreal> data;
    data.insert(1, 2);
    data.insert(2, 3);
    QVERIFY(db->updateWidgetZValues(data, Qt::Vertical));
    
    QVERIFY(db->widgetPresentation(presentation1));
    QCOMPARE(presentation1.zValue, qreal(2));
    QVERIFY(db->widgetPresentation(presentation2));    
    QCOMPARE(presentation2.zValue, qreal(3));
}
