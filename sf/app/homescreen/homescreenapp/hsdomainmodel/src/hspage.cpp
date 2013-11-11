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

#include <QHash>

#include <HbInstance>

#include "hsdomainmodeldatastructures.h"
#include "hspage.h"
#include "hspagevisual.h"
#include "hspagetoucharea.h"
#include "hspagenewwidgetlayout.h"
#include "hsscene.h"
#include "hsdatabase.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsconfiguration.h"
#include "hsgui.h"


/*!
    \class HsPage
    \ingroup group_hsdomainmodel
    \brief Represents a page in the framework.
    HsPage contains group of widgets. HsPage can have a wallpaper.
*/

/*!
    Constructor.

    \a parent Owner.
    \a aFlags Window flags.
*/
HsPage::HsPage(QObject* parent)
    : QObject(parent),
      mDatabaseId(-1),
      mPageVisual(new HsPageVisual), 
      mWallpaper(0),
      mRemovable(true),
      mPageMargin(0.0)
{
 
    //Page margin
    mPageMargin = HSCONFIGURATION_GET(pageMargin);
    connect(HsConfiguration::instance(), SIGNAL(propertyChanged(QString)), SLOT(onPageMarginChanged(QString)));
}

/*!
    Destructor.
*/
HsPage::~HsPage()
{
    // visuals are owned by widget host, detach those from page visual
    HsWidgetHostVisual *visual(0);
    foreach (HsWidgetHost *widget, mWidgets) {
        visual = widget->visual();
        visual->setParent(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
    }
   
    foreach (HsWidgetHost *widget, mNewWidgets) {
        visual = widget->visual();
        visual->setParent(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
    }
   
    foreach (HsWidgetHost *widget, mUnavailableWidgets) {
        visual = widget->visual();
        visual->setParent(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
    }
    qDeleteAll(mWidgets);
    qDeleteAll(mNewWidgets);
    qDeleteAll(mUnavailableWidgets);

    delete mPageVisual;
    delete mWallpaper;
}

/*!
    Returns the database id.
*/
int HsPage::databaseId() const
{
    return mDatabaseId;
}

/*!
    Sets the database id to \a id.
*/
void HsPage::setDatabaseId(int id)
{
    mDatabaseId = id;
}
/*
void HsPage::setGeometry(const QRectF &rect)
{
    if (mTouchArea) {
        mTouchArea->resize(rect.size());
    }

    HbWidget::setGeometry(rect);
}
*/
/*!
    Loads widgets.
*/
bool HsPage::load()
{
    if (mDatabaseId < 0) {
        return false;
    }

    HsDatabase *db = HsDatabase::instance();

    QList<HsWidgetData> datas;
    if (!db->widgets(mDatabaseId, datas)) {
        return false;
    }

    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        mWallpaper = new HsPageWallpaper(this);
    }

    foreach (HsWidgetData data, datas) {
        HsWidgetHost *widget = new HsWidgetHost(data.id);
        if (!widget->loadPresentation(Qt::Horizontal) &&
            !widget->loadPresentation(Qt::Vertical)) {
            widget->setPage(this);
            widget->visual()->hide();
            connectWidget(widget);
            mNewWidgets << widget;            
        }
        else {
            mWidgets.append(widget);
            connectWidget(widget);
            widget->setPage(this);
            widget->visual()->setParentItem(this->visual());
            widget->startWidget(isDefaultPage());
        }
    }

    connect(HsGui::instance(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(onOrientationChanged(Qt::Orientation)));

    return true;
}

/*!
    Return wallpaper.
*/
HsWallpaper *HsPage::wallpaper() const
{
    return mWallpaper;
}
/*!
    Add given existing \a widgetHost to a page. Returns true if successful
*/
bool HsPage::addExistingWidget(HsWidgetHost *widgetHost)
{
    if (!widgetHost) {
        return false;
    }

    if (mWidgets.contains(widgetHost)) {
        return true;
    }

    if (!widgetHost->setPage(this)) {
        return false;
    }

    connectWidget(widgetHost);
    mWidgets << widgetHost;
    widgetHost->visual()->setParentItem(visual());

    return true;
 }

/*!
    Remove given \a widgetHost from a page. Widget is not deleted. 
    Returns true if successful
*/
bool HsPage::removeWidget(HsWidgetHost *widgetHost)
{
    if (!widgetHost || !widgetHost->setPage(0)) {
        return false;
    }

    disconnectWidget(widgetHost);
    mWidgets.removeOne(widgetHost);
    widgetHost->visual()->setParentItem(0);

    return true;
}

/*!
    Returns list of new widgets belonging to a page. Widgets which are
    not yet layouted are considered as new widgets. 
*/
QList<HsWidgetHost *> HsPage::newWidgets()
{
    return mNewWidgets;
}

/*!
    Adds new widget into a page. Returns true if successfull.
*/
bool HsPage::addNewWidget(HsWidgetHost* widgetHost, const QPointF &touchPoint)
{
    mTouchPoint = touchPoint;

    if (!widgetHost || mWidgets.contains(widgetHost)) {
        return false;
    }

    if (mNewWidgets.contains(widgetHost)) {
        return true;
    }
    
    widgetHost->setPage(this);    

    widgetHost->visual()->hide();

    connectWidget(widgetHost);
    mNewWidgets << widgetHost;

    return true;
}

/*!
    Layouts all the new widgets
*/
void HsPage::layoutNewWidgets()
{
    if (mNewWidgets.isEmpty()) {
        return;
    }

    HsPageNewWidgetLayout *newWidgetLayout = static_cast<HsPageNewWidgetLayout *>(visual()->layout());
    if (!newWidgetLayout) {
        newWidgetLayout = new HsPageNewWidgetLayout(mTouchPoint);
        visual()->setLayout(newWidgetLayout);
    }    
    updateZValues();
    HsWidgetHost *widget = 0;
    for (int i = 0; i < mNewWidgets.count(); ++i) {
        widget = mNewWidgets.at(i);
        newWidgetLayout->addItem(widget);
        widget->setPage(this);
        widget->visual()->setParentItem(visual());
        widget->showWidget();
        widget->visual()->show();
    }
    mWidgets << mNewWidgets;
    mNewWidgets.clear();
}

/*!
    Clears new widgets list and resets layout.
*/
void HsPage::resetNewWidgets()
{
    mNewWidgets.clear();
    visual()->setLayout(0);
}

/*!
    Remove page and all it's contained widgets from database 
*/
bool HsPage::deleteFromDatabase()
{
    
    HsWidgetHostVisual *visual(0);
    foreach (HsWidgetHost *widget, mWidgets) {
        visual = widget->visual();
        visual->setParent(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
        widget->remove();
    }
    mWidgets.clear();
    

    foreach (HsWidgetHost *widget, mNewWidgets) {
        visual = widget->visual();
        visual->setParent(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
        widget->remove();
    }
    mNewWidgets.clear();

    foreach (HsWidgetHost *widget, mUnavailableWidgets) {
        visual = widget->visual();
        visual->setParent(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
        widget->remove();
    }
    mUnavailableWidgets.clear();
    
    if (mWallpaper) {
        mWallpaper->remove();
    }
    return HsDatabase::instance()->deletePage(mDatabaseId);
}

/*!
    Return list of widgets belonging to a page 
*/
QList<HsWidgetHost *> HsPage::widgets() const
{
    return mWidgets;
}

/*!
    Returns true if the page can be removed. Otherwise,
    returns false.
*/
bool HsPage::isRemovable() const
{
    return mRemovable;
}

/*!
    Sets removable flag to \a removable.
*/
void HsPage::setRemovable(bool removable)
{
    mRemovable = removable;
}

/*!
    Return true if page is default page.
*/
bool HsPage::isDefaultPage() const
{
    return mDatabaseId == HSCONFIGURATION_GET(defaultPageId);
}

/*!
    Return true if page is active page.
*/
bool HsPage::isActivePage() const
{
    return this == HsScene::instance()->activePage();
}

/*!
    Create page into database and return instance of a new page.
*/
HsPage *HsPage::createInstance(const HsPageData &pageData)
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

    HsPageData data(pageData);
    if (db->insertPage(data)) {
        HsPage *page = new HsPage;
        page->setDatabaseId(data.id);
        return page;
    }

    return 0;
}

/*!
    The widget is bounded in the rectangle which is smaller by pageMargin on all sides of page.
*/
QPointF HsPage::adjustedWidgetPosition(const QRectF &origWidgetRect)
{
    QRectF widgetAreaRect = contentGeometry();
    qreal widgetX = qBound(widgetAreaRect.left(), origWidgetRect.x(), widgetAreaRect.right() - origWidgetRect.width());
    qreal widgetY = qBound(widgetAreaRect.top(), origWidgetRect.y(), widgetAreaRect.bottom() - origWidgetRect.height());

    return QPointF(widgetX, widgetY);
}

/*!
    Returns rect of rectangular where widgets are allowed to be placed in the page.
*/
QRectF HsPage::contentGeometry()
{
    return contentGeometry(HsGui::instance()->orientation());
}

/*!
    Returns rect of rectangular where widgets are allowed to be placed in the page.
*/
QRectF HsPage::contentGeometry(Qt::Orientation orientation)
{
    QRectF pageRect = HsGui::instance()->layoutRect();

    if (orientation != HsGui::instance()->orientation()) {
        qreal width = pageRect.width();
        qreal height = pageRect.height();
        pageRect.setWidth(height);
        pageRect.setHeight(width);
    }

    //Take care of chrome in both orientation
    pageRect.setTop(64);

    //Shrink by page margins at each side
    return pageRect.adjusted(mPageMargin, mPageMargin, -mPageMargin, -mPageMargin);
}

/*!
    Returns rect of rectangular where widgets are allowed to be placed in the page.
*/
QRectF HsPage::contentRect()
{
    return contentRect(HsGui::instance()->orientation());
}

/*!
    Returns rect of rectangular where widgets are allowed to be placed in the page.
*/
QRectF HsPage::contentRect(Qt::Orientation orientation)
{
    QRectF rect = contentGeometry(orientation);
    rect.moveTopLeft(QPointF(0,0));
    return rect;
}

HsPageVisual *HsPage::visual() const
{
    return mPageVisual;
}
/*!
    Calls onShow() for contained widgets.
*/
void HsPage::showWidgets()
{
    foreach (HsWidgetHost *widget, mWidgets) {
        if (widget->visual()->parentItem() == visual()) {
            widget->showWidget();
        }
    }
}

/*!
    Calls onHide() for contained widgets.
*/
void HsPage::hideWidgets()
{
    foreach (HsWidgetHost *widget, mWidgets) {
        if (widget->visual()->parentItem() == visual()) {
            widget->hideWidget();
        }
    }
}

/*!
    Propagate online state to widgets.
*/
void HsPage::setOnline(bool online)
{
    foreach (HsWidgetHost *widget, mNewWidgets) {
        widget->setOnline(online);
    }
    foreach (HsWidgetHost *widget, mWidgets) {
        widget->setOnline(online);
    }
}

/*!
    Update widgets z-values and persist those. Active widget has top most 
    z-value.
*/
void HsPage::updateZValues()
{
    int z = 0;

    QHash<int, qreal> widgetZValues;

    if (!mWidgets.isEmpty()) {
        QMultiMap<qreal, HsWidgetHost *> map;
        foreach (HsWidgetHost *widget, mWidgets) {
            map.insert(widget->visual()->zValue(), widget);
        }

        QList<HsWidgetHost *> sortedWidgets = map.values();

        HsWidgetHost *activeWidget = HsScene::instance()->activeWidget();
        if (sortedWidgets.contains(activeWidget)) {
            sortedWidgets.removeOne(activeWidget);
            sortedWidgets.append(activeWidget);
        }

        foreach (HsWidgetHost *widget, sortedWidgets) {
            widgetZValues.insert(widget->databaseId(), z);
            widget->visual()->setZValue(z++);
        }
    }

    if (!mNewWidgets.isEmpty()) {
        foreach (HsWidgetHost *widget, mNewWidgets) {
            widgetZValues.insert(widget->databaseId(), z);
            widget->visual()->setZValue(z++);
        }
    }

    if (!widgetZValues.isEmpty()) {
        HsDatabase::instance()->updateWidgetZValues(
            widgetZValues, HsGui::instance()->orientation());
    }
}
/*!
    Return this page's index.
*/
int HsPage::pageIndex()
{
    return HsScene::instance()->pages().indexOf(this);
}
/*!
    Create touch area for page.
*/
/*void HsPage::setupTouchArea()
{
    mTouchArea = new HsPageTouchArea(this);
    mTouchArea->setZValue(-1);
}*/
/*!
    Utility to connect widget signals to page.
*/
void HsPage::connectWidget(HsWidgetHost *widget)
{
    connect(widget, SIGNAL(finished()), SLOT(onWidgetFinished()));
    connect(widget, SIGNAL(faulted()), SLOT(onWidgetFaulted()));
    connect(widget->visual(), SIGNAL(resized()), SLOT(onWidgetResized()));
    connect(widget, SIGNAL(available()), SLOT(onWidgetAvailable()));
    connect(widget, SIGNAL(unavailable()), SLOT(onWidgetUnavailable()));
}
/*!
    Disconnect widget signals from page
*/
void HsPage::disconnectWidget(HsWidgetHost *widget)
{
    widget->visual()->disconnect(this);
    widget->disconnect(this);
}

/*!
    Disconnect and remove widget
*/
void HsPage::onWidgetFinished()
{
    HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());

    // It can be in new widget list if we haven't layouted it yet
    // or layouted new widget and widget list
    if (!mNewWidgets.removeOne(widget)) {
        mWidgets.removeOne(widget);
    }
    if (widget){     
        HsWidgetHostVisual *visual = widget->visual();
        visual->setParentItem(0);
        if (visual->scene()) {
            visual->scene()->removeItem(visual);
        }
        disconnectWidget(widget);
        widget->remove();
    }
}

/*!
    Remove widget if it faulted
*/
void HsPage::onWidgetFaulted()
{
    onWidgetFinished();
}

/*!
    Calculates new widget position on page when widget size changes. If page has layout then there are new widgets
    and we use layout to calculate new widget positions.
*/
void HsPage::onWidgetResized()
{
    if (!visual()->layout()) {
        HsWidgetHostVisual *widgetVisual = qobject_cast<HsWidgetHostVisual *>(sender());
        widgetVisual->setPos(adjustedWidgetPosition(widgetVisual->geometry()));
    } else {
        visual()->layout()->invalidate();
    }
}
/*!
    Show widget if it came available
*/
void HsPage::onWidgetAvailable()
{
    HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());
    HsWidgetHostVisual *widgetVisual(widget->visual());

    mUnavailableWidgets.removeOne(widget);
    mWidgets.append(widget);

    widgetVisual->setParentItem(visual());
    widget->startWidget(isActivePage());
    widgetVisual->show();
}
/*!
    Update internal bookkeeping and hide widget
*/
void HsPage::onWidgetUnavailable()
{
    HsWidgetHost *widget = qobject_cast<HsWidgetHost *>(sender());
    HsWidgetHostVisual *widgetVisual(widget->visual());

    if (mWidgets.contains(widget)) {
        mWidgets.removeOne(widget);
    } else if (mNewWidgets.contains(widget)) {
        mNewWidgets.removeOne(widget);
    } else {
        return;
    }

    mUnavailableWidgets.append(widget);

    widgetVisual->hide();
    widgetVisual->setParentItem(0);
}

/*!
    Run positioning algorithm for widgets which don't have position on 
    target orientation. Otherwise set orientation positions for widgets.
*/
void HsPage::onOrientationChanged(Qt::Orientation orientation)
{
    Qt::Orientation orientationFrom = orientation == Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
    QRectF from = contentGeometry(orientationFrom);
    QRectF to = contentGeometry(orientation);

    HsWidgetPresentationData presentation;
    presentation.orientation = orientation;
    
    HsPageVisual *pageVisual = visual();
    HsWidgetHostVisual *visual(0);
    QList<HsWidgetHost*> newWidgets; 
    foreach (HsWidgetHost *widget, mWidgets) {
        visual = widget->visual();
        if ( pageVisual->isAncestorOf(visual)) {
            if (!widget->getPresentation(presentation)) {
                newWidgets << widget;
            } else {
                QRectF adjustWidgetPosition;
                adjustWidgetPosition = visual->geometry();
                adjustWidgetPosition.moveTopLeft(presentation.pos());
                visual->setPos(adjustedWidgetPosition(adjustWidgetPosition));
                visual->setZValue(presentation.zValue);
            }
        }
    }
    if (newWidgets.count()) {
        // get rects for new widgets
        QList<QRectF> newRects;
        foreach (HsWidgetHost *newWidget, newWidgets) {
            newRects << QRectF(QPointF(), newWidget->visual()->preferredSize());
        }
        
        // get page rect
        QRectF pageRect = contentGeometry();

        // scan existing widgets rects
        QList<QRectF> existingRects;
        foreach (HsWidgetHost *widget, mWidgets) {
            if (!newWidgets.contains(widget)) {
                existingRects << QRectF(widget->visual()->geometry());
            }
        }
         
        // calculate new widget positions with "stuck 'em all"-algorithm
        HsWidgetPositioningOnWidgetAdd *algorithm =
            HsWidgetPositioningOnWidgetAdd::instance();

        HsWidgetPositioningOnWidgetAdd::Result result;
        result = algorithm->convert(pageRect, existingRects, newRects, QPointF());
        
        QList<HsWidgetPresentationData> widgetPresentationList;

        if (result.rectOrder != QList<int>()) {
            for (int i=0; i<newWidgets.count(); i++) {
                int rectId = result.rectOrder.at(i);
                int widgetId = mWidgets.indexOf(newWidgets.at(rectId));
                mWidgets.at(widgetId)->visual()->setGeometry(result.calculatedRects.at(rectId));
                // update z value
                int zValue = mWidgets.at(widgetId)->visual()->zValue();
                mWidgets.at(widgetId)->visual()->setZValue(zValue - rectId + i);
                HsWidgetPresentationData data;
                data.zValue = zValue - rectId + i;
                data.widgetId = mWidgets.at(widgetId)->databaseId();
                data.setPos(mWidgets.at(widgetId)->visual()->pos());
                widgetPresentationList.append(data);
            }
        }
        if (!widgetPresentationList.isEmpty()) {
            HsDatabase::instance()->updateWidgetPresentations(
                widgetPresentationList, HsGui::instance()->orientation());
        }
        
    }        
}

void HsPage::onPageMarginChanged(const QString &value)
{
    if (value == "pageMargin") {
        mPageMargin = HSCONFIGURATION_GET(pageMargin);
        HsWidgetHostVisual *visual(0);
        if (!mWidgets.isEmpty()) {
            foreach (HsWidgetHost *widget, mWidgets) {
                visual = widget->visual();
                visual->setPos(adjustedWidgetPosition(visual->geometry()));
                widget->savePresentation();
            }
        }

        if (!mNewWidgets.isEmpty()) {
            foreach (HsWidgetHost *widget, mNewWidgets) {
                visual = widget->visual();
                visual->setPos(adjustedWidgetPosition(visual->geometry()));
                widget->savePresentation();
            }
        }
    }
}
