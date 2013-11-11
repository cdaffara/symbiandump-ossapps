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

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QDir>

#include <HbMainWindow>
#include <HbVkbHost>

#include "hsidlewidget.h"
#include "hsscene.h"
#include "hspage.h"
#include "hspagevisual.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hstrashbinwidget.h"
#include "hspageindicator.h"
#include "hsdocumentloader.h"
#include "hsconfiguration.h"
#include "hsapp_defs.h"
#include "hssnapline.h"
#include "hsgui.h"


namespace
{
    const char gControlLayerDocmlName[] = "controllayer.docml";
    const char gControlLayerName[]      = "controlLayer";
    const char gTrashBinName[]          = "trashBin";
    const char gPageIndicatorName[]     = "pageIndicator";
}

/*!
    \class HsIdleWidget
    \ingroup group_hshomescreenstateplugin
    \brief View part of the home screen idle state.

    Maintains the idle view ui layers and takes care of
    receiving user input and communicating it to the idle
    state for further processing.
*/

/*!
    Constructs a new idle widget with the given \a parent.
*/
HsIdleWidget::HsIdleWidget(QGraphicsItem *parent)
  : HbWidget(parent),
    mControlLayer(0), mPageLayer(0), mPageWallpaperLayer(0),
    mSceneLayer(0),
    mTrashBin(0), mPageIndicator(0),
    mHorizontalSnapLine(0), mVerticalSnapLine(0)
{
    setFlag(ItemHasNoContents);

    loadControlLayer();

    QGraphicsLinearLayout *linearLayout = 0;

    linearLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    linearLayout->setContentsMargins(0, 0, 0, 0);
    linearLayout->setSpacing(0);
    mPageLayer = new HbWidget(this);
    mPageLayer->setLayout(linearLayout);
    mPageLayer->setZValue(2);
    
    linearLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    linearLayout->setContentsMargins(0, 0, 0, 0);
    linearLayout->setSpacing(0);
    mPageWallpaperLayer = new HbWidget(this);
    mPageWallpaperLayer->setLayout(linearLayout);
    mPageWallpaperLayer->setZValue(1);
    
    linearLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    linearLayout->setContentsMargins(0, 0, 0, 0);
    linearLayout->setSpacing(0);
    mSceneLayer = new HbWidget(this);
    mSceneLayer->setLayout(linearLayout);
    mSceneLayer->setZValue(0);
}

/*!
    Destroys this idle widget.
*/
HsIdleWidget::~HsIdleWidget()
{
    QList<HsPage *> pages = HsScene::instance()->pages();
    foreach (HsPage *page, pages) {
        page->visual()->setParentItem(0);
        if (page->visual()->scene()) {
            page->visual()->scene()->removeItem(page->visual());
        }
        HsWallpaper *pageWallpaper = page->wallpaper();
        if (pageWallpaper) {
            pageWallpaper->setParentItem(0);
            if (pageWallpaper->scene()) {
                pageWallpaper->scene()->removeItem(pageWallpaper);
            }
        }
    }

    HsWallpaper *sceneWallpaper = HsScene::instance()->wallpaper();
    if (sceneWallpaper) {
        sceneWallpaper->setParentItem(0);
        if (sceneWallpaper->scene()) {
            sceneWallpaper->scene()->removeItem(sceneWallpaper);
        }
    }
}

qreal HsIdleWidget::sceneX() const
{
    return mPageLayer->x();
}

void HsIdleWidget::setSceneX(qreal x)
{
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::SceneWallpaper) {
        mPageLayer->setX(x);
        mSceneLayer->setX((parallaxFactor() * x) - HSCONFIGURATION_GET(bounceEffect) / 2);
    } else {
        mPageLayer->setX(x);
        mPageWallpaperLayer->setX(x);
    }
}

