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
* Description:   VideoFileDetailsViewPlugin class definition
*
*/

#ifndef VIDEOFILEDETAILSPLUGIN_H
#define VIDEOFILEDETAILSPLUGIN_H


// INCLUDES
#include <qobject.h>
#include <mpxviewpluginqt.h>
#include <hbdocumentloader.h>
#include <qpixmap.h>
#include <mpxitemid.h>

class VideoProxyModelGeneric;
class QModelIndex;
class HbAction;
class HbMarqueeItem;
class ThumbnailManager;
class VideoServices;
class VideoCollectionWrapper;
class VideoDetailsLabel;
class ShareUi;

class VideoFileDetailsViewPlugin : public MpxViewPlugin
	{

    Q_OBJECT

public: // Constructor / destructor

    /**
     * Contructor.
     *
     */
    VideoFileDetailsViewPlugin();

    /**
     * Destructor.
     *
     */
    virtual ~VideoFileDetailsViewPlugin();

public: // from QViewPlugin

    /**
     * Initializes view creation.
     *
     */
    void createView();

    /**
     * Deallocates view and it's objects.
     */
    void destroyView();

    /**
     * Activates view
     *
     */
    void activateView();

    /**
     * Deactivates view
     *
     */
    void deactivateView();

    /**
     * Returns a pointer to the view read from the XML
     *
     * @return QGraphicsWidget*
     */
    QGraphicsWidget* getView();

signals:
    /**
     * Command signal, plugin user shoulf connect this in case it wants
     * to receive commands emitted from the view.
     *
     * @param command id
     */
    void command( int );

    /**
     * Signals the URI of the current video to VideoServices
     *
     * @param URI of the video
     */
    void fileUri(const QString&);

public slots: // from QViewPlugin

    /**
     * Plugin user can notify orientation changes by connecting into this slot
     * to it's signal. In normal cases, view handles orientation chages itself.
     *
     * @param orientation new orientation
     */
    void orientationChange( Qt::Orientation orientation );

    /**
     * Plugin user can notify back button changes by connecting into this slot
     *
     */
    void back();
    
    /**
     * Signaled by HbMessageBox when it's closed.
     */
    void deleteVideoDialogFinished(int action);

private slots:

    /**
     * Signaled when attach button is clicked, only when started as a service.
	 * Gets the URI of the current video
     *
     */
	void getFileUri();

    /**
     * Signaled when short details are ready.
     *
     * @param id Mpx id of the clip, needed when getting the data from model.
     */
    void shortDetailsReadySlot(TMPXItemId id);

    /**
     * Signaled when full details are ready.
     *
     * @param metadata The metadata for the clip.
     */
    void fullDetailsReadySlot(QVariant& variant);

    /**
     * Slot that receives signal from play button to start playback.
     */
    void startPlaybackSlot();

    /**
     * Slot that receives signal from send button
     */
    void sendVideoSlot();

    /**
     * Slot that receives signal from delete button
     */
    void deleteVideoSlot();

    /**
     * Slot that receives signal when video(s) removed.
     * If video to be removed is the one whose details
     * are visible, this view is deactivated and collection
     * list view is put back on.
     *
     * @param parent parent item index (not used at the moment)
     * @param first first item to remove
     * @param last last item to remove
     */
    void rowsRemovedSlot(const QModelIndex &parent, int first, int last);

    /**
     * Slot is connected into videocollection wrapper's asyncStatus -signal
     *
     * Handles possible collection error; usually by just showing error msg
     *
     * @param errorCode error code
     * @param additional additional data gotten from the error
     */
    void handleErrorSlot(int errorCode, QVariant &additional);

    /**
     * Slot that is connected to thumbnailReady signal in tnwrapper, which
     * signals when thumbnail loading has been completed.
     *
     * @param pixmap     An object representing the resulting thumbnail.
     * @param clientData Client data
     * @param id         Request ID for the operation
     * @param errorCode  error code
     */
    void thumbnailReadySlot(QPixmap pixmap, void * clientData, int id, int errorCode);

private:

    /**
     * Signaled for one item deletion.
     *
     */
    void deleteItem(QModelIndex index);

    /**
     * Starts fetching the large thumbnail with tnwrapper.
     */
    void startFetchingThumbnail();
    
    /**
     * Gets the play icon for thumbnail.
     */
    const QPixmap &playIcon();

    /**
     * Finds and return the widget from document loader with the given name. Casts
     * to templated type.
     *
     * @param name Name of the widget
     * @return The widget.
     */
    template<class T>
    T* findWidget(QString name);

    template<class T>
    T* findObject(QString name);

private:

    /**
     * Document loader that holds the view object
     */
    HbDocumentLoader mLoader;

    /**
     * Pointer to the model that holds video details. Not owned.
     */
    VideoProxyModelGeneric* mModel;

    /**
     * pointer to videoservices instance
     */
    VideoServices* mVideoServices;

    /**
     * Activated flag. Set as true when view is properly activated.
     * If flag is false, no operations can be do to the view.
     */
    bool mActivated;

    /**
     * Boolean for knowing when the app was started as a service.
     */
    bool mIsService;

    /**
     * Mpx id of the video clip.
     */
    TMPXItemId mVideoId;
    
    /**
     * Index of the clip to be deleted in the proxy model.
     */
    int mDeletedIndex;
    
    /**
     * Previously loaded orientation.
     */
    Qt::Orientation mPreviousOrietation;

	/**
     * Navigation softkey action object for back.
     */
	HbAction *mNavKeyBackAction;

    /**
     * Title animation widget
     */
    HbMarqueeItem *mTitleAnim;

    /**
     * Thumbnail label
     */
    VideoDetailsLabel *mThumbLabel;

    /**
     * Thumbnail manager.
     */
    ThumbnailManager* mThumbnailManager;

    /**
     * Collection wrapper.
     */
    VideoCollectionWrapper &mCollectionWrapper;
    
    /**
     * Play icon with transparent background.
     */
    QPixmap mPlayIcon;
    
    /**
     * Share ui object for handling of sharing videos
     */
    ShareUi *mShareUi;
    };

#endif  // VIDEOFILEDETAILSPLUGIN_H

// End of File
