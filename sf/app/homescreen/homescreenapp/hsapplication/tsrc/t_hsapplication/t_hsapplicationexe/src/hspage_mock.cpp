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

#include <HbInstance>

#include "hsdomainmodeldatastructures.h"
#include "hsgui.h"
#include "hspage.h"
#include "hspagevisual.h"
#include "hspagetoucharea.h"
#include "hspagenewwidgetlayout.h"
#include "hsscene.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswallpaper.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hsconfiguration.h"


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

/*!
    Loads widgets.
*/
bool HsPage::load()
{
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        mWallpaper = new HsPageWallpaper(this);
    }

    HsWidgetHost *widget = new HsWidgetHost(-1);
    mWidgets.append(widget);
    connectWidget(widget);
    widget->setPage(this);
    widget->visual()->setParentItem(this->visual());
    widget->startWidget();
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
    widgetHost->visual()->setParentItem(this->visual());

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
    Q_UNUSED(touchPoint)
    if (!widgetHost || mWidgets.contains(widgetHost)) {
        return false;
    }

    if (mNewWidgets.contains(widgetHost)) {
        return true;
    }

    HsWidgetPresentationData presentation;
    presentation.orientation = Qt::Vertical;
    if (!widgetHost->getPresentation(presentation)) {
        presentation.orientation = Qt::Vertical;
        presentation.setPos(QPointF());
        presentation.zValue = 0;
        widgetHost->savePresentation(presentation);
    }

    widgetHost->visual()->hide();
    widgetHost->visual()->setPos(presentation.x, presentation.y);
    widgetHost->visual()->setZValue(presentation.zValue);

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

    updateZValues();
    HsWidgetHost *widget = 0;
    for (int i = 0; i < mNewWidgets.count(); ++i) {
        widget = mNewWidgets.at(i);
//Not used in mock        newWidgetLayout->addItem(widget);
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
}

/*!
    Remove page and all it's contained widgets from database
*/
bool HsPage::deleteFromDatabase()
{
	//Not used in mock
    return true;
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
    Q_UNUSED(pageData);
    HsPage *page = new HsPage;
    return page;
}

/*!
    The widget is bounded in the rectangle which is smaller by PageMargin on all sides of page.
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
    return contentGeometry(Qt::Vertical);
}

/*!
    Returns rect of rectangular where widgets are allowed to be placed in the page.
*/
QRectF HsPage::contentGeometry(Qt::Orientation orientation)
{
    QRectF pageRect = QRectF(0, 0, 360.0, 640.0);

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
    return contentRect(Qt::Vertical);
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

/*!
    Calls onShow() for contained widgets.
*/
void HsPage::showWidgets()
{
}

/*!
    Calls onHide() for contained widgets.
*/
void HsPage::hideWidgets()
{
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
}

/*!
    Return this page's index.
*/
int HsPage::pageIndex()
{
    return HsScene::instance()->pages().indexOf(this);
}
/*!
    Utility to connect widget signals to page.
*/
void HsPage::connectWidget(HsWidgetHost *widget)
{
    connect(widget, SIGNAL(finished()), SLOT(onWidgetFinished()));
    connect(widget, SIGNAL(faulted()), SLOT(onWidgetFaulted()));
    connect(widget, SIGNAL(resized()), SLOT(onWidgetResized()));
    connect(widget, SIGNAL(available()), SLOT(onWidgetAvailable()));
    connect(widget, SIGNAL(unavailable()), SLOT(onWidgetUnavailable()));
}
/*!
    Disconnect widget signals from page
*/
void HsPage::disconnectWidget(HsWidgetHost *widget)
{
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

    disconnectWidget(widget);
    widget->remove();
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
}
/*!
    Show widget if it came available
*/
void HsPage::onWidgetAvailable()
{
}
/*!
    Update internal bookkeeping and hide widget
*/
void HsPage::onWidgetUnavailable()
{
}

/*!
    Run positioning algorithm for widgets which don't have position on
    target orientation. Otherwise set orientation positions for widgets.
*/
void HsPage::onOrientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
}

void HsPage::onPageMarginChanged(const QString &value)
{
    Q_UNUSED(value)
}

HsPageVisual *HsPage::visual() const
{
    return mPageVisual;
}
