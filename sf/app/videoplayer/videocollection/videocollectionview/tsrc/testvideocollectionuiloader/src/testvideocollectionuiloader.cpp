/**
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
* Description:   tester for methods in VideoCollectionUiLoader
*
*/

#include <hbdocumentloader.h>
#include <hbgroupbox.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbview.h>
#include <qhash.h>
#include <qmap.h>
#include <qset.h>
#include <qdebug.h>

#include "videolistwidgetdata.h"
#include "testvideocollectionuiloader.h"
#include "videocollectionwrapperdata.h"

#define private public
#include "videocollectionuiloader.h"
#include "videocollectionuiloaderdef.h"
#undef private

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestVideoCollectionUiLoader tv;
    int res;
    if (argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testvideocollectionuiloader.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

TestVideoCollectionUiLoader::TestVideoCollectionUiLoader():
    mTestObject(0)
{
    
}

TestVideoCollectionUiLoader::~TestVideoCollectionUiLoader()
{
    cleanup();
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::init()
{
    // create test object
    if (!mTestObject)
    {
        mTestObject = new VideoCollectionUiLoader;
    }
    
    // clear object list
    mObjects.clear();
    
    HbDocumentLoader::cleanup();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::cleanup()
{
    // delete test object
    delete mTestObject;
    mTestObject = 0;
}

// TEST CASES START ----------------------------------------------------------

// ---------------------------------------------------------------------------
// testAddData
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testAddData()
{
    VideoCollectionUiLoaderParam banner(
        DOCML_NAME_VC_HEADINGBANNER,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    VideoCollectionUiLoaderParam collectionWidget(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    VideoCollectionUiLoaderParam collectionContentWidget(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    const int COUNT = 3;
    
    // add items
    QList<VideoCollectionUiLoaderParam> params;
    params.append(banner);
    params.append(collectionWidget);
    params.append(collectionContentWidget);
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    QVERIFY(mTestObject->mQueue.count() == COUNT);
    QVERIFY(mTestObject->mQueue.contains(banner));
    QVERIFY(mTestObject->mQueue.contains(collectionWidget));
    QVERIFY(mTestObject->mQueue.contains(collectionContentWidget));
    
    // add duplicate items
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    QVERIFY(mTestObject->mQueue.count() == COUNT);
    QVERIFY(mTestObject->mQueue.contains(banner));
    QVERIFY(mTestObject->mQueue.contains(collectionWidget));
    QVERIFY(mTestObject->mQueue.contains(collectionContentWidget));
    
    // load first phase and add duplicate items
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QTest::qWait(1000); // wait 1 second
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    QCOMPARE(mTestObject->mQueue.count(), 0);
}

// ---------------------------------------------------------------------------
// testFindWidget
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testFindWidget()
{
    HbDocumentLoader::cleanup();
    
    QList<VideoCollectionUiLoaderParam> params;
    VideoCollectionUiLoaderParam banner(
        DOCML_NAME_VC_HEADINGBANNER,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    
    // pre-load widget asynchronously and find widget
    params.append(banner);
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QTest::qWait(1000); // wait 1 second
    HbGroupBox *bannerWidget =
        mTestObject->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER);
    QVERIFY(bannerWidget);
    QVERIFY(mObjects.contains(DOCML_NAME_VC_HEADINGBANNER));
    params.clear();
    cleanup();
    init();

    // do not pre-load widget and load it, docml not loaded
    HbDocumentLoader::mFindWidgetFails = true;
    bannerWidget =
        mTestObject->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER);
    QVERIFY(!bannerWidget);
    QVERIFY(!mObjects.contains(DOCML_NAME_VC_HEADINGBANNER));
    cleanup();
    init();

    // do not pre-load widget but add it in queue, docml loaded
    bool ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    params.append(banner);
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    
    // first test the findWidget with loadIfNotFound == false.
    bannerWidget =
        mTestObject->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER, false);
    QVERIFY(!bannerWidget);
    QVERIFY(!mObjects.contains(DOCML_NAME_VC_HEADINGBANNER));
    
    // then with default value.
    bannerWidget =
        mTestObject->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER);
    QVERIFY(bannerWidget);
    QVERIFY(mObjects.contains(DOCML_NAME_VC_HEADINGBANNER));
    params.clear();
    cleanup();
    init();

    // do not pre-load widget, docml loaded
    ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    bannerWidget =
        mTestObject->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER);
    QVERIFY(bannerWidget);
    QVERIFY(!mObjects.contains(DOCML_NAME_VC_HEADINGBANNER));
    cleanup();
    init();
}

// ---------------------------------------------------------------------------
// testFindObject
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testFindObject()
{
    HbDocumentLoader::cleanup();
    
    QList<VideoCollectionUiLoaderParam> params;
    VideoCollectionUiLoaderParam addToCollectionParam(
        DOCML_NAME_ADD_TO_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    
    // find object:
    // -docml not loaded
    // -object not added in loading queue
    HbDocumentLoader::mFindObjectFails = true;
    HbAction *addToCollection = mTestObject->findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
    QVERIFY(!addToCollection);
    QVERIFY(!mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    cleanup();
    init();

    // find object:
    // -docml not loaded
    // -object added in loading queue
    params.append(addToCollectionParam);
    mTestObject->addData(params, this, SLOT(handleObjectReady(QObject*, const QString&)));
    
    // first test with loadIfNotFound == false
    addToCollection = mTestObject->findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION, false);
    QVERIFY(!addToCollection);
    QVERIFY(!mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    
    // then with default value.
    addToCollection = mTestObject->findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
    QVERIFY(addToCollection);
    QVERIFY(mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    params.clear();
    cleanup();
    init();

    // find object:
    // -docml loaded
    // -object not added in loading queue
    bool ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    addToCollection = mTestObject->findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
    QVERIFY(addToCollection);
    QVERIFY(!mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    cleanup();
    init();

    // find object:
    // -docml loaded
    // -object added in loading queue
    ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    params.append(addToCollectionParam);
    mTestObject->addData(params, this, SLOT(handleObjectReady(QObject*, const QString&)));
    addToCollection = mTestObject->findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
    QVERIFY(addToCollection);
    QVERIFY(mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    params.clear();
}

// ---------------------------------------------------------------------------
// testIsService
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testIsService()
{
    // is service:
    // -service flag set
    mTestObject->setIsService(true);
    QVERIFY(mTestObject->mIsService);
    cleanup();
    init();
    
    // is service:
    // -service flag not set    
    mTestObject->setIsService(false);
    QVERIFY(!mTestObject->mIsService);
}

// ---------------------------------------------------------------------------
// testLoadDocml
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testLoadDocml()
{
    HbDocumentLoader::mLoadReturns.append(new QObject());
    
    // load docml:
    // -correct docml once
    bool ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mDocmls.contains(DOCML_VIDEOCOLLECTIONVIEW_FILE));
    cleanup();
    init();
    
    // load docml:
    // -two correct docmls
    ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mDocmls.contains(DOCML_VIDEOCOLLECTIONVIEW_FILE));
    mTestObject->load(DOCML_VIDEOSELECTIONDIALOG_FILE, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mDocmls.contains(DOCML_VIDEOSELECTIONDIALOG_FILE));
    cleanup();
    init();

    // load docml:
    // -same docml twice
    ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mDocmls.contains(DOCML_VIDEOCOLLECTIONVIEW_FILE));
    int objectCount = mTestObject->mOrphans.count();
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mDocmls.contains(DOCML_VIDEOCOLLECTIONVIEW_FILE));
    QVERIFY(objectCount == mTestObject->mOrphans.count());
    cleanup();
    init();

    // load docml:
    // -incorrect docml
    const char *INVALID_DOCML = "invalid docml";
    ok = false;
    HbDocumentLoader::mLoadFails = true;
    mTestObject->load(INVALID_DOCML, &ok);
    QVERIFY(!ok);
    QVERIFY(!mTestObject->mDocmls.contains(INVALID_DOCML));
}

// ---------------------------------------------------------------------------
// testLoadSection
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testLoadSection()
{
    HbDocumentLoader::cleanup();
    
    // load section:
    // -docml loaded
    // -correct section once
    bool ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mSections.contains(DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST));
    cleanup();
    init();

    // load section:
    // -docml loaded
    // -same section twice
    ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST, &ok);
    QVERIFY(ok);
    QVERIFY(mTestObject->mSections.contains(DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST));
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST, &ok);
    int sectionCount = mTestObject->mSections.count();
    QVERIFY(ok);
    QVERIFY(mTestObject->mSections.contains(DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST));
    QVERIFY(sectionCount == mTestObject->mSections.count());
    cleanup();
    init();

    // load section:
    // -docml not loaded
    // -correct section once
    ok = false;
    HbDocumentLoader::mLoadFails = true;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST, &ok);
    QVERIFY(!ok);
    QVERIFY(!mTestObject->mSections.contains(DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST));
    cleanup();
    init();
}

