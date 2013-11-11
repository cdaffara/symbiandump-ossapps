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

#include "hspagevisual.h"
#include "hspagetoucharea.h"
#include "hsscene.h"
#include "hspage.h"


/*!
    \class HsPageVisual
    \ingroup group_hsdomainmodel
    \brief Represents a page in the framework.
    HsPageVisual contains group of widgets. HsPageVisual can have a wallpaper.
*/

/*!
    Constructor.

    \a parent Owner.
    \a aFlags Window flags.
*/
HsPageVisual::HsPageVisual(QGraphicsItem* parent)
    : HbWidget(parent),
      mTouchArea(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents);
    setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
                              QSizePolicy::Ignored));

    setupTouchArea();
}

/*!
    Destructor.
*/
HsPageVisual::~HsPageVisual()
{
}

int HsPageVisual::pageIndex() const
{
    QList<HsPage *> pages = HsScene::instance()->pages();
    for (int i = 0; i < pages.count(); ++i) {
        if (pages.at(i)->visual() == this) {
            return i;
        }
    }
    return -1;
}

void HsPageVisual::setGeometry(const QRectF &rect)
{
    if (mTouchArea) {
        mTouchArea->resize(rect.size());
    }

    HbWidget::setGeometry(rect);
}

/*!
    Create touch area for page.
*/
void HsPageVisual::setupTouchArea()
{
    mTouchArea = new HsPageTouchArea(this);
    mTouchArea->setZValue(-1);
}



