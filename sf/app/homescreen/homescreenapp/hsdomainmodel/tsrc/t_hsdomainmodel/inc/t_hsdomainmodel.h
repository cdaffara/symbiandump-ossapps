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


#include <QtTest/QtTest>
#include <QObject>

#include <HbMainWindow.h>

class CaService;

class TestHsDomainModel : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase(); 
 
    //t_hswidgethostvisual
    void testHsWidgetHostVisualConstruction();
    void testHsWidgetHostVisualShape();
    void testHsWidgetHostVisualSetWidget();
    void testHsWidgetHostVisualSetVisualModel();
    void testHsWidgetHostVisualEffects();
    void testHsWidgetHostVisualEventFilter();
    void testHsWidgetHostVisualGestureEvent();
    void testHsWidgetHostVisualSetupTouchArea();
    void testHsWidgetHostVisualSetupEffects();
    void testHsWidgetHostVisualSetNewSize();

    //t_hsidlewidget
    void testIdleWidgetConstruction();
    void testIdleWidgetSetGeometry();
    void testIdleWidgetSetSceneX();
    void testIdleWidgetSetActivePage();
    void testIdleWidgetInsertPage();
    void testIdleWidgetRemovePage();
    void testIdleWidgetLayers();
    void testIdleWidgetTrashbin();
    void testIdleWidgetPageIndicator();
    void testIdleWidgetParallaxFactor();
    void testIdleWidgetShowTrashBin();
    void testIdleWidgetShowPageIndicator();
    void testIdleWidgetShowSnapLines();
    void testIdleWidgetPolishEvent();

    // testshortcutservice.cpp
    void testShortcutService();

    //t_hsdatabase.cpp
    void testHsDatabaseConstruction();
    void testHsDatabaseProperties();
    void testHsDatabaseStaticInterface();
    void testHsDatabaseTransaction();
    void testHsDatabaseSceneMethods();
    void testHsDatabasePageMethods();
    void testHsDatabaseWidgetMethods();
    void testHsDatabaseWidgetPreferenceMethods();
	void testHsDatabaseBadConnection();
    void testHsDatabaseMatchWidgetsPreferences_data();
    void testHsDatabaseMatchWidgetsPreferences();
    void testHsDatabaseWidgetsCount();
    void testHsDatabaseUpdateWidgetZValues();

    //t_hswidgethost.cpp
    void testHsWidgetHostConstruction();
    void testHsWidgetHostStateMachine();
    void testHsWidgetHostCloseAndRemove();
    void testHsWidgetHostPage();
    void testHsWidgetHostEffects();
    void testHsWidgetHostShowAndHideActions();
    void testHsWidgetHostPresentation();
    void testHsWidgetHostOnError();
    void testHsWidgetHostOnSetPreferences();
    void testHsWidgetHostShape();
    void testHsWidgetHostShapeInvalidValues();
    void testHsWidgetHostGestureEvent();

    //testcontentservice.cpp
    void testCreateNDeleteWidget();
    void testContentServiceStaticInterface();
    void testContentServiceWidgetCount();

    //t_hswallpaperloaderthread.cpp
    void testHsWallpaperLoaderThreadConstruction();
    void testHsWallpaperLoaderThreadRun();

    //t_hswallpaperloader.cpp
    void testHsWallpaperLoaderConstruction();
    void testHsWallpaperLoaderStartWithFinished();
    void testHsWallpaperLoaderStartWithFailed();
    void testHsWallpaperLoaderCancel();

    //t_hswallpaper.cpp
    void testHsWallpaperConstruction();
    void testHsWallpaperSetNRemoveDefaultImage();
    void testHsWallpaperSetNRemoveImage();
    void testHsWallpaperLoaderFailed();
    void testHsWallpaperScene();
    void testHsWallpaperPage();
    void testHsWallpaperSetImages();

    //t_hspage.cpp
    void testHsPage();
    void testHsPageLoad();
    void testHsPageContentRects();
    void testHsPageAddRemoveWidget();
    void testHsPageAddExistingWidget();
    void testHsPageWidgetFinished();
    void testHsPageIsActivePage();
    void testHsPageWidgetSignals();
    void testHsPageOrientationChange();
    void testHsPageDeleteFromDatabase();
    void testHsPageOnPageMarginChanged();
    void testHsPageIndex();

    //t_hspagenewwidgetlayout.cpp
    void testHsPageNewWidgetLayout();
    void testHsPageNewWidgetLayoutWithPosition();

    //t_hsscene.cpp
    void testSceneDefaultValues();
    void testSceneStaticInterface();
    void testSceneLoad();
    void testSceneAddPages();
    void testSceneOnline();
    void testSceneEventFilter();

    //t_hswidgetcomponent.cpp
    void testWidgetComponent();
    void testWidgetComponentEmits();

    //t_hswidgetcomponentregistry.cpp
    void testWidgetComponentRegistry();
    void testWidgetComponentRegistryOnEntryChanged();

    // hswidgetcomponentdescriptor.cpp
    void testWidgetComponentDescriptor();

    // t_hsbackuprestoreobserver.cpp
    void testBackupRestoreObserver();
    void testBackupRestoreObserverStarted();
    void testBackupRestoreObserverCompleted();
#ifdef Q_OS_SYMBIAN
    void testBackupRestoreObserverPsKeyObserver();
#endif //Q_OS_SYMBIAN
    // t_hsbackuprestoreactivecallback.cpp
    void testBackupRestoreActiveCallback();

    // t_hsconfiguration.cpp
    void testConfigurationConstruction();
    void testConfigurationStaticInterface();
    void testConfigurationSizes();
    void testConfigurationImportAndExport();
    void testConfigurationLoad();

    // t_hspagetoucharea.cpp
    void testPageTouchAreaConstruction();
    void testPageTouchAreaGestureEvent();

    // t_hswidgettoucharea.cpp
    void testWidgetTouchAreaConstruction();
    void testWidgetTouchAreaSceneEvent();
    void testWidgetTouchAreaGestureEvent();
		
	// hsdocumentloader.cpp
    void testDocumentLoaderConstructors();
    void testDocumentLoaderCreateObjects();

    // t_hswidgetpositioningonwidgetadd
    void testWidgetPositioningOnWidgetAddChangeInstance();
    void testHsAnchorPointInBottomRight_data();
    void testHsAnchorPointInBottomRight();
    void testHsWidgetOrganizerCase1_data();
    void testHsWidgetOrganizerCase1();
    void testHsWidgetOrganizerCase2_data();
    void testHsWidgetOrganizerCase2();
    void testHsWidgetOrganizerCase3_data();
    void testHsWidgetOrganizerCase3(); 
	
    /**
    * hsgui.cpp
    */
    void testGuiStaticInterface();
    void testSetupCleanupIdleUi();
    void testOrientation();
    void testShow();
    void testPageChangeAnimation();
    void testPageCrawlingAnimation();

    /**
    * hshostedwidgetfactory.cpp
    */
    void testHostedWidgetFactoryStaticInterface();
private:
    void createPagesToDatabase(int pageCount);
    void createSceneWithPages(int pageCount=1,int activePageIndex=0);
    void destroySceneAndPages();
    QString sourceTestWallpaper();
    QString targetPortraitTestWallpaper();
    QString targetLandscapeTestWallpaper();
    QString tempTestWallpaper();
    QString notExistingTestWallpaper();
    QString wallpaperDirectory();
    void cleanupTargetWallpaperFiles();

signals:
    void quitStateMachine();

private:
    HbMainWindow mainWindow;

    QSharedPointer<CaService> mCaService;
    bool mWallpaperTypeScene; //set true at init()
};