// ---------------------------------------------------------------------------
// testLoadPhase
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testLoadPhase()
{
    HbDocumentLoader::cleanup();
    
    // load phase:
    // -add first phase items in queue
    // -load first phase
    VideoCollectionUiLoaderParam videoWidgetParam(
        DOCML_NAME_VC_VIDEOLISTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    VideoCollectionUiLoaderParam collectionWidgetParam(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    VideoCollectionUiLoaderParam collectionContentWidgetParam(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QList<VideoCollectionUiLoaderParam> params;
    params.append(videoWidgetParam);
    params.append(collectionWidgetParam);
    params.append(collectionContentWidgetParam);
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QTest::qWait(1000); // wait 1 second
    QVERIFY(mTestObject->mPhases.contains(VideoCollectionUiLoaderParam::LoadPhasePrimary));
    QCOMPARE(mTestObject->mObjects.count(), 3);
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_VC_VIDEOLISTWIDGET));
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_VC_COLLECTIONWIDGET));
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET));
    params.clear();
    cleanup();
    init();

    // load phase:
    // -add second phase items in queue
    // -load first phase
    // -load second phase
    VideoCollectionUiLoaderParam addToCollectionParam(
        DOCML_NAME_ADD_TO_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    VideoCollectionUiLoaderParam createCollectionParam(
        DOCML_NAME_CREATE_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    VideoCollectionUiLoaderParam deleteMultipleParam(
        DOCML_NAME_DELETE_MULTIPLE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    params.append(addToCollectionParam);
    params.append(createCollectionParam);
    params.append(deleteMultipleParam);
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QTest::qWait(1000); // wait 1 second
    QVERIFY(mTestObject->mPhases.contains(VideoCollectionUiLoaderParam::LoadPhasePrimary));
    QVERIFY(mTestObject->mObjects.count() == 0);
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    QTest::qWait(1000); // wait 1 second
    QVERIFY(mTestObject->mPhases.contains(VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    QCOMPARE(mTestObject->mObjects.count(), 3);
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_CREATE_COLLECTION));
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_DELETE_MULTIPLE));
    params.clear();
    cleanup();
    init();

    // load phase:
    // -add first phase items in queue
    // -add second phase items in queue
    // -load first phase
    // -load second phase
    params.append(videoWidgetParam);
    params.append(addToCollectionParam);
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    QTest::qWait(1000); // wait 1 second
    QVERIFY(mTestObject->mPhases.contains(VideoCollectionUiLoaderParam::LoadPhasePrimary));
    QVERIFY(mTestObject->mPhases.contains(VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    QCOMPARE(mTestObject->mObjects.count(), 2);
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_VC_VIDEOLISTWIDGET));
    QVERIFY(mTestObject->mObjects.contains(DOCML_NAME_ADD_TO_COLLECTION));
    params.clear();
    cleanup();
    init();
}

