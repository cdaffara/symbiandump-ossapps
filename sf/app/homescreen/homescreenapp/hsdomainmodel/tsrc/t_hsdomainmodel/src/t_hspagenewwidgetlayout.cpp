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

#include "t_hsdomainmodel.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hspagenewwidgetlayout.h"
#include "hswidgethost.h"
#include "hsscene.h"
#include "hspage.h"

void TestHsDomainModel::testHsPageNewWidgetLayout()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsPageNewWidgetLayout *layout = new HsPageNewWidgetLayout();
    
    HsPageData pageData;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);

    // add new widget
    HsWidgetData widgetData;
    widgetData.pageId = page->databaseId();
    widgetData.uri = "mockwidgetplugin";
    HsWidgetHost *newWidget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(newWidget);
    page->addNewWidget(newWidget);

    // add existing widget
    HsWidgetData widgetData2;
    widgetData2.pageId = page->databaseId();
    widgetData2.uri = "mockwidgetplugintoosmall";
    HsWidgetHost *existingWidget = HsWidgetHost::createInstance(widgetData2);
    QVERIFY(existingWidget);
    page->addExistingWidget(existingWidget);

    HsScene::instance()->mActivePage = page;
    layout->addItem(newWidget);
    QVERIFY(layout->count() == 1);    
    layout->setGeometry(QRectF());
    layout->removeAt(0);
    QVERIFY(layout->count() == 0);    
    QVERIFY(layout->sizeHint(Qt::NSizeHints) == QSizeF());
    
    delete layout;
    delete newWidget;
    delete existingWidget;
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
}

void TestHsDomainModel::testHsPageNewWidgetLayoutWithPosition()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);
    HsPageNewWidgetLayout *layout = new HsPageNewWidgetLayout();

    HsWidgetHost *widget = new HsWidgetHost(1);
    HsPage *page = new HsPage;
    HsScene::instance()->mActivePage = page;
    layout->addItem(widget);
    layout->mTouchPoint = QPointF(100, 100);
    layout->setGeometry(QRectF());
  
    delete layout;
    delete widget;
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
}
