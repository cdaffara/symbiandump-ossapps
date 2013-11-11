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
 * Description:Message chat View for the messaging application.
 *
 */

#ifndef MSG_CONVERSATION_VIEW_H
#define MSG_CONVERSATION_VIEW_H

// INCLUDES
#include "msgbaseview.h"
#include "convergedmessage.h"

//Forward declarations
class HbListView;
class HbAbstractViewItem;
class MsgSendUtil;
class QStandardItemModel;
class MsgEditorWidget;
class MsgContactCardWidget;
class MsgConversationViewItem;
class HbStaticVkbHost;
class QGraphicsLinearLayout;
class HbAction;
class MsgAudioFetcherDialog;

//Defines
#define INVALID_MSG_ID -1

/**
 * This class provides the message chat view for the messaging application.
 * Data source for this view is the conversation model.
 */
class MsgConversationView : public MsgBaseView
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgConversationView(MsgContactCardWidget *contactCardWidget,
        QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgConversationView();


    /**
     * Clear editors
     * clear message and address editors
     */
    void clearEditors();

    /**
     * Save the content inside editor to drafts
     * @return valid message id if save is success
     * else invalid message id ( i.e. -1 )
     */
    int saveContentToDrafts();

    /**
     * Event handler
     * @param key Key code
     * @return true if key event handled else false.
     * @see MsgBaseView
     */
    bool handleKeyEvent(int key);

private slots:

    /**
     * Slot is called when menu is about to be shown.
     * Populates the menu with relevant actions.
     */
    void menuAboutToShow();
    
	/**
     * This slot is called when settings dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogSettingsLaunch(HbAction* action);
    
	/**
     * This slot is called when delete message centre dialog is launched.
     * @param action selected action (yes or no).
     */	
    void onDialogdeleteMsg(HbAction* action);
	
	/**
     * This slot is called when download message centre dialog is launched.
     * @param action selected action (yes or no).
     */	    
    void onDialogDownLoadMsg(HbAction* action);
	
	/**
     * This slot is called when save tone dialog is launched.
     * @param action selected action (yes or no).
     */	
    void onDialogSaveTone(HbAction* action);

    /**
     * This slot is called when the orientation is changed
     * @param newOrientation orientation
     */
    void onOrientationChanged(Qt::Orientation newOrientation);

    /**
     * This slot is called when the orientation is about to bechanged
     */
    void onOrientationAboutToBeChanged();
    
private:

    /**
     * View initialization function.
     */
    void setupView();

    /**
     * Setup view menu items.
     */
    void setupMenu();

    /**
     * Triggers model to fetch more conversations.
     */
    void fetchMoreConversations();

    /**
     * Populates ConvergedMessage for sending.
     * @param ConvergedMessage to be populated
     * @see ConvergedMessage::MessageType
     */
    void populateForSending(ConvergedMessage &message);
    
    /**
     * Adds context menu entries to context menu
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void setContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);
	
    /**
     * Adds context menu entry to context menu for saving items
     * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */    
    void addSaveItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);
    
    /**
     * Adds context menu entry to context menu for Opening items
	 * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void addOpenItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Resending items
     * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */  
    void addResendItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Forwarding items
	 * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void addForwardItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Deleting items
	 * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void addDeleteItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Downloading items
     * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu    
     * @see ConvergedMessage::MessageType
     */
    void addDownloadItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu);

    /**
     * Launches the BT message display service.
     * @param index Index of the item activated/touched
     */
    void launchBtDisplayService(const QModelIndex & index);

public slots:
    /**
     * Refreshes all widgets in the conversation view according to latest model
     * data
     */
    void refreshView();

    /**
     * This slot is called when the view is successfully added to main window
     */
    void onViewReady();
    