// ---------------------------------------------------------------------------
// testLoadPhase_2
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testLoadPhase_2()
{
    // load phase:
    // -add all items
    // -load all items
    QList<VideoCollectionUiLoaderParam> params;
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VIEW,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_HEADINGBANNER,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_VIDEOLISTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_OPTIONS_MENU,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_MENU,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_DATE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_NAME,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_SIZE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_ADD_TO_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_CREATE_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DELETE_MULTIPLE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_NO_CONTENT_LABEL,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DIALOG,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DLG_HEADINGLBL,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_CHECK_CONTAINER,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_MARKALL,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_LBL_SELECTION,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_LIST_CONTAINER,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    QTest::qWait(5000); // 5 seconds
    QCOMPARE(mObjects.count(), params.count());
    QHash<QString, QObject*>::const_iterator i = mObjects.constBegin();
    while (i != mObjects.constEnd())
    {
        bool found = false;
        foreach (const VideoCollectionUiLoaderParam &param, params)
        {
            if (param == i.key())
            {
                found = true;
                break;
            }
        }
        QVERIFY(found);
        i++;
    }
    QCOMPARE(VideoListWidgetData::mInitializeCount, 3);
    
    // Getting proxymodel fails.
    cleanup();
    init();
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    VideoCollectionWrapperData::mGetAllVideosModelFails = true;
    VideoCollectionWrapperData::mGetCollectionsModelFails = true;
    VideoCollectionWrapperData::mGetCollectionContentModelFails = true;
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhaseSecondary);
    QTest::qWait(5000); // 5 seconds
    QCOMPARE(mObjects.count(), params.count());
    i = mObjects.constBegin();
    while (i != mObjects.constEnd())
    {
        bool found = false;
        foreach (const VideoCollectionUiLoaderParam &param, params)
        {
            if (param == i.key())
            {
                found = true;
                break;
            }
        }
        QVERIFY(found);
        i++;
    }
    params.clear();
    QCOMPARE(VideoListWidgetData::mInitializeCount, 0);
}

// ---------------------------------------------------------------------------
// testDelete
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testDelete()
{
    VideoCollectionWrapperData::reset();
    
    // delete:
    // -load docml
    // -delete one "root" item
    bool ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    QVERIFY(ok);
    HbAction *addToCollection = mTestObject->findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
    HbAction *tempAction = addToCollection;
    delete addToCollection;
    QVERIFY(!mTestObject->mOrphans.contains(tempAction));
    cleanup();
    init();

    // delete:
    // -load docml
    // -delete all items
    ok = false;
    mTestObject->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    foreach (QObject *object, mTestObject->mOrphans)
    {
        delete object;
    }
    QVERIFY(!mTestObject->mOrphans.count());
    QVERIFY(ok);
}

