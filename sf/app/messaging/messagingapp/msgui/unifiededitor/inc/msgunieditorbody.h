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
 * Description:message body. to enter text and show media inserted.
 *
 */

#ifndef MSG_UNIFIED_EDITOR_BODY_H
#define MSG_UNIFIED_EDITOR_BODY_H

#include <msgunieditorbasewidget.h>
#include <f32file.h>
#include "msgunieditorprocessimageoperation.h"

class HbTextEdit;
class HbTextItem;
class HbFrameItem;
class CMsgMediaResolver;
class CMsgImageInfo;
class MmsConformanceCheck;
class UniEditorPluginInterface;
class UniEditorPluginLoader;
class MsgUnifiedEditorPixmapWidget;
class MsgUniFiedEditorAudioWidget;


class MsgUnifiedEditorBody : public MsgUnifiedEditorBaseWidget,
                             public MUniEditorProcessImageOperationObserver
{
    Q_OBJECT

    Q_PROPERTY(bool hasImage READ hasImage WRITE setImage)
    Q_PROPERTY(bool hasAudio READ hasAudio WRITE setAudio)
public:

    /**
     * Constructor
     */
    MsgUnifiedEditorBody(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorBody();

    /**
     * Seeker method to return back data to editor's view
     */
    QString text();

    /**
     * Seeker method to return back data to editor's view
     */
    const QStringList mediaContent();
    
    /**
     * get size of body content for MMS size calculations
     * @return size of body content
     */
    int bodySize();
    
    /*
     * Returns value of mUnicode 
     * @return mUnicode   
     */
     bool isUnicode();

     /*
      * Disables char counter
      */
     void disableCharCounter();

     /*
      * Enables char counter
      */
     void enableCharCounter();
     
     /**
      * To set focus on editable field.
      */
     void setFocus();

     /**
      * Get to find body already contains an image
      * @return bool
      */
     bool hasImage();

     /**
      * Get to find body already contains an audio
      * @return bool
      */
     bool hasAudio();
     
    /**
     * from MUniEditorProcessImageOperationObserver  
     * @see MUniEditorProcessImageOperationObserver
     */ 
    void EditorOperationEvent( TUniEditorProcessImageOperationEvent aEvent,
                               TFileName& aFileName );
    /**
	 * Function which tells whether the image resize process is in progress
	 */
    bool isImageResizing()
        {
        return mIsImageResizing;
        }

    /**
     * remove media content.called when file is not avaialable
     * @param fileName file name
     */
    void removeMediaContent(QString fileName);
    
public slots:
    /**
     * Called to insert image content in editor.
     * @param medialist list of absolute paths of media.
     * @param draftMessage specifies draft message
     */
    void setImage(QString& imagefile, bool draftMessage = false);

    /**
     * Called to insert audio content in editor.
     * @param medialist list of absolute paths of media.
     */
    void setAudio(QString& audiofile);

    /**
     * Called to insert body text in editor.
     * @param text body text.
     */
    void setText(QString& text);

signals:
    /**
     * Emitted when send button from virtual ITUT is pressed.
     */
    void sendMessage();

    /**
     * Emitted when msg-body content changes
     */
    void contentChanged();
	
    /**
     * Emitted when image is being processed.
     * @param enable, true to enable/ false to disable.
     */
   void enableSendButton(bool enable) const;



protected: // from HbWidget
    
    /**
     * Reimplemented from base class to give proper size.
     * @see HbWidget
     */
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

private slots:

    /**
     * called from the media object's item specific menu
     */
    void removeMedia();
    
    /**
     * handle text changes in body field
     */
    void onTextChanged();

private:

    /**
     * Get to find body already contains an image
     * @return bool
     */
    void setImage(bool image = false);

    /**
     * Set that body now contains an audio
     */
	void setAudio(bool audio = false);

    /**
     * size of the msg
     */
    int messageSize();    
    
    /**
     * Handler for set image function 
     */
    void handleSetImage();    

    /**
     * Starts the resize animation 
     */
    void startResizeAnimation();

    /**
     * Stops the resize animation 
     */
    void stopResizeAnimation();

private:
    /**
     * Info if message body has image.
     * true if message body has image else false.
     */
    bool mHasImage;

    /**
     * Info if message body has audio.
     * true if message body has audio else false.
     */
    bool mHasAudio;

    /**
     * editor to in put text.
     */
    HbTextEdit* mTextEdit;

    /**
     * icon item to preview images.
     */
    MsgUnifiedEditorPixmapWidget* mPixmapItem;

    /**
     * inline audio item
     */
    MsgUniFiedEditorAudioWidget* mAudioItem;

	/**
	 * Image file contained inside body
	 */
    QString mImageFile;

    /**
     * Saves the original file which has to be used if resize fails
     */
    QString mSavedImageFile;

	/**
	 * Audio file contained inside body
	 */
    QString mAudioFile;

	/**
	 * Video file contained inside body
	 */
    QString mVideoFile;
	
    /**
     * MMs conformance check utility class
     */
    MmsConformanceCheck* mMmsConformanceCheck;
	
    /**
     * Size of image in body 
     */
    int mImageSize;
    
    /**
     * Size of audio in body
     */
    int mAudioSize;
    
    /**
     * Size of video in body
     */
    int mVideoSize;

    /**
     * CUniEditorProcessImageOperation object
     */
    CUniEditorProcessImageOperation *mProcessImageOperation;

    /**
     * CMsgMediaResolver object
     */
    CMsgMediaResolver* mMediaResolver;
    
    /**
     * CMsgImageInfo object
     */
    CMsgImageInfo *mImageInfo;    
    
    /**
     * Instance of HbTextItem
     * Will be deleted automatically by parent.
     * Own.
     */
    HbTextItem *mCharCounter;

    /**
     * Instance of HbFrameItem
     * Will be deleted automatically by parent.
     * Own.
     */
    HbFrameItem* mBackgroundItem;
    
    /**
     * Holds the previous buffer inside msgeditor
     */
    QString mPrevBuffer;

    /**
     * Holds char type supported
     */
    int mCharSupportType;

    /**
     * Instance of UniEditorPluginInterface
     * Will be deleted automatically by parent.
     * Own.
     */
    UniEditorPluginInterface* mPluginInterface;

    /**
     * Instance of UniEditorPluginLoader
     * Will be deleted when UniEditorPluginInterface object is deleted
     * Not Own.
     */
    UniEditorPluginLoader* mPluginLoader;

    /*
     * Maintains information if any unicode character has been entered or not
     */
    bool mUnicode;
    
    /**
     * Content widget for processing animation.
     */
    HbWidget* mProcessingWidget;

    /**
     * boolean specifying a draft message
     */
    bool mDraftMessage ;
    
    /*
     * Flag to indicate the state of image resizing process. 
	 * true - resize in progress. false - resize is not underway.
     */
    bool mIsImageResizing;
};

#endif //MSG_UNIFIED_EDITOR_BODY_H
