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

#ifndef RADIOSTATIONSVIEW_H
#define RADIOSTATIONSVIEW_H

// System includes
#include <HbIcon>
#include <QScopedPointer>

// User includes
#include "radioviewbase.h"

// Forward declarations
class RadioWindow;
class RadioStationModel;
class HbListView;
class HbPushButton;
class HbAbstractViewItem;
class RadioUiLoader;
class RadioStation;
class RadioStationFilterModel;
class RadioFrequencyScanner;
class QSortFilterProxyModel;

// Class declaration
class RadioStationsView : public RadioViewBase
{
    Q_OBJECT
    Q_PROPERTY(HbIcon nowPlayingIcon READ nowPlayingIcon WRITE setNowPlayingIcon)
    Q_PROPERTY(HbIcon favoriteIcon READ favoriteIcon WRITE setFavoriteIcon)
    Q_PROPERTY(HbIcon nonFavoriteIcon READ nonFavoriteIcon WRITE setNonFavoriteIcon)

public:

    explicit RadioStationsView();
    ~RadioStationsView();

    void setNowPlayingIcon( const HbIcon& nowPlayingIcon );
    HbIcon nowPlayingIcon() const;

    void setFavoriteIcon( const HbIcon& favoriteIcon );
    HbIcon favoriteIcon() const;

    void setNonFavoriteIcon( const HbIcon& nonFavoriteIcon );
    HbIcon nonFavoriteIcon() const;

private slots:

    void handleClick( const QModelIndex& index );
    void handleLongPress( HbAbstractViewItem* item, const QPointF& coords );
    void updateAntennaStatus( bool connected );
    void updateViewMode();
    void startScanning();
    void finishScanning();
    void updateVisibilities();
    void openMultiSelection();
    void play();            // Called from context menu
    void rename();          // Called from context menu
    void toggleFavorite();  // Called from context menu
    void deleteStation();   // Called from context menu
    void renameDone( HbAction* action );

private:

// from base class RadioViewBase

    void init();
    void userAccepted();

// New functions

    void initListView();

private: //data

    RadioStationModel*                      mModel;

    QSortFilterProxyModel*                  mFilterModel;

    HbAction*                               mScanStationsAction;
    HbAction*                               mMultiSelectionAction;

    HbListView*                             mStationsList;

    HbAction*                               mFavoritesButton;
    HbAction*                               mLocalStationsButton;

    HbIcon                                  mFavoriteIcon;
    HbIcon                                  mNonFavoriteIcon;
    HbIcon                                  mNowPlayingIcon;

    QScopedPointer<RadioStation>            mSelectedStation;

    QScopedPointer<RadioFrequencyScanner>   mFrequencyScanner;

    enum UserQuestion { NoQuestion, DeleteStation, StartScanning, MultiSelection };
    UserQuestion                            mCurrentQuestion;

};

#endif // RADIOSTATIONSVIEW_H
