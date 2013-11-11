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

#include <QStandardItemModel>
#include <QGraphicsLinearLayout>

#include <HbGridView>
#include <HbAction>
#include <HbInstance>

#include "hsimagegridwidget.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

namespace
{
    const int FILENAMEROLE = Qt::UserRole + 1;
}

/*!
    \class HsImageGridWidget
    \ingroup group_hsutils
    \brief Widget which shows images in a grid. 
 */

/*!
    Constructor.
    \a imageFiles List of absolut paths to image files.
    \a parent Parent item.
*/
HsImageGridWidget::HsImageGridWidget(const QStringList &imageFiles,
                                     QGraphicsItem *parent)
    : HbView(parent),
      mGridModel(new QStandardItemModel(this))
{
    mGridView = new HbGridView();
    mGridView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAutoHide);
    mGridView->setSelectionMode(HbAbstractItemView::NoSelection);
    mGridView->setModel(mGridModel);

    connect(mGridView,
            SIGNAL(activated(QModelIndex)),
            SLOT(gridItemActivated(QModelIndex)));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->addItem(mGridView);
    setLayout(layout);

    setContent(imageFiles);
}

/*!
    Destructor
*/
HsImageGridWidget::~HsImageGridWidget()
{
}

/*!
    Sets content image files to widget.
    \a images List of absolut paths to image files.
*/
void HsImageGridWidget::setContent(const QStringList &images)
{
    mGridModel->clear();
    QSize targetSize(150, 150);
    QList<QStandardItem *> items;
    QStandardItem *standardItem = 0;
    for (int i = 0; i < images.count(); ++i) {
        QPixmap original(images[i]);
        QIcon scaledIcon(original.scaled(targetSize, Qt::KeepAspectRatio));
        standardItem = new QStandardItem();
        standardItem->setData(HbIcon(scaledIcon), Qt::DecorationRole);
        standardItem->setData(images[i], FILENAMEROLE);
        items << standardItem;
    }
    mGridModel->appendColumn(items);
}

/*!
    \fn void HsImageGridWidget::imageSelected(const QString& aSelectedImage)
    Emitted when image is selected from a grid
    \a aImageFiles List of absolut paths to image files.
*/

/*!
    Finds item from model and emits imageSelected
    \a index is the index of selected image
*/
void HsImageGridWidget::gridItemActivated(const QModelIndex &index)
{
    QVariant data = mGridModel->data(index, FILENAMEROLE);
    emit imageSelected( data.toString() );
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
