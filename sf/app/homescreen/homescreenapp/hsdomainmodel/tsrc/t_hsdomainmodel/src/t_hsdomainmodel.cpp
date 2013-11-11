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
* Description:  Main test class for domain model.
*
*/

#include <qservicemanager.h>
#include <hbinstance.h>
#include "t_hsdomainmodel.h"
#include "hsdatabase.h"
#include "hspage.h"
#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetcomponentregistry.h"
#include "hsbackuprestoreobserver.h"
#include "hsshortcutservice.h"
#include "caservice.h"
#include "hsconfiguration.h"
#include "hswallpaper.h"

QTM_USE_NAMESPACE

#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#endif

const char WIDGET_PLUGIN_PATH[] = "./hsresources/plugins/widgetplugins/";
const char WIDGET_PLUGIN_XML[] = "mockwidgetplugin.xml";
const char WIDGET_PLUGIN_TOOSMALL_XML[] = "mockwidgetplugintoosmall.xml";
const char WIDGET_PLUGIN_TOOBIG_XML[] = "mockwidgetplugintoobig.xml";
const char WIDGET_PLUGIN_CHEATER_XML[] = "mockwidgetplugincheater.xml";
const char DBPATH[]= "./";
const char DBNAME[] = "hsdomainmodeltest2.db";
const char DBNAMECLEAN[] = "hsdomainmodeltest.db";

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestHsDomainModel::initTestCase()
{
    mCaService = CaService::instance();
    QString path = QDir(WIDGET_PLUGIN_PATH).absolutePath();
    QApplication::addLibraryPath(path);
    QServiceManager manager;
    if (!manager.addService(QDir(WIDGET_PLUGIN_PATH).absolutePath().append("/").append(WIDGET_PLUGIN_XML))) {
        QServiceManager::Error err = manager.error();
        qDebug() << "TestHsDomainModel::initTestCase register mockwidgetplugin failed" << err;
    }
    if (!manager.addService(QDir(WIDGET_PLUGIN_PATH).absolutePath().append("/").append(WIDGET_PLUGIN_TOOSMALL_XML))) {
        QServiceManager::Error err = manager.error();
        qDebug() << "TestHsDomainModel::initTestCase register mockwidgetplugintoosmall failed" << err;
    }

    if (!manager.addService(QDir(WIDGET_PLUGIN_PATH).absolutePath().append("/").append(WIDGET_PLUGIN_TOOBIG_XML))) {
        QServiceManager::Error err = manager.error();
        qDebug() << "TestHsDomainModel::initTestCase register mockwidgetplugintoobig failed" << err;
    }

    if (!manager.addService(QDir(WIDGET_PLUGIN_PATH).absolutePath().append("/").append(WIDGET_PLUGIN_CHEATER_XML))) {
        QServiceManager::Error err = manager.error();
        qDebug() << "TestHsDomainModel::initTestCase register mockwidgetplugincheater failed" << err;
    }

    HsConfiguration::setInstance(new HsConfiguration);

    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestHsDomainModel::cleanupTestCase()
{
    QServiceManager manager;
    manager.removeService("mockwidgetplugin");
    manager.removeService("mockwidgetplugintoosmall");
    manager.removeService("mockwidgetplugintoobig");
    manager.removeService("mockwidgetplugincheater");
    HsDatabase::setInstance(0);

    delete HsWidgetComponentRegistry::mInstance;
    HsWidgetComponentRegistry::mInstance = 0;

    delete HsShortcutService::mInstance;
    HsShortcutService::mInstance = 0;
}

void TestHsDomainModel::init()
{
    QString dbFile = QDir(DBPATH).absolutePath() + "/" + QString(DBNAME);
    QString testDbFile = QDir(DBPATH).absolutePath() + "/" + QString(DBNAMECLEAN);
    QFile::remove(dbFile);
    QFile::copy(testDbFile, dbFile);

    HsScene::setInstance(new HsScene);

    HsDatabase *db = new HsDatabase;
    HsDatabase::setInstance(db);
    db->setConnectionName("homescreen.dbc");
    db->setDatabaseName(dbFile);
    db->open();

    mWallpaperTypeScene = true;
}

void TestHsDomainModel::cleanup()
{
    // cleanup all the pages and widgets
    HsDatabase::setInstance(0);
    QString path = QDir(WIDGET_PLUGIN_PATH).absolutePath();
    QApplication::addLibraryPath( path );
    QString dbFile = QDir(DBPATH).absolutePath() + "/" + QString(DBNAME);
    QFile::remove(dbFile);

    if (HsBackupRestoreObserver::mInstance) {
        delete HsBackupRestoreObserver::mInstance;
        HsBackupRestoreObserver::mInstance = 0;
    }

    delete HsScene::takeInstance();
}

void TestHsDomainModel::createPagesToDatabase(int pageCount)
{
    HsDatabase *db = HsDatabase::instance();
    for (int i = 0; i < pageCount; ++i) {
        HsPageData data;
        data.indexPosition = i;
        db->insertPage(data);
    }
}

void TestHsDomainModel::createSceneWithPages(int pageCount,int activePageIndex)
{
    for (int i = 0; i < pageCount; ++i) {
        HsPageData pageData;
        pageData.indexPosition = i;
        HsPage *p = HsPage::createInstance(pageData);
        HsScene::instance()->addPage(p);
    }
    if (activePageIndex < pageCount) {
        QList<HsPage*> pages = HsScene::instance()->pages();
        if(activePageIndex < pages.count())
            HsScene::instance()->setActivePage(pages.at(activePageIndex));
    }

}

void TestHsDomainModel::destroySceneAndPages()
{
     delete HsScene::takeInstance();
}

QString TestHsDomainModel::sourceTestWallpaper()
{
    QString testImage("default_portrait.png");
    QString fullSource(wallpaperDirectory() + testImage);
    return QDir::toNativeSeparators(fullSource);
}

QString TestHsDomainModel::targetPortraitTestWallpaper()
{
    QString path = wallpaperDirectory() +
                   "1_portrait." +
                   QFileInfo(sourceTestWallpaper()).suffix();
    return QDir::toNativeSeparators(path);
}

QString TestHsDomainModel::targetLandscapeTestWallpaper()
{
    QString path = wallpaperDirectory() +
                   "1_landscape." +
                   QFileInfo(sourceTestWallpaper()).suffix();
    return QDir::toNativeSeparators(path);
}

QString TestHsDomainModel::tempTestWallpaper()
{
    QString tempTestImage("tempdefault_portrait.png");
    QString fullTarget(wallpaperDirectory() +  tempTestImage);
    return QDir::toNativeSeparators(fullTarget);
}

QString TestHsDomainModel::notExistingTestWallpaper()
{
    QString tempTestImage("notfound.png");
    QString fullTarget(wallpaperDirectory() +  tempTestImage);
    return QDir::toNativeSeparators(fullTarget);
}

QString TestHsDomainModel::wallpaperDirectory()
{
    QString directory;
    if (mWallpaperTypeScene) {
        HsSceneWallpaper wallpaper(HsScene::instance());
        directory = wallpaper.wallpaperDirectory();
    } else {
        HsPage *page = new HsPage();
        HsPageWallpaper wallpaper(page);
        directory = wallpaper.wallpaperDirectory();
        delete page;
    }

    return directory;
}

void TestHsDomainModel::cleanupTargetWallpaperFiles()
{
    if (QFile::exists(targetPortraitTestWallpaper())) {
        QFile::remove(targetPortraitTestWallpaper());
    }

    if (QFile::exists(targetLandscapeTestWallpaper())) {
        QFile::remove(targetLandscapeTestWallpaper());
    }

    if (QFile::exists(tempTestWallpaper())) {
        QFile::remove(tempTestWallpaper());
    }
}

#ifdef Q_OS_SYMBIAN
//QTEST_MAIN corrected since crashes if TRAP not in correct place.
//Will be corrected in later (estimate 4.6.0) Qt release for Symbian.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int error(0);
    TRAP_IGNORE( QTEST_DISABLE_KEYPAD_NAVIGATION \
	 HbInstance::instance(); \
    TestHsDomainModel tc; \
    error = QTest::qExec(&tc, argc, argv););
    return error;
}
#else //Q_OS_SYMBIAN
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION
    HbInstance::instance();
    TestHsDomainModel tc;
    return QTest::qExec(&tc, argc, argv);
}
//QTEST_MAIN(TestHsDomainModel)
#endif //Q_OS_SYMBIAN