/*!
    Layouts the ui layers according to the given \a rect.
*/
void HsIdleWidget::setGeometry(const QRectF &rect)
{
    
    int n = HsScene::instance()->pages().count();    
    mControlLayer->resize(rect.size());
    mPageLayer->resize(n * rect.width(), rect.height());
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        mPageWallpaperLayer->resize(n * rect.width(), rect.height());
    }
    mSceneLayer->resize(2 * rect.width() + HSCONFIGURATION_GET(bounceEffect), rect.height());
    HbWidget::setGeometry(rect);
}

/*!
    Sets the active page \a index to the page
    indicator.
*/
void HsIdleWidget::setActivePage(int index)
{
    mPageIndicator->setActiveItemIndex(index);
}

/*!
    Inserts the given \a page at index position
    \a index in the page layer.
*/
void HsIdleWidget::insertPage(int index, HsPage *page)
{
    QGraphicsLinearLayout *layout =
        static_cast<QGraphicsLinearLayout *>(mPageLayer->layout());
    layout->insertItem(index, page->visual());
    mPageLayer->resize(
        layout->count() * size().width(), size().height());

    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        QGraphicsLinearLayout *layout =
            static_cast<QGraphicsLinearLayout *>(mPageWallpaperLayer->layout());
        layout->insertItem(index, page->wallpaper());
        mPageWallpaperLayer->resize(
            layout->count() * size().width(), size().height());
    }
}

/*!
    Removes the page at index position
    \a index in the page layer.
*/
void HsIdleWidget::removePage(int index)
{
    QGraphicsLinearLayout *layout =
        static_cast<QGraphicsLinearLayout *>(mPageLayer->layout());
    layout->removeAt(index);
    mPageLayer->resize(
        layout->count() * size().width(), size().height());

    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        QGraphicsLinearLayout *layout =
            static_cast<QGraphicsLinearLayout *>(mPageWallpaperLayer->layout());
        layout->removeAt(index);
        mPageWallpaperLayer->resize(
            layout->count() * size().width(), size().height());
    }
    mPageIndicator->removeItem(index);
}

/*!
    \fn HsIdleWidget::controlLayer() const

    Returns the control layer.
*/

/*!
    \fn HsIdleWidget::pageLayer() const

    Returns the page layer.
*/

/*!
    \fn HsIdleWidget::sceneLayer() const

    Returns the scene layer.
*/

/*!
    \fn HsIdleWidget::trashBin() const

    Returns the trashbin widget.
*/

/*!
    \fn HsIdleWidget::pageIndicator() const

    Returns the page indicator widget.
*/

/*!
    Sets the trashbin visible and hides the page indicator.
*/
void HsIdleWidget::showTrashBin()
{
    mPageIndicator->hide();
    mTrashBin->show();
}

/*!
    Sets the page indicator visible and hides the trashbin.
*/
void HsIdleWidget::showPageIndicator()
{
    mTrashBin->hide();
    mTrashBin->deactivate();
    mPageIndicator->setSpacing(HSCONFIGURATION_GET(pageIndicatorSpacing)); // for usability optimization widget, can be removed later on
    mPageIndicator->setVisible(1 < mPageIndicator->itemCount());
}

/*!
    Shows the Vertical snapping lines showing the guidance
*/
void HsIdleWidget::showVerticalSnapLine(const QLineF &snapLine)
{
    QVariantHash snapConfiguration;
    snapConfiguration[Hs::snapLineFadeinDuration] = QString::number(HSCONFIGURATION_GET(snapLineFadeInDuration));
    snapConfiguration[Hs::snapLineFadeoutDuration] = QString::number(HSCONFIGURATION_GET(snapLineFadeOutDuration));

    mVerticalSnapLine->setConfiguration(snapConfiguration);
    mVerticalSnapLine->showLine(snapLine);
}

