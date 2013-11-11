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

// System includes
#include <HbListView>
#include <HbAction>
#include <HbAbstractViewItem>
#include <HbMenu>
#include <HbMessageBox>
#include <HbSelectionDialog>

// User includes
#include "radiohistoryview.h"
#include "radiowindow.h"
#include "radiologger.h"
#include "radiouiloader.h"
#include "radiouiengine.h"
#include "radiohistorymodel.h"
#include "radiohistoryitem.h"
#include "radioutil.h"

// BEGIN TEMPORARY TEST CODE CODE
#include <QTimer>
#include "radiostationmodel.h"

struct Song
{
    const char* mArtist;
    const char* mTitle;
};
const Song KRecognizedSongs[] = {
    { "Red Hot Chili Peppers", "Under The Bridge" },
    { "", "No One Knows" },
    { "The Presidents of the United States of America", "Dune Buggy" },
    { "System of a Down", "Aerials" },
    { "The White Stripes", "Seven Nation Army" },
    { "Alice In Chains", "When The Sun Rose Again" },
    { "Bullet For My Valentine", "Tears Don't Fall" }
};
const int KSongsCount = sizeof( KRecognizedSongs ) / sizeof( KRecognizedSongs[0] );
// END TEMPORARY TEST CODE CODE

/*!
 *
 */
RadioHistoryView::RadioHistoryView() :
    RadioViewBase(),
    mHistoryList( 0 ),
    mAllSongsButton( 0 ),
    mTaggedSongsButton( 0 ),
    mSelectedItem( new RadioHistoryItem() ),
    mCurrentRow( -1 ),
    mSongIndex( 0 )
{
}

/*!
 *
 */
RadioHistoryView::~RadioHistoryView()
{
}

/*!
 *
 */
void RadioHistoryView::setNonTaggedIcon( const HbIcon& nonTaggedIcon )
{
    mNonTaggedIcon = nonTaggedIcon;
    mNonTaggedIcon.setColor( Qt::white );
}

/*!
 *
 */
HbIcon RadioHistoryView::nonTaggedIcon() const
{
    return mNonTaggedIcon;
}

/*!
 *
 */
void RadioHistoryView::setTaggedIcon( const HbIcon& taggedIcon )
{
    mTaggedIcon = taggedIcon;
}

/*!
 *
 */
