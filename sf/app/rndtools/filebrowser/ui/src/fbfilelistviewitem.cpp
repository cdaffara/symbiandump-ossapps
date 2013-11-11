/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "fbfilelistviewitem.h"

#include <QString>
#include <QStringList>
#include <QGraphicsLinearLayout>
#include <QRectF>
#include <QPainter>

#include <hblabel.h>
#include <hbcheckbox.h>
#include <hbabstractitemview.h>

FbFileListViewItem::FbFileListViewItem(QGraphicsItem *parent) :
    HbListViewItem(parent),
    hLayout(0),
    mDiskNameLabel(0),
    mSizeLabel(0),
    mFreeLabel(0),
    mIconLabel(0)
{
}

FbFileListViewItem::~FbFileListViewItem()
{
}

void FbFileListViewItem::polish(HbStyleParameters &params)
{
    Q_UNUSED(params);
}

bool FbFileListViewItem::canSetModelIndex(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return true;
}


HbAbstractViewItem *FbFileListViewItem::createItem()
{
    return new FbFileListViewItem(*this);
}

void FbFileListViewItem::updateChildItems()
{
    if(!hLayout) {
        init();
    }

    // Qt::DisplayRole
    QVariant displayRole = modelIndex().data(Qt::DisplayRole);
    QString diskName("default");
    QStringList stringList;
    if (displayRole.isValid()) {
        if (displayRole.canConvert<QString>()) {                // EFileViewModeSimple
            stringList.append(displayRole.toString());
        } else if (displayRole.canConvert<QStringList>()) {     // EFileViewModeExtended
            stringList = displayRole.toStringList();
        }
        if (stringList.count() > 0)
            diskName = stringList[0]; //modelIndex().data( Qt::UserRole ).toString();

        mDiskNameLabel->setPlainText(diskName/*displayString*/);
    }

//    mSizeLabel->setPlainText( hbTrId ( "Size: " ) /*+ FmUtils::formatStorageSize( driverInfo.size() )*/ );
//    mFreeLabel->setPlainText( hbTrId ( "Free: " ) /*+ FmUtils::formatStorageSize( driverInfo.freeSize() )*/ );
    if (stringList.count() > 1)
        mSizeLabel->setPlainText( stringList[1] );

    //mStorageSizeDiagram->setFreeSizePercentage();
    //mFreeLabel->setPlainText( stringList[2] );

    //    mCheckBox->setCheckState( checkState() );

    // Qt::DecorationRole
    QVariant decorationRole = modelIndex().data(Qt::DecorationRole);
    if (decorationRole.isValid()) {
        QIcon icon = qvariant_cast<QIcon>(decorationRole);
//    if( icon.isNull() ) {
//        QFileIconProvider fileIconProvider;
//        icon = fileIconProvider.icon(QFileIconProvider::Drive);
//    }
    mIconLabel->setIcon(HbIcon(icon));
    }
}

void FbFileListViewItem::setCheckedState(int state)
{
    HbAbstractViewItem::setCheckState(static_cast<Qt::CheckState>(state));
}

void FbFileListViewItem::init()
{
    hLayout = new QGraphicsLinearLayout();

    hLayout->setOrientation( Qt::Horizontal );
    hLayout->addItem(layout());

    mIconLabel = new HbLabel();
    hLayout->addItem( mIconLabel );
    hLayout->setAlignment( mIconLabel, Qt::AlignTop );
    hLayout->setStretchFactor( mIconLabel, 0 );

//    mStorageSizeDiagram = new FbDriveStorageSizeDiagram(this);
//    hLayout->addItem( mStorageSizeDiagram );
//    hLayout->setAlignment( mIconLabel, Qt::AlignTop );
//    hLayout->setStretchFactor( mStorageSizeDiagram, 0 );

    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation( Qt::Vertical );

    mDiskNameLabel = new HbLabel();
    //HbFontSpec fontSpecPrimary(HbFontSpec::Primary);
    //fontSpecPrimary.setTextHeight(18.0);
    //mDiskNameLabel->setFontSpec( fontSpecPrimary );
    vLayout->addItem( mDiskNameLabel );
    vLayout->setAlignment( mDiskNameLabel, Qt::AlignLeft );

    mSizeLabel = new HbLabel();
    //HbFontSpec fontSpecSecondary(HbFontSpec::Secondary);
    //fontSpecSecondary.setTextHeight(18.0);
    //mSizeLabel->setFontSpec(fontSpecSecondary);
    vLayout->addItem( mSizeLabel );
    vLayout->setAlignment(mSizeLabel, Qt::AlignLeft);

//    mFreeLabel = new HbLabel();
//    mFreeLabel->setFontSpec( HbFontSpec( HbFontSpec::Secondary ) );
//    vLayout->addItem( mFreeLabel );
//    vLayout->setAlignment( mFreeLabel, Qt::AlignLeft );

    HbWidget *labelsWidget = new HbWidget();
    labelsWidget->setLayout(vLayout);

    hLayout->setAlignment( labelsWidget, Qt::AlignLeft );

    hLayout->addItem( labelsWidget );
    hLayout->setStretchFactor( labelsWidget, 1 );

//    this->setMaximumHeight(80);

    setLayout( hLayout );
}
