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

#include "fbdrivelistviewitem.h"
#include "fbdriveentry.h"
#include "fbdrivemodel.h"

#include <hblabel.h>
#include <hbcheckbox.h>
#include <hbabstractitemview.h>

#include <QString>
#include <QStringList>
#include <QFileIconProvider>
#include <QGraphicsLinearLayout>
#include <QRectF>
#include <QPainter>

FbDriveListViewItem::FbDriveListViewItem(QGraphicsItem *parent) :
    HbListViewItem(parent),
    hLayout(0),
    mDiskNameLabel(0),
    mSizeLabel(0),
    mFreeLabel(0),
    mIconLabel(0)
{
}

FbDriveListViewItem::~FbDriveListViewItem()
{
}

void FbDriveListViewItem::polish(HbStyleParameters& params)
{
    Q_UNUSED(params);
}

bool FbDriveListViewItem::canSetModelIndex(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return true;
}


HbAbstractViewItem *FbDriveListViewItem::createItem()
{
    return new FbDriveListViewItem(*this);
}

void FbDriveListViewItem::updateChildItems()
{
    if(!hLayout) {
        init();
    }

    // Qt::DisplayRole
//    QVariant displayRole = modelIndex().data(Qt::DisplayRole);
//    QString diskName("default");
    QStringList stringList;
//    if (displayRole.isValid()) {
//        if (displayRole.canConvert<QString>()) {                // EFileViewModeSimple
//            stringList.append(displayRole.toString());
//        } else if (displayRole.canConvert<QStringList>()) {     // EFileViewModeExtended
//            stringList = displayRole.toStringList();
//        }
//        if (stringList.count() > 0)
//            diskName = stringList[0]; //modelIndex().data( Qt::UserRole ).toString();
//
//        mDiskNameLabel->setPlainText(diskName/*displayString*/);
//    }
//    if (stringList.count() > 1)
//        mSizeLabel->setPlainText( stringList[1] );


    // Get the Drive Entry
    const FbDriveModel* driveModel= qobject_cast<const FbDriveModel *>(modelIndex().model());
    FbDriveEntry driveEntry = driveModel->driveEntry(modelIndex());

    const QString SimpleDriveEntry("%1: <%2>");

    QString diskName = SimpleDriveEntry.arg(driveEntry.driveLetter()).arg(driveEntry.mediaTypeString());
    mDiskNameLabel->setPlainText(diskName);

    const QString ExtendedDriveEntry("%1/%2 kB");
    QString diskSize = ExtendedDriveEntry.arg(QString::number(driveEntry.volumeInfoFree()/1024))
                                         .arg(QString::number(driveEntry.volumeInfoSize()/1024));
    mSizeLabel->setPlainText( diskSize );

    //mFreeLabel->setPlainText( stringList[2] );

    //    mCheckBox->setCheckState( checkState() );

    // Qt::DecorationRole
    // QTBUG-11033 No Icon provided for QFileIconProvider::Desktop, Network, Drive on Symbian
    QVariant decorationRole = modelIndex().data(Qt::DecorationRole);
    if (decorationRole.isValid()) {
        QIcon icon = qvariant_cast<QIcon>(decorationRole);
    if( icon.isNull() ) {
        QFileIconProvider fileIconProvider;
        icon = fileIconProvider.icon(QFileIconProvider::File);
    }
    mIconLabel->setIcon(HbIcon(icon));
    }
}

void FbDriveListViewItem::setCheckedState(int state)
{
    HbAbstractViewItem::setCheckState(static_cast<Qt::CheckState>(state));
}

void FbDriveListViewItem::init()
{
    hLayout = new QGraphicsLinearLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);

    hLayout->setOrientation( Qt::Horizontal );
    hLayout->addItem(layout());

    mIconLabel = new HbLabel();
    hLayout->addItem( mIconLabel );
    hLayout->setAlignment( mIconLabel, Qt::AlignTop );
    hLayout->setStretchFactor( mIconLabel, 0 );

    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    //vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setOrientation( Qt::Vertical );

    mDiskNameLabel = new HbLabel();
    HbFontSpec fontSpecPrimary(HbFontSpec::Primary);
    //fontSpecPrimary.setTextHeight(18.0);
    mDiskNameLabel->setFontSpec( fontSpecPrimary );
    vLayout->addItem( mDiskNameLabel );
    vLayout->setAlignment( mDiskNameLabel, Qt::AlignLeft );

    mSizeLabel = new HbLabel();
    HbFontSpec fontSpecSecondary(HbFontSpec::Secondary);
    //fontSpecSecondary.setTextHeight(18.0);
    mSizeLabel->setFontSpec(fontSpecSecondary);
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

    //this->setMaximumHeight( mDiskNameLabel->preferredHeight() );
    //setMaximumHeight( mIconLabel->preferredHeight() );

    setLayout( hLayout );
}