HbIcon RadioHistoryView::taggedIcon() const
{
    return mTaggedIcon;
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::updateViewMode()
{
    QString section = DOCML::HV_SECTION_HISTORY_MODE;
    bool showTagged = false;

    if ( sender() == mTaggedSongsButton ) {
        section = DOCML::HV_SECTION_FAVORITE_MODE;
        showTagged = true;
    }

    loadSection( DOCML::FILE_HISTORYVIEW, section );
    historyModel().setShowTagged( showTagged );

    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::openMultiSelection()
{
    showSelectionDialog( &mUiEngine->historyModel() , 
                         hbTrId( "txt_fmradio_title_delete_song_information" ), 
                         hbTrId( "txt_common_button_delete_toolbar" ) );
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::updateVisibilities()
{
    const int itemCount = mUiEngine->historyModel().rowCount();
    loadSection( DOCML::FILE_HISTORYVIEW, itemCount ? DOCML::HV_SECTION_SHOW_LIST : DOCML::HV_SECTION_HIDE_LIST );
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::showContextMenu( const QModelIndex& index )
{
    *mSelectedItem = historyModel().itemAtIndex( index );
    mCurrentRow = index.row();

    HbMenu* menu = mUiLoader->findObject<HbMenu>( DOCML::HV_NAME_CONTEXT_MENU );

    if ( HbAction* tagAction = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_CONTEXT_TAG ) ) {
        if ( mSelectedItem->isTagged() ) {
            tagAction->setText( hbTrId( "txt_rad_menu_remove_tag" ) );
        } else {
            tagAction->setText( hbTrId( "txt_rad_menu_tag_song" ) );
        }
    }

    if ( HbAction* searchAction = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_CONTEXT_SEARCH ) ) {
        //TODO: Add support for other music store
        searchAction->setVisible( false );
    }

    HbAbstractViewItem* item =  mHistoryList->itemByIndex( index );
    QPointF coords = item->pos();
    coords.setY( mHistoryList->contentWidget()->pos().y() + coords.y() );
    menu->setPreferredPos( QPointF( size().width() / 2 - menu->size().width() / 2, coords.y() + menu->size().height() / 2 ) );

    menu->show();
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::handleLongPress( HbAbstractViewItem* item )
{
    showContextMenu( item->modelIndex() );
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::toggleTagging()
{
    historyModel().toggleTagging( *mSelectedItem, mCurrentRow );
    mSelectedItem->reset();
    mCurrentRow = -1;
    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::openOviStore()
{
    QString msg = "To be implemented: Open ovi store. Artist: %1, Title: %2";
    HbMessageBox::information( msg.arg( mSelectedItem->artist() ).arg( mSelectedItem->title() ) );
    mUiEngine->openMusicStore( *mSelectedItem );
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::openOtherStore()
{
    mUiEngine->openMusicStore( *mSelectedItem, RadioUiEngine::OtherStore );
}

/*!
 * Private slot
 * TEMPORARY TEST CODE
 */
void RadioHistoryView::addSongs()
{
    for ( int i = 0; i < KSongsCount; ++i ) {
        QTimer::singleShot( 1000 + i * 1500, this, SLOT(addOneSong()) );
    }
}

/*!
 * Private slot
 * TEMPORARY TEST CODE
 */
void RadioHistoryView::addOneSong()
{
    Song song = KRecognizedSongs[mSongIndex++];
    mSongIndex %= KSongsCount;

    RadioStation station = mUiEngine->stationModel().currentStation();
    mUiEngine->historyModel().addItem( song.mArtist, song.mTitle, station );
}

/*!
 * \reimp
 *
 */
void RadioHistoryView::init()
{
    LOG_METHOD;
    loadSection( DOCML::FILE_HISTORYVIEW, DOCML::SECTION_LAZY_LOAD );

    RadioHistoryModel* historyModel = &mUiEngine->historyModel();
    historyModel->setShowDetails( mOrientation == Qt::Horizontal );

    if ( !mNonTaggedIcon.isNull() && !mTaggedIcon.isNull() ) {
        historyModel->setIcons( mNonTaggedIcon.qicon(), mTaggedIcon.qicon() );
    }

    mHistoryList = mUiLoader->findObject<HbListView>( DOCML::HV_NAME_HISTORY_LIST );
    mHistoryList->setScrollingStyle( HbListView::PanOrFlick );
    mHistoryList->setModel( historyModel );
    mHistoryList->setSelectionMode( HbListView::NoSelection );
    mHistoryList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    mHistoryList->setItemPixmapCacheEnabled( true ); // Improves scrolling FPS rate

    mAllSongsButton = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_ALL_SONGS_BUTTON );
    mTaggedSongsButton = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_TAGGED_SONGS_BUTTON );

    if ( HbAction* multiDelAction = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_MULTI_DELETE_ACTION ) ) {
            Radio::connect( multiDelAction,    SIGNAL(triggered()),
                            this,              SLOT(openMultiSelection()) );
    }

    Radio::connect( mTaggedSongsButton,     SIGNAL(triggered()),
                    this,                   SLOT(updateViewMode()) );
    Radio::connect( mAllSongsButton,        SIGNAL(triggered()),
                    this,                   SLOT(updateViewMode()) );
    Radio::connect( historyModel,           SIGNAL(itemAdded()),
                    this,                   SLOT(updateVisibilities()) );
    Radio::connect( mHistoryList,           SIGNAL(activated(QModelIndex)),
                    this,                   SLOT(showContextMenu(QModelIndex)) );
    Radio::connect( mHistoryList,           SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
                    this,                   SLOT(handleLongPress(HbAbstractViewItem*)) );

    connectCommonMenuItem( MenuItem::Exit );
    connectCommonMenuItem( MenuItem::UseLoudspeaker );

    initContextMenu();

    initBackAction();

    updateViewMode();

    // BEGIN TEMPORARY TEST CODE
    if ( HbAction* addSongsAction = mUiLoader->findObject<HbAction>( "hv:add_songs_action" ) ) {
        Radio::connect( addSongsAction,     SIGNAL(triggered()),
                        this,               SLOT(addSongs()) );
    }
    // END TEMPORARY TEST CODE
}

/*!
 * \reimp
 *
 */
void RadioHistoryView::setOrientation()
{
    if ( mUiEngine ) {
        RadioHistoryModel& model = mUiEngine->historyModel();
        model.setShowDetails( mOrientation == Qt::Horizontal );
    }
}

/*!
 * \reimp
 *
 */
void RadioHistoryView::userAccepted()
{
    HbSelectionDialog* dlg = static_cast<HbSelectionDialog*>( sender() );
    if(dlg) {
        bool favoriteMode = mTaggedSongsButton->isChecked();
        QModelIndexList selected = dlg->selectedModelIndexes();
        int count = selected.count();
        if( count ==  mUiEngine->historyModel().rowCount() ) {
            mUiEngine->historyModel().removeAll( favoriteMode );
        } else {
            mUiEngine->historyModel().removeByModelIndices( selected, favoriteMode );            
        }
        QString msg = hbTrId("txt_rad_dpophead_l1_song_information_removed", count );
        RadioUtil::showDiscreetNote( msg );
        updateVisibilities();
    }
}

/*!
 *
 */
void RadioHistoryView::initContextMenu()
{
    // Context menu actions
    connectXmlElement( DOCML::HV_NAME_TOGGLE_TAG_ACTION,    SIGNAL(triggered()),
                       this,                                SLOT(toggleTagging()) );
    connectXmlElement( DOCML::HV_NAME_OVI_STORE_ACTION,     SIGNAL(triggered()),
                       this,                                SLOT(openOviStore()) );

    // TODO: Get additional music stores. For now use "Amazon" for demonstration purposes
    const QString otherStoreFormatter = hbTrId( "txt_rad_menu_search_from_other_store" );
    if ( HbMenu* contextMenu = mUiLoader->findObject<HbMenu>( DOCML::HV_NAME_CONTEXT_MENU ) ) {
        contextMenu->addAction( otherStoreFormatter.arg( "Amazon" ), this, SLOT(openOtherStore()) );
    }
}

/*!
 *
 */
RadioHistoryModel& RadioHistoryView::historyModel() const
{
    return *static_cast<RadioHistoryModel*>( mHistoryList->model() );
}
