/*!
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

#include <QtGui>
#include <QtTest/QtTest>

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbstyle.h>

#include "bubbletest.h"
#include "bubbleimagemanager.h"

static const int WAIT_TIME = 500;

class ut_BubbleImageManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testLoadImage();
    void testLoadImageQueued();
    void testLoadNonExistingImage();

    void testUnloadImageOutstandingRequest();

    void testReleasePixmaps();

private:
    QString fileNameWithPath(const QString& fileName);

private:
    BubbleImageManager* mImageManager;
};

void ut_BubbleImageManager::initTestCase()
{
    mImageManager = new BubbleImageManager();
}

void ut_BubbleImageManager::cleanupTestCase()
{
    delete mImageManager;
}

QString ut_BubbleImageManager::fileNameWithPath(const QString& fileName)
{
#ifdef __WINS__
    return "c:/data/images/" + fileName;
#else
    return ":/resources/" + fileName;
#endif
}

void ut_BubbleImageManager::testLoadImage()
{
    QSignalSpy spy(mImageManager,SIGNAL(pixmapReady(QString)));

    QString imageFile1 = fileNameWithPath("testpic1.png");
    qDebug() << imageFile1;
    mImageManager->loadImage(imageFile1);

    // wait loading to finish
    QTest::qWait(4*WAIT_TIME);

    QVERIFY(spy.count()==1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == imageFile1);

    const QPixmap* pixmap1 = mImageManager->pixmap(imageFile1);
    QVERIFY(pixmap1!=0);

    // try to load cached image
    mImageManager->loadImage(imageFile1);
    QVERIFY(spy.count()==0);

    QString imageFile2 = fileNameWithPath("testpic2.jpg");
    qDebug() << imageFile2;
    mImageManager->loadImage(imageFile2);

    // wait loading to finish
    QTest::qWait(WAIT_TIME);

    QVERIFY(spy.count()==1);
    arguments.clear();
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == imageFile2);

    const QPixmap* pixmap2 = mImageManager->pixmap(imageFile2);
    QVERIFY(pixmap2!=0);

    mImageManager->unloadImage(imageFile1);
    mImageManager->unloadImage(imageFile2);
}

void ut_BubbleImageManager::testLoadImageQueued()
{
    QSignalSpy spy(mImageManager,SIGNAL(pixmapReady(QString)));

    QString imageFile1 = fileNameWithPath("testpic1.png");
    qDebug() << imageFile1;
    mImageManager->loadImage(imageFile1);

    QString imageFile2 = fileNameWithPath("testpic2.jpg");
    qDebug() << imageFile2;
    mImageManager->loadImage(imageFile2);

    // try to load images that are being loaded
    mImageManager->loadImage(imageFile1);
    mImageManager->loadImage(imageFile2);

    // wait loading to finish
    QTest::qWait(WAIT_TIME);

    QVERIFY(spy.count()==2);

    const QPixmap* pixmap1 = mImageManager->pixmap(imageFile1);
    QVERIFY(pixmap1!=0);

    const QPixmap* pixmap2 = mImageManager->pixmap(imageFile2);
    QVERIFY(pixmap2!=0);

    mImageManager->unloadImage(imageFile1);
    mImageManager->unloadImage(imageFile2);
}

void ut_BubbleImageManager::testLoadNonExistingImage()
{
    QSignalSpy spy(mImageManager,SIGNAL(pixmapReady(QString)));

    QString imageFile1 = fileNameWithPath("foobar.png");
    qDebug() << imageFile1;
    mImageManager->loadImage(imageFile1);

    // wait loading to finish
    QTest::qWait(WAIT_TIME);

    QVERIFY(spy.count()==0);

    const QPixmap* pixmap1 = mImageManager->pixmap(imageFile1);
    QVERIFY(pixmap1==0);

    mImageManager->unloadImage(imageFile1);
}

void ut_BubbleImageManager::testUnloadImageOutstandingRequest()
{
    QSignalSpy spy(mImageManager,SIGNAL(pixmapReady(QString)));

    QString imageFile1 = fileNameWithPath("testpic1.png");
    qDebug() << imageFile1;
    mImageManager->loadImage(imageFile1);
    QTest::qWait(5);
    mImageManager->unloadImage(imageFile1);
    mImageManager->unloadImage("foobar.png");

    // wait loading to finish
    QTest::qWait(WAIT_TIME);

    QVERIFY(spy.count()==0);

    const QPixmap* pixmap1 = mImageManager->pixmap(imageFile1);
    QVERIFY(pixmap1==0);

    QString imageFile2 = fileNameWithPath("testpic2.jpg");;
    qDebug() << imageFile2;
    mImageManager->loadImage(imageFile2);

    QTest::qWait(5);
    mImageManager->unloadImage(imageFile2);

    // wait loading to finish
    QTest::qWait(WAIT_TIME);

    QVERIFY(spy.count()==0);

    const QPixmap* pixmap2 = mImageManager->pixmap(imageFile2);
    QVERIFY(pixmap2==0);
}

void ut_BubbleImageManager::testReleasePixmaps()
{
    QString imageFile1 = fileNameWithPath("testpic1.png");
    qDebug() << imageFile1;
    mImageManager->loadImage(imageFile1);

    QString imageFile2 = fileNameWithPath("testpic2.jpg");
    qDebug() << imageFile2;
    mImageManager->loadImage(imageFile2);

    // wait loading to finish
    QTest::qWait(WAIT_TIME);

    const QPixmap* pixmap1 = mImageManager->pixmap(imageFile1);
    QVERIFY(pixmap1!=0);

    const QPixmap* pixmap2 = mImageManager->pixmap(imageFile2);
    QVERIFY(pixmap2!=0);

    mImageManager->releasePixmaps();

    pixmap1 = mImageManager->pixmap(imageFile1);
    QVERIFY(pixmap1==0);

    pixmap2 = mImageManager->pixmap(imageFile2);
    QVERIFY(pixmap2==0);
}

BUBBLE_TEST_MAIN(ut_BubbleImageManager)
#include "ut_bubbleimagemanager.moc"
