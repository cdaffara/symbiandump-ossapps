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
* Description:  Homescreen runtime content service.
*
*/

#include "hscontentservice.h"
#include "hsdatabase.h"
#include "hsscene.h"
#include "hspage.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgethost.h"
#include "hsapp_defs.h"


/*!
    \class HsContentService
    \ingroup group_hsdomainmodel
    \brief Service for creating widget to Home Screen and make query for widget instances. 
    
*/

/*!
    Constructor.
    
    \a parent Owner.
*/
HsContentService::HsContentService(QObject *parent)
    : QObject(parent), mWidgetStartFaulted(false)
{
   
}

/*!
    Destructor.
*/
HsContentService::~HsContentService()
{
}

/*!
    Creates widget. \a params must include 'uri' for the desired widget type.
    'preferences' is optional.
*/
bool HsContentService::createWidget(const QVariantHash &params)
{
    return addWidget(params.value(Hs::uri).toString(),
                     params.value(Hs::preferences).toHash(),
                     params.value(Hs::homescreenData));
}

// This method will be removed.
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
HsWidgetHost *HsContentService::createWidgetForPreview(const QVariantHash &params)
{
    HsWidgetData widgetData;
    widgetData.uri = params.value(Hs::uri).toString();

    return HsWidgetHost::createInstance(
        widgetData, params.value(Hs::preferences).toHash());
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!

*/
bool HsContentService::addWidget(const QString &uri, const QVariantHash &preferences,
                                 const QVariant &homescreenData)
{
    HsWidgetData data;
    data.uri = uri;

    HsWidgetHost *widget = HsWidgetHost::createInstance(data, preferences);    
    if (!widget) {
        return false;
    }

    HsPage *page = HsScene::instance()->activePage();
    QPointF touchPoint = homescreenData.toPointF();
    if (!page->addNewWidget(widget, touchPoint)) {
        widget->remove();
        return false;
    }
    connect(widget,SIGNAL(event_faulted()),SLOT(widgetStartFaulted()));
    mWidgetStartFaulted = false; 
    widget->startWidget(false); // synchronous operation
    if (mWidgetStartFaulted) {
        // page will destroy widget instance
        return false;
    }
    widget->disconnect(this);
    emit widgetAdded(uri, preferences);
    return true;
}

/*!
    Returns false if database query fails. If returns true then 
    number of widget instances of the given \a uri and \a preferences is stored to \a count.
    If \a preferences is empty then returns number of widget instances with given uri.
*/
bool HsContentService::widgets(const QString &uri, const QVariantHash &preferences, int &count)
{
    return HsDatabase::instance()->widgets(uri, preferences, count);
}

/*!
*/
HsContentService *HsContentService::instance()
{
    if (!mInstance) {
        mInstance = new HsContentService();
    }
    return mInstance;
}

/*!

*/
void HsContentService::emitWidgetRemoved(const QString &uri, const QVariantHash &preferences)
{
    emit widgetRemoved(uri, preferences);
}

/*!

*/
void HsContentService::widgetStartFaulted()
{
    mWidgetStartFaulted = true;
}

/*!
    Points to the content service instance.
*/
HsContentService *HsContentService::mInstance(0);

/*!
    \fn HsContentService::widgetAdded(const QString &uri, const QVariantHash &preferences);
    
    Emited when widget is added.

*/
/*!
    \fn HsContentService::widgetRemoved(const QString &uri, const QVariantHash &preferences);
    
    Emited when widget is removed.

*/

