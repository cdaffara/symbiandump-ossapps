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
* Description:   VideoCollectionListener class definition*
*/

#ifndef __VIDEOCOLLECTIONLISTENER_H
#define __VIDEOCOLLECTIONLISTENER_H

// INCLUDES
#include <QObject>
#include <mpxcollectionobserver.h>


// FORWARD DECLARATIONS
class VideoCollectionClient;
class VideoCollectionUtils;
class VideoDataSignalReceiver;

// CLASS DECLARATION
class VideoCollectionListener : public QObject, public MMPXCollectionObserver
{
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
   
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoCollectionListener) 
      
public: 
    
    /**
     * Constructor
     */
    VideoCollectionListener(VideoCollectionClient &collectionClient,
                            VideoDataSignalReceiver &mSignalReceiver);

    /**
     * Destructor.
     */
    virtual ~VideoCollectionListener();
     
signals:

    /**
     * Emitted when new video list is available.
     * 
     * If client needs to maintains media-array, it should create
     * a copy from provided array pointer
     * 
     * @param pointer to new media array, ownership is NOT transferred
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * Emitted from HandleOpenL when mNewArrayRequest is false.
     * In this case we moght have not got all videos from previous
     * HandleOpenL so we need to provide this list to client for it to
     * check wether there are some new videos to be appended
     * 
     * @param pointer to appendable media array, ownership is NOT transferred
     */
    void videoListAppended(CMPXMediaArray*);       
    
    /**
     * Emitted when a single video item has fetched.
     * 
     * @param pointer to Video item. Ownership is NOT transferred
     */
    void newVideoAvailable(CMPXMedia*);
    
    /**
     * Emitted when received an avent about externally removed video.
     * 
     * @param video id
     */
    void videoDeleted(TMPXItemId);
           
    /**
     * Emitted when multiple video deletion command is completed.
     * 
     * @param int contains count of overall deleted items
     * @param media-array containing possible failed medias. Ownership is NOT transferred
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
    
    /**
     * Emitted when all details for a video have been fetched.
     * 
     * @param id of video whose details have bee fetched.
     */
    void videoDetailsCompleted(TMPXItemId);
    
    /**
     * Emitted when album items are recieved.
     * 
     * @param albumId, Album which items are received.
     * @param albumItems, Items belonging to the current album.
     */
    void albumListAvailable(TMPXItemId albumId, CMPXMediaArray *albumItems);
        
private: // From MMPXCollectionObserver

    /**
     * Handle extended media properties
     *
     * not implemented here
     * 
     * @param aMedia media 
     * @param aError error code    
     */
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );
    
    /**
     * Handles the collection entries being opened. Typically called
     * when client has Open()'d a folder.
     * Uses mNewArrayRequest flag to indicate wether newVideoList or
     * videoArrayAppended is emitted. If flag value is true, newVideolist 
     * is emitted and flag value is setted false. This is because only one 
     * newVideoList signal alloved per content request. 
     *
     * @param aEntries collection entries opened
     * @param aIndex focused entry
     * @param aComplete ETrue no more entries. EFalse more entries
     *                  expected
     * @param aError error code   
     */
    void HandleOpenL( const CMPXMedia& aEntries,
                      TInt aIndex,
                      TBool aComplete,
                      TInt aError );

    /**
     * Handles the item being opened. Typically called
     * when client has Open()'d an item. Client typically responds by
     * 'playing' the item via the playlist.
     * 
     * Not implemented here.
     *
     * @param aPlaylist collection playlist
     * @param aError error code   
     */                         
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /**
     * Handle completion of a asynchronous command
     *
     * @param aCommandResult result of the command, NULL if error
     * @param aError error code    
     */
    void HandleCommandComplete( CMPXCommand* aCommandResult, TInt aError );

    /** 
     * Handle collection message
     * NOTE: only one of HandleCollectionMessage callback can be implemented
     * 
     * @param aMessage collection message, ownership not transferred. 
     *        Please check aMsg is not NULL before using it. If aErr is not 
     *        KErrNone, plugin might still call back with more info in the aMsg.
     * @param aErr system error code.
     */
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr ); 
  
private: 
    
    /**
     * Default constructor should not be used
     */
    VideoCollectionListener();
    
    /**
     * called by the HandleCollectionMessage(), in case message received
     * contains array -based data.
     * 
     * @param aMessage
     */
    void handleMyVideosMessageArray(CMPXMessage *aMessage);
    
    /**
     * redirects the actual messagehandling based on given myVideosMsgId.
     * Messages to handle contains KVcxCommandIdMyVideos as their 
     * general message id 
     * 
     * @param myVideosMsgId message id parsed by the caller
     * @param aMessage
     */
    void handleMyVideosMPXMessage(int &myVideosMsgId, CMPXMessage *aMessage);
    
    /**
     * messages that do fall into KVcxCommandIdMyVideos, are passed here 
     * to be treated as "common" mpx messages.
     * 
     * @param myVideosMsgId message id parsed by the caller
     * @param aMessage
     */
    void handleMPXMessage(TMPXMessageId &mpxMsgId, CMPXMessage *aMessage);
    
    /**
     * Handles general MPX message (TMPXMessageId == KMPXMessageGeneral)
     * In here we handle only the collection opening case: when collection
     * informs it has opened and browsed to all videos category, we need to 
     * open collection to it's current state in order to get video list. 
     * 
     * @param aMessage collection message.
     */
    void handleGeneralMPXMessage(CMPXMessage* aMessage); 
 
    /**
     * Handles item changed message.
     * 
     * @param aMessage collection message.
     */
    void handleMyVideosItemsChanged( CMPXMessage* aMessage);        
    
    /**
     * Handles delete related message.
     * 
     * @param aMessage collection message.
     */
    void handleMyVideosDeleteMessage(CMPXMessage* aMessage);         

    /**
     * Handles response message when MPX Media object requested.
     *
     * @param aMessage collection message.
     */
    void handleGetMediasByMpxIdResp(CMPXMessage* aMessage);
    
    /**
    * Handles response message when video details requested.
    * 
    * @param aMessage collection message.
    */
    void handleGetVideoDetailsResp(CMPXMessage* aMessage);

    
private:
    
    /**
     * Reference to collection client
     */
    VideoCollectionClient &mCollectionClient;
    
    /**
     * reference to collection message signal receiver.
     * Used as observer for performance.
     */
    VideoDataSignalReceiver &mSignalReceiver;
    
    /**
     * Reference to utility objetc used to parse CMPXMessages and
     * CMPXMedias
     */
    VideoCollectionUtils &mVideoUtils;
        
 };

#endif // __VIDEOCOLLECTIONLISTENER_H
