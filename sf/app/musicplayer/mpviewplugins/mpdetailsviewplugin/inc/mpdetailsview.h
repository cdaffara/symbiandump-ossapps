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
* Description: Music Player details view.
*
*/

#ifndef MPDETAILSVIEW_H
#define MPDETAILSVIEW_H

#include <QPointer>
#include <QNetworkReply>
#include <QDomDocument>
#include <QMap>

#include "mpviewbase.h"

class QGraphicsWebView;
class QNetworkAccessManager;

class HbMainWindow;
class HbAction;
class HbLabel;
class HbWidget;
class HbGroupBox;
class HbPushButton;
class HbDocumentLoader;
class HbListWidget;
class HbListWidgetItem;

class QGraphicsLinearLayout;
class HbProgressBar;

class MpEngine;
class MpSongData;
class MpQueryManager;

//class declaration
/*!
  Details view is the "flipside view" of Music Player.
  In addition to displaying song details, details view
  also provides user the possibility to get
  recommendations based on his playlist (by displaying
  a list of favourites, fetched from OVI music server).

  DetailsView's song information will not be updated
  if the track changes while detailsview is active.
*/
class MpDetailsView : public MpViewBase
{
    Q_OBJECT
    
public:

    MpDetailsView();
    virtual ~MpDetailsView();

    void initializeView();
    void activateView();
    void deactivateView();

signals:
    void command( int aCommand );
    
public slots:
    void back();    
    void albumArtChanged();
    
private slots:
    void queryInspireMe(bool storeUpdated = true);
    void queryLocalMusicStore();
    void handlePlaybackInfoChanged();
    void songDetailInfoChanged();
    
    void handleListItemSelected( HbListWidgetItem  *item);
    void handleDetailsGroupBoxToggled( bool state );
    void handleInspireMeGroupBoxToggled( bool state );
    void renderInspireMeMetadata();
    void renderInspireMeAlbumArts();
    void abortInspireMeProcess();

private:
    void startInspireMe();
    bool isMetadata() const;
    void saveGroupBoxStates();

private:    
    MpEngine                *mMpEngine;         // Owned by the factory
    
    MpSongData              *mSongData;
    
    bool                    mActivated;
    HbAction                *mSoftKeyBack;
    
    HbLabel                 *mSongText; //owned
    HbLabel                 *mAlbumText; //owned
    HbLabel                 *mArtistText; //owned
    HbLabel                 *mAlbumArt; //owned
    HbGroupBox              *mSongDetailsGroupBox; //owned
    HbGroupBox              *mInspireMeGroupBox; //owned
    HbDocumentLoader        *mDocumentLoader;//owned
    HbListWidget            *mDetailList; // owned by mSongDetailsGroupBox
    HbListWidget            *mInspireList; // owned by mInspireMeGroupBox
    HbPushButton            *mButton;
    HbProgressBar           *mInspireMeProgressBar;

    MpQueryManager*         mMpQueryManager;     // Own
	
    bool                    mInspireMeQueryOngoing;
    bool                    mInspireMeQueryRendered;
    
    Q_DISABLE_COPY(MpDetailsView)
};


#endif //MPDETAILSVIEW_H
