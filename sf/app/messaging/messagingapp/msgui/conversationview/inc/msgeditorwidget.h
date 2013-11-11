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

#ifndef MSGEDITORWIDGET_H
#define MSGEDITORWIDGET_H

// SYSTEM INCLUDES
#include <HbWidget>
#include <HbLineEdit>

// FORWARD DECLARATIONS
class HbPushButton;
class HbFrameItem;
class HbTextItem;
class HbAction;
class UniEditorPluginLoader;
class UniEditorPluginInterface;
class UniEditorGenUtils;

/**
 * Custom editor class
 */
class MsgEditor : public HbLineEdit
{
Q_OBJECT
    
public:    
    /**
     * Constructor
     */
    MsgEditor(QGraphicsItem *parent = 0);
    
    /**
     * Foucus in Event method from  HbAbstractEdit
     */
    
protected:
    /**
     * reimplemented from base class.
     */
    void focusInEvent(QFocusEvent *event);    
    void focusOutEvent(QFocusEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);
    
signals:  
    /**
     * Signal emitted when the focus in event comes
     * i.e. when editor is tapped for reply
     */ 
     void replyStarted();
};

/**
 * This class is a custom layout widget for Msg Editor.
 */

class MsgEditorWidget : public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgEditorWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgEditorWidget();

public:
    /* Setter/Getter methods */

    /**
     * Returns the editor's contents.
     * @return Editor contents
     */
    QString content() const;

    /**
     * Set the editor's content.
     * @param contentText content text to set.
     */
    void setContent(const QString &contentText);

    /**
     * This function does initialisations needed for character counter
     */
    void setEncodingSettingsL();
   
signals:

    /**
     * Emitted when send button from virtual ITUT is pressed.
     */
    void sendMessage();

    /**
     * Signal emitted when the focus in event comes
     * i.e. when editor is tapped for reply
     */ 
    void replyStarted();

    /**
     * Signal emitted when the sms char limit is reached
     */ 
    void smsCharLimitReached();
    
public slots:

    /**
     * Clears the content.
     */
    void clear();
    
private slots:

    /**
     * Called when editor content is chaged. to enable/disable send button.
     */
    void onTextChanged(const QString& str);
    
    /**
     * called when send button is clicked.
     */
    void onClicked();

    /**
     * called the timer expires and handles the case when sms char limit is
     * reached
     */
	 void handleSmsCharLimitReached();
	 
	 /**
     * called when send button is pressed/released.
     */  
    void onPressed();
    void onReleased();
	
	/**
     * This slot is called when sms char limit reached dialog is launched.
     * @param action selected action (yes or no).
     */	
    void onSmsCharLimitReached(HbAction* action);

private:

    /**
     * Initialization function.
     */
    void init();
    
	 /**
	  * To update back ground.
	  */
    void updateButtonBackgroundAndColor(const QString& bg,const QString& iconColor);

private:

    /**
     * Instance of message edit.
     * Will be deleted automatically by parent.
     * Own.
     */
    MsgEditor *mMsgEditor;

    /**
     * Instance of push button.
     * Will be deleted automatically by parent.
     * Own.
     */
    HbPushButton *mSendButton;

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

    /**
     * Instance of UniEditorGenUtils
     * Own.
     */
    UniEditorGenUtils* mEditorUtils;

    /**
     * Indication for sms char limt reached
     */
    bool mSmsCharLimitReached;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgEditorWidget;
#endif
};

#endif // MSGEDITORWIDGET_H
// EOF
