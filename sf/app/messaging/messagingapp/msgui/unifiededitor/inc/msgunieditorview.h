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
 * Description:uni editor view class.
 *
 */

#ifndef MSG_UNIFIED_EDITOR_VIEW_H
#define MSG_UNIFIED_EDITOR_VIEW_H

#ifdef MSGUI_UNIT_TEST
 #define UNIFIEDEDITOR_EXPORT
#else
 #ifdef UNIFIEDEDITOR_DLL
  #define UNIFIEDEDITOR_EXPORT Q_DECL_EXPORT
 #else
  #define UNIFIEDEDITOR_EXPORT Q_DECL_IMPORT
 #endif
#endif

#include "msgbaseview.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"

class HbWidget;
class HbAction;
class HbGroupBox;
class QGraphicsLinearLayout;
class MsgUnifiedEditorSubject;
class MsgUnifiedEditorAddress;
class MsgUnifiedEditorBody;
class MsgUnifiedEditorMonitor;
class MsgAttachmentContainer;
class UniEditorPluginLoader;
class HbListWidgetItem;
class HbAbstractVkbHost;
class MsgUnifiedEditorBaseWidget;
class HbListWidget;
class MsgAudioFetcherDialog;

class UNIFIEDEDITOR_EXPORT MsgUnifiedEditorView : public MsgBaseView
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorView(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorView();

    /**
     * Populate editor with passed information
     * @param editorData data for editor's usage
     */
    void populateContent(const QVariantList& editorData);
    
    /**
     * Opens the drafts message into editor
     * @param editorData data for editor's usage
     */
    void openDraftsMessage(const QVariantList& editorData);


    /**
     * Saves the content inside editor to save
     */
    int saveContentToDrafts();
    
    /**
     * Event handler
     * @param key Key code
     * @return true if key event handled else false.
     * @see MsgBaseView
     */
    bool handleKeyEvent(int key);

protected:
    /**
     * reimplemented from base class to provide proper geometry for scrolling.
     */
    void resizeEvent( QGraphicsSceneResizeEvent * event );

private:

    /**
     * helper method to add menu.
     */
    void addMenu();

    /**
     * helper method to add tool bar.
     */
    void addToolBar();

    /**
     * Packs the content inside editor into converged message
     * @param [OUT]msg, converged message to hold editor data
     * @param isSave, flag to indicate that msg needs to be packed
     * for saving to draft or not
     * @return returns error if something fails
     */
    int packMessage(ConvergedMessage &msg, bool isSave=false);

    /**
     * Populate editor with prepopulated msg content
     * @param messageDetails message details
     * @param draftMessage boolean for specifying draft message
     */
    void populateContentIntoEditor(
        const ConvergedMessage& messageDetails, bool draftMessage = false);
    
    /**
     * Populate the editor with the message's content
     * for various cases like forward, reply, replyAll
     * @param messageId, unique id of message in store
     * @param messageType, sms or mms
     * @param editorOperation, forward or reply or replyAll etc
     */
    void fetchMessageFromStore(ConvergedMessageId& messageId,
        ConvergedMessage::MessageType messageType,
        int editorOperation);
    
    /**
     * Create VCards for all the contacts available in the list
     * @param value list of fetched contacts
     * @param [OUT]filelist list of newly added vcard files
     * @return errId KErrNone if no error
     */
    int createVCards(const QVariant& value, QStringList& filelist);
    
    /**
     * Resolve name conflict when adding attachments to editor
     * @param suggestedName suggested file name to use
     * @return fully qualified path of the file in unieditor's temp folder
     */
    QString generateFileName(QString& suggestedName);
    
    /**
     * Fetch images 
     */
    void fetchImages();

    /**
     * Fetch contacts
     */
    void fetchContacts();

    /**
     * Fetch audio
     */
    void fetchAudio();

    /**
     * To hide/show chrome.
     */
    void hideChrome(bool hide);
    
    /**
     * To initialize view.
     */
    void initView();
    
    /**
     * Creates temp folder for editor.
     */
    bool createTempFolder();
    
    /**
     * Removes editors temp folder.
     */
    void removeTempFolder();

    /**
     * Attachment options in TBE
     * Row number of the TBE actions
     */
    enum TBE_AttachOption
    {
        TBE_PHOTO = 0x00, TBE_SOUND = 0x01, TBE_VCARD = 0x02
    };

    /**
     * Enable/Disable attachment options for slide-conformance
     * @param opt, row number of action in TBE
     * @param isEnabled, true/false
     */
    void setAttachOptionEnabled(MsgUnifiedEditorView::TBE_AttachOption opt,
            bool enable);

    /**
     * Addresses are all ok. Now parse not allowed chars away 
	 * before giving it to MTM
	 * @param addresses, list of ConvergedMessageAddress
     */
    void formatAddresses(ConvergedMessageAddressList& addresses);
    
private slots:

    /**
     * slot to add subject field.
     */
    void addSubject();

    /**
     * slot to add Cc,Bcc field.
     */
    void addCcBcc();

    /**
     * slot to handle Cc, Bcc group-box's toggle action
     */
    void updateOtherRecipientCount(bool state);

    /**
     * slot to change priority.
     */
    void changePriority();

    /**
     * slot to current delete message.
     */
    void deleteMessage();

    /**
     * slot to images fetched.
     */
    void imagesFetched(const QVariant& result );

    /**
     * slot to receive fetched contacts
     */
    void contactsFetched(const QVariant& value);

    /**
     * slot to send message.
     */
    void send();
	
    /**
     * slot to tell view to remove the attachment's container
     */
    void removeAttachmentContainer();

    /**
     * add attachments to the editor
     * @param files list of file paths
     */
    void addAttachments(QStringList files);

    /**
     * add an attachment to the editor
     * @return addition operation status
     */
    int addAttachment(const QString& filepath);

	/**
     * called when extention item is clicked.
     */
    void handleViewExtnActivated(HbListWidgetItem* item);
    
    /**
     * Signal emitted when an error is generated.
     * @param errorCode Error code.
     * @param errorMessage Error description.
     */
    void serviceRequestError(int errorCode, const QString& errorMessage);
   
    /**
     * Activate Input Blocker
     */
    void activateInputBlocker();

    /**
     * Deactivate Input Blocker
     */
    void deactivateInputBlocker();
    
    /**
     * Resizes the view when VKB is opened.
     * This slot is triggered when vkb is opened.
     */
    void vkbOpened();

    /**
     * Resizes the view when VKB is closed.
     * This slot is triggered when VKB focus is lost.
     */
    void vkbClosed();
    
    /**
     * Slot to do delayed construction.
     */
    void doDelayedConstruction();
    
    /**
     * Sets focus to item.
     */
    void setFocus(MsgUnifiedEditorBaseWidget* item);
    
    /**
     * Listens to contentChanged signal of various fields.
     */
    void onContentChanged();

	/**
     * This slot is called when delete message dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogDeleteMsg(HbAction* action);

	/**
     * This slot is called when define sms settings dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogSmsSettings(HbAction* action);
    
    /**
     * This slot is called when define mms settings dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogMmsSettings(HbAction* action);    
	
	/**
     * Enable/Disable send tool button.
     * @param true/false to enable/disable.
     */
     void enableSendButton(bool enable);
     
     /**
      * This slot is triggered when vkb is about to be opened.
      */
     void vkbAboutToOpen();
     
     /**
      * This slot is triggered when vkb is about to be closed.
      */
     void vkbAboutToClose();
     
     /** 
      * This slot is called after sound clip is 
      * selected from audio fetcher dialog    
      */
     void onAudioSelected(QString& filePath);
	 
	 /**
      * Check if the reply-path is broken
      * @return bool, true if reply-path constraints are broken
      */
     bool isReplyPathBroken();
    
private:
     
    HbAction* mSubjectAction;
    HbAction* mCcBccAction;
    HbAction* mSendAction;
    QGraphicsLinearLayout* mMainLayout;
    MsgUnifiedEditorSubject* mSubjectField;
    MsgUnifiedEditorAddress* mToField;
    MsgUnifiedEditorAddress* mCcField;
    MsgUnifiedEditorAddress* mBccField;
    MsgUnifiedEditorBody*   mBody;

    HbWidget* mContentWidget;

    MsgUnifiedEditorMonitor* mMsgMonitor;
    MsgAttachmentContainer* mAttachmentContainer;
    UniEditorPluginLoader* mPluginLoader;
    ConvergedMessageId mOpenedMessageId;
    ConvergedMessage::MessageType mmOpenedMessageType;
	bool mCanSaveToDrafts;
	HbAction *mAttachAction;
    
	/**
	 * TBE's content widget
	 */
	HbListWidget* mTBExtnContentWidget;
	
    /**
     * Instance of VKB host
     */
	HbAbstractVkbHost* mVkbHost;
	
	/**
	 * Instance of Audio Fetcher Dialog
	 * Need to show when attachment audio selected
	 */
	MsgAudioFetcherDialog* mDialog;

    /**
     * Originating SC, 
     * to be used only when reply-path is available
     */
    QString mOriginatingSC;

    /**
     * Originating SME,
     * to be used only when reply-path is available
     */
    QString mOriginatingSME;

    /**
     * Flag to tell if reply-path is available
     */
    bool mReplyPath;	

	friend class MsgUnifiedEditorMonitor;
    };

#endif //MSG_UNIFIED_EDITOR_VIEW_H