private slots:

    /**
     * Utility method to scroll the list to show the bottom most item
     */
    void scrollToBottom();
    
    void onConversationViewEmpty();

    /**
     * Handler for long tap of a list item.
     * @param viewItem  Long tapped view item
     * @param point X and Y co-ordinates of long tap
     */
    void longPressed(HbAbstractViewItem* viewItem, const QPointF& point);

    /**
     * Handler for short tap/direct touch of a list item.
     * @param index index of the item activated/touched
     */
    void openItem(const QModelIndex & index);

    /**
     * Sends the message
     */
    void send();

    // ----------------------------- View Specific Menu Slots--------------------//

   
    /**
     * Fetch images 
     */
    void fetchImages();

    /**
     * Fectch conatcts
     */
    void fetchContacts();

    /**
     * Fectch audio
     */
    void fetchAudio();

    /*
     * Get the photos and launch editor
     */
    void imagesFetched(const QVariant& result );
    
    /**
     * slot to receive fetched contacts for addition
     */
    void contactsFetched(const QVariant& value);

    /**
     * slot to receive fetched contacts for vcard addition
     */
    void contactsFetchedForVCards(const QVariant& value);
    
    /*
     * Adds subject file to editor.
     */
    void addSubject();


    //----------------------Item Specific menu slots ---------------------//

    /**
     * Forwards the message
     */
    void forwardMessage();

    /**
     * Deletes the item
     */
    void deleteItem();
    
    /**
     * Resends the item. This is only valid in the failed message case.
     */
    void resendMessage();

    /**
     * Download the specified message
     */
    void downloadMessage();

    /**
     * Open the item
     */
    void openItem();

    /**
     * Set model to the view 
     */    
    void populateConversationsView();

    /**
     * Save ringing tone
     */
    void saveRingingTone();   
    
    /**
     * Launch Editor since sms character limit reached
     */
    void handleSmsCharLimitReached();   
    
    /**
     * Hanldes saving of vcard.
     */
    void saveVCard();
    
    /**
     * Tells is mms is shared among conversations.
     * @param messageId, message id.
     * @return true is shared among conversations.
     */
    bool isSharedMessage(qint32 messageId);
    
    /** 
     * This slot is called after sound clip is 
     * selected from audio fetcher dialog    
     */
    void onAudioSelected(QString& filePath);
    
signals:
    /**
     * Signal emitted to inform close the conversation view.
     * When Conversation view has opened in send mode, after sending
     * the message, this signal is emitted.
     */
    void closeConversationView();

    /**
     * Emitted when editor is tapped to reply.
     */
    void replyStarted();
    
   /**
    * This signal is emitted when vkb is open/closed.
    * @param state True if keypad is opened else false.
    */
    void vkbOpened(bool state);

private slots:
    /**
     * Resizes the view when VKB is opened.
     * This slot is triggered when user taps on the CV editor
     */
    void vkbOpened();

    /**
     * Resizes the view when VKB is closed.
     * This slot is triggered when VKB focus is lost.
     */
    void vkbClosed();
    
private:
    /**
     * launch editor when attachment is inserted
     * @params data used for launching editor with pre-populated content
     */
    void launchUniEditor(const QVariantList& data);
    
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
     * Handle provisioning message
     * @param msgId message id
     */
    void handleProvisoningMsg(int msgId);
    
    /**
     * Handles short tap for vcard.
     */
    void handleShortTap();
    
    /**
     * helper method to show long/short tap context menu.
     */
    void showContextMenu(HbAbstractViewItem* viewItem,const QPointF& point, int placement);

    /**
     * Launches Dialer Service 
     */
    void call(const QString& address);

private:

    /**
     * List to hold the conversations
     * Owned
     */
    HbListView *mConversationList;

    /**
     * Model to facilitate filtering and sorting
     * Owned
     */
    QStandardItemModel *mMessageModel;

    /**
     * Instance of messaged editor widget.
     */
    MsgEditorWidget *mEditorWidget;

    /**
     * Instance of Contact Card widget.
     */
    MsgContactCardWidget* mContactCardWidget;
    
    /**
     * Send utils instance
     */
    MsgSendUtil *mSendUtil;
 
    /**
     * Instance of the main layout.
     */
    QGraphicsLinearLayout *mMainLayout;

    /*
     * Instance of VKB 
     */
    HbStaticVkbHost* mVkbHost;
    
    /**
     * Instance of Audio Fetcher Dialog
     * Need to show when attachment audio selected
     */
    MsgAudioFetcherDialog* mDialog;
    
    /**
     * variable holding the visible model index
     */
    QModelIndex mVisibleIndex;
    
    /**
     * Flag is set when model is populated.
     * @see signal conversationModelPopulated()
     */
    bool mModelPopulated;
    
    /**
     * Flag is set when 
     */
    bool mViewReady;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgConversationView;
#endif
};

#endif // MSG_CONVERSATION_VIEW_H
// EOF
