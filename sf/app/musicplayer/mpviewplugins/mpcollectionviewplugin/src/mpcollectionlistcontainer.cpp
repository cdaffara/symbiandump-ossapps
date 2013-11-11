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
* Description: Music Player collection view base list container definition.
*
*/

// INCLUDE FILES
#include <QtCore>
#include <QGraphicsGridLayout>

#include <hblistview.h>
#include <hbabstractviewitem.h>
#include <hblistviewitem.h>
#include <hbscrollbar.h>
#include <hbdocumentloader.h>
#include <hblabel.h>
#include <hbindexfeedback.h>

#include "mpcollectionlistcontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionListContainer
    \brief Music Player collection view base list container definition.

    Collection container is a base container that provides interface to
    be implemented by the specific collection context containers, e.g.
    All Songs, Artists, etc. Collection container owns UI elements such
    as the layout and the widgets.
*/

/*!
 Constructs the collection container.
 */
MpCollectionListContainer::MpCollectionListContainer( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionContainer(loader, parent),
      mList(0),
      mNoMusic(0),
      mIndexFeedback(0),
      mLongPressedIndex(0),
      mLongPressEnabled(true)
{
    TX_ENTRY
    mIndexFeedback = new HbIndexFeedback();
    mIndexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    TX_EXIT
}

/*!
 Destructs the collection container.
 */
MpCollectionListContainer::~MpCollectionListContainer()
{
    TX_ENTRY
    delete mNoMusic;
    delete mIndexFeedback;
    TX_EXIT
}

/*!
 Initializes the collection container. Allocates all resources needed by the
 container.
 */
void MpCollectionListContainer::initialize()
{
    TX_LOG
}

/*!
 Sets the data model for the container.
 */
void MpCollectionListContainer::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    MpCollectionContainer::setDataModel(dataModel);
    if ( mList ) {
        mList->setModel(dataModel);
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
 */
void MpCollectionListContainer::itemActivated( const QModelIndex &index )
{
    TX_ENTRY_ARGS("index=" << index.row());
    emit MpCollectionContainer::itemActivated( index.row() );
    TX_EXIT
}

/*!
 Slot to be called when an item is long pressed by the user.
 Remember the index so that we can scroll to this vicinity if the data is reloaded, for example,
 after a delete operation is selected from the context menu.
 */
void MpCollectionListContainer::onLongPressed( HbAbstractViewItem *listViewItem, const QPointF &coords )
{
    TX_ENTRY
    if ( mLongPressEnabled ) {
        mLongPressedIndex = listViewItem->modelIndex().row();
        emit MpCollectionContainer::itemLongPressed(listViewItem->modelIndex(), coords);
    }
    TX_EXIT
}

/*!
 Slot to be called data model has new data. 
 If the data is reloaded as a result of operation like delete, we want to scroll to the 
 nearby area where user has performed the action.
 
 \sa onLongPressed
 */
void MpCollectionListContainer::dataReloaded()
{
    TX_ENTRY
    if ( mLongPressedIndex > 0 ) {
        --mLongPressedIndex;
    }
    if ( mList ) {
        mList->scrollTo( mDataModel->index(mLongPressedIndex, 0) );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpCollectionListContainer::initializeList()
{
    TX_ENTRY
    mList->setItemRecycling(true);
    mList->setLongPressEnabled(true);

    connect(mList, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
    connect(mList, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)), this, SLOT(onLongPressed(HbAbstractViewItem*, QPointF)));

    HbScrollBar *scrollbar = mList->verticalScrollBar();
    scrollbar->show();
    scrollbar->setInteractive(true);
    mList->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);

    mList->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);
    TX_EXIT
}

/*!
 \internal
 */
void MpCollectionListContainer::setupEmptyListContainer()
{
    TX_ENTRY
    bool ok = false;
    mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "emptyList", &ok);
    if ( !ok ) {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(ok, "MpCollectionListContainer::setupContainer", "invalid xml file");
    }

    QGraphicsWidget *widget;
    widget = mDocumentLoader->findWidget(QString("noMusic"));
    mNoMusic = qobject_cast<HbLabel*>(widget);
    TX_EXIT
}