/*!
    Shows the Horizontal snapping lines showing the guidance
*/
void HsIdleWidget::showHorizontalSnapLine(const QLineF &snapLine)
{
    QVariantHash snapConfiguration;
    snapConfiguration[Hs::snapLineFadeinDuration] = QString::number(HSCONFIGURATION_GET(snapLineFadeInDuration));
    snapConfiguration[Hs::snapLineFadeoutDuration] = QString::number(HSCONFIGURATION_GET(snapLineFadeOutDuration));

    mHorizontalSnapLine->setConfiguration(snapConfiguration);
    mHorizontalSnapLine->showLine(snapLine);
}

/*!
    Hides the Vertical snapping line showing the guidance
*/
void HsIdleWidget::hideVerticalSnapLine()
{
    mVerticalSnapLine->hideLine();
}

/*!
    Hides the Horizontal snapping line showing the guidance
*/
void HsIdleWidget::hideHorizontalSnapLine()
{
    mHorizontalSnapLine->hideLine();
}

/*!
    Reimplements QGraphicsWidget::polishEvent().
*/
void HsIdleWidget::polishEvent()
{
    HsScene *scene = HsScene::instance();
    Q_ASSERT(scene);

    QGraphicsLinearLayout *pageLayout = 
        static_cast<QGraphicsLinearLayout *>(mPageLayer->layout());

    QList<HsPage *> pages = scene->pages();

    foreach (HsPage *page, pages) {
        pageLayout->addItem(page->visual());
        if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
            QGraphicsLinearLayout *pageWallpaperLayout = 
                static_cast<QGraphicsLinearLayout *>(mPageWallpaperLayer->layout());
            pageWallpaperLayout->addItem(page->wallpaper());
        }
    }
   if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::SceneWallpaper) {
        QGraphicsLinearLayout *sceneLayout = 
            static_cast<QGraphicsLinearLayout *>(mSceneLayer->layout());
    	HsWallpaper *wallpaper = HsScene::instance()->wallpaper();
        sceneLayout->addItem(wallpaper);
    }
           
    mPageIndicator->initialize(pages.count(), scene->activePageIndex());    
    showPageIndicator();

   // HsGui::instance()->scene()->installEventFilter(this);
}

/*!
    Loads the control layer declared in a docml file.
*/
void HsIdleWidget::loadControlLayer()
{
    HsDocumentLoader loader;
    bool loaded = false;

#ifndef Q_OS_SYMBIAN
    QString path = QDir::currentPath();
#else
    QString path = "c:";
#endif

    QString file = path + "/hsresources/" + gControlLayerDocmlName;
    QString fallbackPath = QString(":/") + gControlLayerDocmlName;

    if (QFile::exists(file)) {
        loader.load(file, &loaded);
        if (!loaded) {
            loader.load(fallbackPath, &loaded);
        }
    } else {
        loader.load(fallbackPath, &loaded);
    }

    if (loaded) {
        mControlLayer = qobject_cast<HbWidget *>(loader.findWidget(gControlLayerName));
        mControlLayer->setZValue(3);
        mControlLayer->setParentItem(this);

        mTrashBin = qobject_cast<HsTrashBinWidget *>(loader.findWidget(gTrashBinName));
        mTrashBin->setZValue(1e6);

        mPageIndicator = qobject_cast<HsPageIndicator *>(loader.findWidget(gPageIndicatorName));
        mPageIndicator->setZValue(1e6);


        mHorizontalSnapLine = new HsSnapLine(mControlLayer);
        mHorizontalSnapLine->setZValue(10);

        mVerticalSnapLine = new HsSnapLine(mControlLayer);
        mVerticalSnapLine->setZValue(10);
    } else {
        // TODO: Handle error.
    }
}

qreal HsIdleWidget::parallaxFactor() const
{   
    qreal clw = mControlLayer->size().width();
    qreal slw = mSceneLayer->size().width() - HSCONFIGURATION_GET(bounceEffect);
    int n = HsScene::instance()->pages().count();
    if (n < 2) {
        return 1;
    } else {
        return (slw - clw) / ((n - 1) * clw);
    }
}
