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
 * class to manage differnt messaging views.
 *
 */

#ifndef MSGVIEWMANAGER_H_
#define MSGVIEWMANAGER_H_

#include <QObject>
#include <QVariantList>
#include <QVariant>

class HbMainWindow;
class MsgUnifiedEditorView;
class MsgListView;
class MsgConversationBaseView;
class UnifiedViewer;
class MsgBaseView;
class DraftsListView;
class MsgSettingsView;
class HbAction;
class HbView;

class MsgViewManager: public QObject
{
Q_OBJECT

public:
    /**
     * constructor
     */
    MsgViewManager(bool serviceRequest, HbMainWindow* mainWindow, 
                   QObject* parent = 0,int activityMsgId = -1);

    /**
     * Destructor.
     */
    ~MsgViewManager();

    /**
     * call back function will be called from MsgServiceInterface.
     */
    void send(const qint32 contactId, const QString phoneNumber, const QString displayName);

    /**
     * call back function called from MsgServiceInterface to set service request flag.
     */
    void setServiceRequest(bool request);

    /**
     * call back function called MsgServiceInterface.
     */
    void open(qint64 conversationId);

    /**
     * call back function called MsgServiceInterface.
     */
    void send(QVariant data);

    /**
     * depricated api. will be removed in future.
     */
    void openEditor(QString number, QString name);

    /**
     * call back function will be called from MsgServiceInterface.
     */
    void send(const QString phoneNumber, const QString alias, const QString bodyText);

    /**
     * Opens the viewer to view the message.
     * @param msgId message id of the message.
     */
    void view(int msgId);

    /**
     * Returns the current active view.
     */
    int currentView();
	
    /**
	 * Saves the content of editor or Cv to draft.
	 */
    int saveContentToDraft();

    /**
     * Handles the HW key events.
     * @param key key type.
     * @return true if key event handled else false.
     */
    bool handleKeyEvent(int key);

private:
    /**
     * swiches back to last view after service request is complete.
     */
    void switchToLastSavedView();

    /**
     * find conversation id corresponding to given phone no.
     * @param phoneNum
     */
    qint64 findConversationId(const QString phoneNum);

    /**
     * helper method to complete back action.
     */
    void completeServiceRequest();

    /*
     * switch to clv.
     */
    void switchToClv(const QVariantList& data);

    /*
     * switch to cv.
     */
    void switchToCv(const QVariantList& data);

    /*
     * switch to dlv.
     */
    void switchToDlv(const QVariantList& data);

    /*
     * switch to unieditor.
     */
    void switchToUniEditor(const QVariantList& data);

    /*
     * switch to univiewer.
     */
    void switchToUniViewer(const QVariantList& data);

    /*
     * switch to settings.
     */
    void switchToMsgSettings(const QVariantList& data);

    /*
     * handle other cases.
     */
    void handleDefault(const QVariantList& data);

    /**
     * handle sms and mms messge
     * @param msgId message id
     */
    void handleSmsMmsMsg(int msgId);

    /**
     * handle ringtone message
     * @param msgId message id
     */
    void handleRingtoneMsg(int msgId);

    /**
     * handle provisioning message
     * @param msgId message id
     */
    void handleProvisoningMsg(int msgId);
    
	/**
	* Appends the views to be deleted into a QList to be deleted when view is ready
	*/
    void appendViewToBeDeleted(HbView* view);

	 /**
	  * Save the editor data to be populated
	  * @param editorData QVariantList
	  */    
    void populateUniEditorAfterViewReady(const QVariantList& editorData);
    
    /**
	 * opens unieditor as activity.
	 * @param activityMsgId activity msg id.
	 */
	void openUniEditorActivity(int activityMsgId);

    /**
    * find contact id corresponding to given phone no.
    * @param phoneNum
    */
    qint32 findContactId(const QString address);
    
private slots:
    /**
     * this slot is called on mainwindows back action.
     */
    void onBackAction();

    /**
     * This slot is called when switchView signal is emitted from a view.
     */
    void switchView(const QVariantList& data);

    /**
     * This slot is called when viewReady signal is emitted from main window.
     */
    void setViewInteractive();

    /**
     * Slot to delete previous view instances on view switch
     */
    void deletePreviousView();
	
	/**
     * This slot is called when delete message dialog is launched.
     * @param action selected action (yes or no).
     */	
    void onDialogDeleteMsg(HbAction* action);
    
	/**
     * This slot is called when save tone dialog is launched.
     * @param action selected action (yes or no).
     */	
    void onDialogSaveTone(HbAction* action);
    
    /**
     * When this slot is called the saved editor data is set to the editor
     */
    void populateUniEditorView();
	
private:
    /**
     * main window reference not owned.
     */
    HbMainWindow* mMainWindow;

    /**
     * different views.
     */
    MsgUnifiedEditorView* mUniEditor;
    MsgListView* mListView;
    MsgConversationBaseView* mConversationView;
    UnifiedViewer* mUniViewer;
    DraftsListView* mDraftsListView;
    MsgSettingsView* mSettingsView;
    
    HbAction* mBackAction;

    int mPreviousView;
    int mCurrentView;
    int mViewAtServiceRequest;
    bool mServiceRequest;
    qint64 mConversationId;
    bool mViewServiceRequest;
    QList<HbView*> mViewTobeDeleted;
    HbView* mDummyview;
    int mMessageId;
    
    QVariantList mEditorData;
    QVariantList mViewerData;
};

#endif /* MSGVIEWMANAGER_H_ */
