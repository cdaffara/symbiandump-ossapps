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

#ifndef RADIOHISTORYMODEL_H
#define RADIOHISTORYMODEL_H

// System includes
#include <QAbstractListModel>
#include <QScopedPointer>

// User includes
#include "radiouiengineexport.h"
#include "radiohistoryitem.h"

// Forward declarations
class RadioHistoryModelPrivate;
class RadioUiEngine;
class RadioStation;
class QIcon;
class RadioHistoryItem;

class UI_ENGINE_DLL_EXPORT RadioHistoryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioHistoryModel )
    Q_DISABLE_COPY( RadioHistoryModel )

    friend class RadioUiEngine;
    friend class RadioUiEnginePrivate;
    friend class RadioStationModelPrivate;

    friend class RadioHistoryView;  // TEMPORARY TEST CODE, REMOVE
    friend class TestRadioUiEngine;
public:

    ~RadioHistoryModel();

    enum HistoryRole
    {
        HistoryItemRole = Qt::UserRole + 100,
        SetFavoriteRole
    };

// from base class QAbstractListModel

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

// New functions

    /*!
     * Sets the icons to be used in the list
     */
    void setIcons( const QIcon& nonTaggedIcon, const QIcon& taggedIcon );

    bool isCurrentSongRecognized() const;
    void setShowDetails( bool showDetails );
    void setShowTagged( bool showTagged );
    void toggleTagging( const RadioHistoryItem& item, const int row );

    RadioHistoryItem itemAtIndex( const QModelIndex& index ) const;

    void removeAll( bool removeTagged );
    void removeByModelIndices( QModelIndexList& indices, bool removeTags );

signals:

    void itemAdded();
    void currentSongReset();

public slots:

    void resetCurrentSong();

private:

    explicit RadioHistoryModel( RadioUiEngine& uiEngine );

    void addItem( const QString& artist, const QString& title, const RadioStation& station );

    void clearRadioTextPlus();
    void addRadioTextPlus( int rtClass, const QString& rtItem, const RadioStation& station );

    void reportChangedData( int start, int end = -1 );

    void emitItemAdded();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    const QScopedPointer<RadioHistoryModelPrivate> d_ptr;

};

#endif // RADIOHISTORYMODEL_H