// ---------------------------------------------------------------------------
// testRemoveOrphanFromList
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testRemoveOrphanFromList()
{
    QObject *obj = 0;
    
    // Nothing in the object lists.
    cleanup();
    init();
    connect(this, SIGNAL(testSignal(QObject *)), mTestObject, SLOT(removeOrphanFromList(QObject *)));
    emit testSignal(obj);
    disconnect(this, SIGNAL(testSignal(QObject *)), mTestObject, SLOT(removeOrphanFromList(QObject *)));
    QCOMPARE(mTestObject->mOrphans.count(), 0);
    QCOMPARE(mTestObject->mObjects.count(), 0);
    
    // Orphans and objects contain the object.
    cleanup();
    init();
    connect(this, SIGNAL(testSignal(QObject *)), mTestObject, SLOT(removeOrphanFromList(QObject *)));
    obj = new QObject();
    mTestObject->mOrphans.append(obj);
    mTestObject->mObjects.insert("test", obj); 
    emit testSignal(obj);
    disconnect(this, SIGNAL(testSignal(QObject *)), mTestObject, SLOT(removeOrphanFromList(QObject *)));
    QCOMPARE(mTestObject->mOrphans.count(), 0);
    QCOMPARE(mTestObject->mObjects.count(), 0);
    delete obj;

    // Orphans contain the object, objects has different
    cleanup();
    init();
    connect(this, SIGNAL(testSignal(QObject *)), mTestObject, SLOT(removeOrphanFromList(QObject *)));
    obj = new QObject();
    mTestObject->mOrphans.append(obj);
    mTestObject->mObjects.insert("test", new QObject()); 
    emit testSignal(obj);
    disconnect(this, SIGNAL(testSignal(QObject *)), mTestObject, SLOT(removeOrphanFromList(QObject *)));
    QCOMPARE(mTestObject->mOrphans.count(), 0);
    QCOMPARE(mTestObject->mObjects.count(), 1);
    delete obj;
}

// TEST CASES END -------------------------------------------------------------

void TestVideoCollectionUiLoader::handleObjectReady(QObject *object, const QString &name)
{
    qDebug() << "Received object :" << name;
    
    if (object)
    {
        if (name == DOCML_NAME_VIEW)
        {
            if(hbInstance->allMainWindows().count())
            {
                HbMainWindow *window = hbInstance->allMainWindows().at(0);
                HbView *view = qobject_cast<HbView*>(object);
                if (view)
                {
                    window->setCurrentView(view);
                }
            }
        }
        mObjects.insert(name, object);
    }
}

// ---------------------------------------------------------------------------
// testPrepareDocmlAndSection
// ---------------------------------------------------------------------------
//
void TestVideoCollectionUiLoader::testPrepareDocmlAndSection()
{
    VideoCollectionUiLoaderParam banner(
        DOCML_NAME_VC_HEADINGBANNER,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    VideoCollectionUiLoaderParam collectionWidget(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        0,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    VideoCollectionUiLoaderParam collectionContentWidget(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        0,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary);
    const int COUNT = 3;
    
    // add items
    QList<VideoCollectionUiLoaderParam> params;
    params.append(banner);
    params.append(collectionWidget);
    params.append(collectionContentWidget);
    
    cleanup();
    init();
    HbDocumentLoader::mFindWidgetFails = true;
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QTest::qWait(1000); // wait 1 second
    QVERIFY(mTestObject->mQueue.count() == 0);
    QVERIFY(!mTestObject->mQueue.contains(banner));
    QVERIFY(!mTestObject->mQueue.contains(collectionWidget));
    QVERIFY(!mTestObject->mQueue.contains(collectionContentWidget));
    QCOMPARE(mTestObject->mQueue.count(), 0);    

    cleanup();
    init();
    HbDocumentLoader::mFindWidgetFails = true;
    HbDocumentLoader::mLoadFails = true;
    mTestObject->addData(params,
        this,
        SLOT(handleObjectReady(QObject*, const QString&)));
    mTestObject->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    QTest::qWait(1000); // wait 1 second
    QVERIFY(mTestObject->mQueue.count() == 0);
    QVERIFY(!mTestObject->mQueue.contains(banner));
    QVERIFY(!mTestObject->mQueue.contains(collectionWidget));
    QVERIFY(!mTestObject->mQueue.contains(collectionContentWidget));
    QCOMPARE(mTestObject->mQueue.count(), 0);    
}

// end of file
