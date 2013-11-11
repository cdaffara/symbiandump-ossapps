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
 * Description: Manages differnt messaging views.
 * 
 *
 */

#ifndef MSGSERVICEVIEWMANAGER_H_
#define MSGSERVICEVIEWMANAGER_H_

#include <QObject>
#include <QVariant>
#include <HbEffect>
#include <convergedmessage.h>

class QGraphicsItem;
class QGraphicsPixmapItem;
class HbMainWindow;
class MsgUnifiedEditorView;
class UnifiedViewer;
class MsgBaseView;
class MsgSettingsView;
class HbAction;
class MsgStoreHandler;

class MsgServiceViewManager: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgServiceViewManager(MsgStoreHandler* storeHandler,
            HbMainWindow* mainWindow, QObject* parent = 0);

    /**
     * Destructor.
     */
    ~MsgServiceViewManager();

    /**
     * Send message.
     * @param phoneNumber phone number.
     * @param contactId contactId.
     * @param displayName displayname.
     */
    void send(const QString phoneNumber, 
              const qint32 contactId, 
              const QString displayName);

    /**
     * Send message.
     * @param phoneNumber phone number.
     * @param alias alias.
     * @param bodyText body text.
     */
    void send(const QString phoneNumber, 
              const QString alias, 
              const QString bodyText);
              
    /**
     * Send message.
     * @param addressList list of phone numbers and displaynames. 
     * key is the phone number and value is the display name.
     * @param bodyText body text.
     */
    void send(const QVariantMap addressList, 
              const QString bodyText = QString());

    /**
     * Send message.
     * @param data data to be sent.
     */
    void send(QVariant data);
	
    /**
     * Send message.
     */
    void send(ConvergedMessage message);
	
    /**
     * Opens the viewer to view the message.
     * @param msgId message id of the message.
     */
    void view(int msgId);

    /**
     * Handles the HW key events.
     * @param key key type.
     * @return true if key event handled else false.
     */
    bool handleKeyEvent(int key);
    
    /**
     * Save editor contents to drafts
     */
    void saveContentToDraft();

private:
    /*
     * Switch to unieditor.
     * @param editorData editor data.
     */
    void switchToUniEditor(const QVariantList& editorData);

    /*
     * Switch to settings view.
     */
    void switchToMsgSettings(const QVariantList& data);

    /**
     * Show information popup dialog
     * @param dispText, text to be displayed
     */
    void showPopup(const QString& dispText);

    /**
     * Handle Draft messages
     * @param msgId, message id
     * @param msgType, type of message e.g. sms or mms etc
     */
    void handleDraftMsg(int msgId, int msgType);

    /**
     * Handle Business Card messages (for view)
     * @param msgId, message id
     */
    void handleVCardMsg(int msgId);

    /**
     * Handle sms and mms message
     * @param msgId, message id
     */
    void handleSmsMmsMsg(int msgId);

    /**
     * Handle MMS notification message
     * @param msgId, message id
     */
    void handleMmsNotification(int msgId);

    /**
     * Handle ringtone message
     * @param msgId message id
     */
    void handleRingtoneMsg(int msgId);

    /**
     * Handle provisioning message
     * @param msgId message id
     */
    void handleProvisoningMsg(int msgId);

    /**
     * Handle bluetooth message
     * @param msgId message id
     */
    void handleBTMessage(int msgId);

    /**
     * Start a custom effect animation
     * e.g. editor's send effect
     * @param effectEvent, effect type for animation
     */
    void startAnimation(QString effectEvent);
    
    /**
     * Reset a custom effect animation
     * e.g. editor's send effect
     * @param effectEvent, animation effect type which needs to be reset
     * @param item, QGraphicsItem on which the effect was run
     */
    void resetAnimation(QString effectEvent, QGraphicsItem* item);
    
    /**
     * Grab screenshot for animation purpose
     * e.g. editor's send effect animation
     * @return QGraphicsPixmapItem, item on which animation will run
     */
    QGraphicsPixmapItem *screenShot();
    
    /**
     * find and return animation file for an effect
     * @param effectEvent, effect for which file definition needs to be found
     * @return QString, animation definition file path
     */
    QString getAnimationFile(QString effectEvent);

private slots:
    /**
     * This slot is called on mainwindows back action.
     */
    void onBackAction();

    /**
     * This slot is called when switchView signal is emitted from a view.
     * @param data data required to switch the views.
     */
    void switchView(const QVariantList& data);

    /**
     * This slot sets the current view on view-ready signal of mainwindow.
     * This is needed to show the default transitions correctly.
     */
    void showOnViewReady();
    
    /**
     * Handle a custom effect animation complete signal
     * e.g. editor's send effect complete
     * @param status, status information of the effect that has finished
     */
    void onAnimationComplete(const HbEffect::EffectStatus &status);
    
private:
    /**
     * Main window pointer. 
     * Not owned.
     */
    HbMainWindow* mMainWindow;

    /**
     * Unified editor. 
     * Owned.
     */
    MsgUnifiedEditorView* mUniEditor;
    
    /**
     * Unified viewer. 
     * Owned.
     */
    UnifiedViewer* mUniViewer;
    
    /**
     * Settings view. 
     * Owned.
     */
    MsgSettingsView* mSettingsView;
    
    /**
     * Back action 
     * Owned.
     */
    HbAction* mBackAction;

    /**
     * Store handler.
     * Not Owned.
     */
    MsgStoreHandler* mStoreHandler;
    
    /**
     * Current view value.
     */
    int mCurrentView;    
};

#endif /* MSGSERVICEVIEWMANAGER_H_ */
