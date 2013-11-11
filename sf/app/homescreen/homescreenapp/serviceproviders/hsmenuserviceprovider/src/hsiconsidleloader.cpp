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
 * Description: Menu All Applications state.
 *
 */

#include <QPainter>
#include <QSize>
#include <QTimer>
#include "hsmenuitemmodel.h"
#include "hsiconsidleloader.h"

/*!
 Constructor
 \param model with defined icons
 \param pointer to parent object
 */
HsIconsIdleLoader::HsIconsIdleLoader(HsMenuItemModel *model, QObject *parent):
    QObject(parent),
    mModel(model),
    mTimer(NULL),
    mIconSize(mModel->getIconSize())
{
    mTimer = new QTimer(this);
    mOutStandingIconToLoad = mModel->rowCount();
    connect(mTimer, SIGNAL(timeout()), this, SLOT(processWhenIdle()));
    mTimer->start(0); // NOTE: zero for idle
}

/*!
 Destructor
 */
HsIconsIdleLoader::~HsIconsIdleLoader()
{
    mTimer->stop();
}

/*!
 Preloading icons if idle
 \retval void
 */
void HsIconsIdleLoader::processWhenIdle()
{
    if (mOutStandingIconToLoad >= 1 
        && mModel->rowCount() >= mOutStandingIconToLoad ) {
    	QPixmap pixmap(mIconSize.toSize());
    	pixmap.fill(Qt::transparent);

        QModelIndex idx = mModel->index(mOutStandingIconToLoad -1);
        HbIcon icon = mModel->entry(idx)->makeIcon(mIconSize);
        icon.setSize(mIconSize);
        // creating raster data
        QPainter painter;
        painter.begin(&pixmap);
        icon.paint(&painter, QRectF(QPointF(), mIconSize), 
                     Qt::KeepAspectRatio, 
                     Qt::AlignCenter, 
                     QIcon::Normal, 
                     QIcon::Off);
        painter.end();
        --mOutStandingIconToLoad;
    } else {
    	mTimer->stop(); // No more timing
	}
}

