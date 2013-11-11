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


#ifndef HSIDLEWIDGET_H
#define HSIDLEWIDGET_H

#include <HbWidget>

#include "hstest_global.h"
#include "hsdomainmodel_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HsPage;
class HsTrashBinWidget;
class HsPageIndicator;
class HsSnapLine;

class HSDOMAINMODEL_EXPORT HsIdleWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal sceneX READ sceneX WRITE setSceneX)

public:
    HsIdleWidget(QGraphicsItem *parent = 0);
		~HsIdleWidget();

    qreal sceneX() const;
    void setSceneX(qreal x);

    void setGeometry(const QRectF &rect);

    void setActivePage(int index);
    void insertPage(int index, HsPage *page);
    void removePage(int index);

    HbWidget *controlLayer() const { return mControlLayer; }
    HbWidget *pageLayer() const { return mPageLayer; }
    HbWidget *pageWallpaperLayer() const { return mPageWallpaperLayer; }
    HbWidget *sceneLayer() const { return mSceneLayer; }

    HsTrashBinWidget *trashBin() const { return mTrashBin; }
    HsPageIndicator *pageIndicator() const { return mPageIndicator; }

    qreal parallaxFactor() const;


public slots:
    void showTrashBin();
    void showPageIndicator();

    void showVerticalSnapLine(const QLineF &snapLine);	
    void hideVerticalSnapLine();
    void showHorizontalSnapLine(const QLineF &snapLine);
    void hideHorizontalSnapLine();

protected:
    void polishEvent();

private:
    Q_DISABLE_COPY(HsIdleWidget)
    void loadControlLayer();
        
private:
    HbWidget *mControlLayer;
    HbWidget *mPageLayer;
    HbWidget *mPageWallpaperLayer;
    HbWidget *mSceneLayer;

    HsTrashBinWidget *mTrashBin;
    HsPageIndicator *mPageIndicator;

    HsSnapLine *mHorizontalSnapLine;
    HsSnapLine *mVerticalSnapLine;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSIDLEWIDGET_H
