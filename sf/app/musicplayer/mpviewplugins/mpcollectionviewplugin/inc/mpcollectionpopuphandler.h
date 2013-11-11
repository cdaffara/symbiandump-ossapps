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
* Description: Music Player popup handler.
*
*/

#ifndef MPCOLLECTIONPOPUPHANDLER_H
#define MPCOLLECTIONPOPUPHANDLER_H

#include <QObject>
#include <QModelIndex>
#include "mpmpxcollectionviewdefs.h"

class MpCollectionView;
class HbPopup;
class HbAction;
class MpEngine;
class QAbstractItemModel;
class MpMpxCollectionData;
class QPointF;
class MpPopupHandlerPermanentData;

class MpCollectionPopupHandler : public QObject
{
    Q_OBJECT

public:
    MpCollectionPopupHandler( MpCollectionView *parent );
    virtual ~MpCollectionPopupHandler();

    void openDefaultViewContextMenu( const QModelIndex &index, const QPointF &coords );
    void openFetchViewContextMenu( const QModelIndex &index, const QPointF &coords );
    void openRenamePlaylistContainerDialog( const QString &currentName );
    void openRenamePlaylistItemDialog( const QString &currentName );
    void openAddSongsToPlaylist( QAbstractItemModel* model );
    void openAddSongsToPlaylistFromTBone();
    void openDeleteSongs( QAbstractItemModel* model );
    void openAddToCurrentPlaylist( MpMpxCollectionData* collectionData );
    void openCreateNewPlaylist( MpMpxCollectionData* collectionData, bool isolated );
    void openCreateNewPlaylistFromTBone();
    void openArrangeSongs();
    void cancelOngoingPopup(  bool onlyContextMenu );

public slots:
    //Context Menus
    void defaultContextMenuOptionSelected( HbAction *selectedAction );
    void fetcherContextMenuOptionSelected( HbAction *selectedAction );

    //Dialogs
    void handleAddToPlaylistDialogFinished( HbAction *selectedAction );
    void handleAddToPlaylistGetTextFinished( HbAction *selectedAction );
    void handleRenamePlaylistContainer( HbAction *selectedAction );
    void handleRenamePlaylistItem( HbAction *selectedAction );
    void handleAddSongsToPlayList( HbAction *selectedAction );
    void handleDeleteSongs( HbAction *selectedAction );
    void handleAddToCurrentPlaylist( HbAction *selectedAction );
    void handleCreateNewPlaylistGetTextFinished( HbAction *selectedAction );
    void handleCreateNewPlaylistGetModelIndexesFinished( HbAction *selectedAction );
    void handleArrangeSongs( HbAction *selectedAction );
    void handleRequestDelete( HbAction *selectedAction );

    //External Events
    void handleDeleteStarted( TCollectionContext context, int count );
    void handleAddingSongs( int count );
    void handleOperationEnded( bool success );

    void outstandingPopupClosing();
    void inputDialogChanged( QString text );
    void requestDelete();

private:
    void setOutstandingPopup( HbPopup *popup );
    void clearOutstandingPopup( HbPopup *popup );    
    void launchAddToPlaylistDialog( QList<int> &selection );
    void queryNewPlaylistName(const QStringList &playlists, const char *handler );
    void getText(const QString &label, const QString &text, const QString &confirmationActionLabel, const char *handler );
    void getModelIndexes( const QString &label, const QString &confirmationActionLabel, QAbstractItemModel* model, const char *handler );
    void launchArrangeSongsDialog();
    void launchProgressDialog( const char *id );
    void connectExternalEvents();

private:
    MpCollectionView                *mView;                          // Not own
    HbPopup                         *mOutstandingPopup;              // Own
    MpEngine                        *mMpEngine;                      // Not own
    MpPopupHandlerPermanentData     *mPermanentData;                 // Own
    bool                            mExternalEventsConnected;
};

#endif  // MPCOLLECTIONPOPUPHANDLER_H

