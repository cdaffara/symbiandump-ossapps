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

#include "msgeditorwidget.h"

// SYSTEM INCLUDES
#include <HbStyle>
#include <HbPushButton>
#include <HbTextItem>
#include <qgraphicsscene.h>
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <HbAction>
#include <csmsaccount.h>
#include <smutset.h>
#include <hbmessagebox.h>
#include <hbcolorscheme.h>
#include <QColor>
#include <QTimer>
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
#include "UniSendingSettings.h"
#include "UniEditorGenUtils.h"
#include "debugtraces.h"

// LOCAL CONSTANTS

const QString SEND_ICON("qtg_mono_send");
const QString BACKGROUND("qtg_fr_input_v_bg");
const QString BACKGROUND_FRAME("qtg_fr_messaging_char_count");
const QString CHAR_COUNTER_COLOR("qtc_messaging_char_count");

const QString SEND_BUTTON_NORMAL("qtg_fr_btn_green_normal");
const QString SEND_BUTTON_PRESSED("qtg_fr_btn_green_pressed");
const QString SEND_BUTTON_DISABLED("qtg_fr_btn_disabled");

const QString SEND_BUTTON_NORMAL_COLOR("qtc_callhandling_answer_normal");
const QString SEND_BUTTON_PRESSED_COLOR("qtc_callhandling_answer_pressed");
const QString SEND_BUTTON_DISABLED_COLOR("qtc_button_disabled");

#define LOC_SMS_CHAR_LIMIT_REACHED hbTrId("txt_messaging_dialog_sms_character_count_exceeded")
#define LOC_HINT_TEXT hbTrId("txt_messaging_formlabel_enter_message_here")

  
const TInt KShowCounterLimit = 10;

QGraphicsItem* FOCUSITEM = 0;

//---------------------------------------------------------------
// MsgEditorWidget::MsgEditorWidget
// @see header
//---------------------------------------------------------------
MsgEditorWidget::MsgEditorWidget(QGraphicsItem *parent) :
    HbWidget(parent), mMsgEditor(0), mSendButton(0), mPluginInterface(0), mPluginLoader(0),
        mSmsCharLimitReached(false)
{
    //setting background.
    HbFrameItem* backGround = new HbFrameItem(this);
    backGround->frameDrawer().setFrameGraphicsName(BACKGROUND);
    backGround->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesVertical);
    backGround->frameDrawer().setFillWholeRect(true);
    this->setBackgroundItem(backGround); 

    // Initialize the widget.
    init();
}

//---------------------------------------------------------------
// MsgEditorWidget::init
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::init()
{
    // Create mandatory element of mesh layout.
    mMsgEditor = new MsgEditor(this);
    mMsgEditor->setMaxRows(3); // NOTE: Don't remove this line.
    HbStyle::setItemName(mMsgEditor, "msgEditor");

	mSendButton = new HbPushButton(this); 
    HbStyle::setItemName(mSendButton, "sendButton"); 
    mSendButton->setEnabled(false); 
    
    HbFrameItem* backGround = new HbFrameItem(this); 
    backGround->frameDrawer().setFrameType(HbFrameDrawer::NinePieces); 
    mSendButton->setBackgroundItem(backGround); 
    updateButtonBackgroundAndColor(SEND_BUTTON_DISABLED,SEND_BUTTON_DISABLED_COLOR); 
    mSendButton->setIcon(HbIcon(SEND_ICON));
    
    mCharCounter = new HbTextItem(this);
    HbStyle::setItemName(mCharCounter, "charCounter");
    mCharCounter->setZValue(1.5);
    
    QColor color = HbColorScheme::color(CHAR_COUNTER_COLOR);
    mCharCounter->setTextColor(color);
    
    mBackgroundItem = new HbFrameItem(this);
    HbStyle::setItemName(mBackgroundItem, "charCounterFrame");

    mBackgroundItem->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesHorizontal);
    mBackgroundItem->frameDrawer().setFillWholeRect(true);
    
    mBackgroundItem->frameDrawer().setFrameGraphicsName(
        BACKGROUND_FRAME);    
    
    //Create editor utils object
    mEditorUtils = q_check_ptr(new UniEditorGenUtils());

    FOCUSITEM = mSendButton;


    connect(mSendButton, SIGNAL(clicked()),this, SLOT(onClicked()));
    connect(mSendButton, SIGNAL(pressed()),this, SLOT(onPressed()));
    connect(mSendButton, SIGNAL(released()),this, SLOT(onReleased()));
    connect(mMsgEditor, SIGNAL(replyStarted()),this, SIGNAL(replyStarted()));
    connect(mMsgEditor, SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)));
    
}

//---------------------------------------------------------------
// MsgEditorWidget::~MsgEditorWidget
// @see header
//---------------------------------------------------------------
MsgEditorWidget::~MsgEditorWidget()
{
    delete mEditorUtils;
}

//---------------------------------------------------------------
// MsgEditorWidget::content
// @see header
//---------------------------------------------------------------
QString MsgEditorWidget::content() const
{
    return mMsgEditor->text();
}

//---------------------------------------------------------------
// MsgEditorWidget::setContent
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::setContent(const QString &text)
{
    mMsgEditor->setText(text);
}

//---------------------------------------------------------------
// MsgEditorWidget::clear
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::clear()
{
    mMsgEditor->setText(QString());
    mMsgEditor->setCursorVisibility(Hb::TextCursorHidden);
}

//---------------------------------------------------------------
// MsgEditorWidget::onTextChanged
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onTextChanged(const QString& str)
{
    //If char limit is already reached we are about to show the sms limit dialog
    //So we need not process the additional character entered
    if(mSmsCharLimitReached)
    {
        return;
    }
    if(str.length() > 0 )
        {
        if(!mSendButton->isEnabled())
            {
            mSendButton->setFocusProxy(mMsgEditor);
            mSendButton->setEnabled(true);
            updateButtonBackgroundAndColor(SEND_BUTTON_NORMAL,SEND_BUTTON_NORMAL_COLOR);
            }
        }
    else
        {
        if(mSendButton->isEnabled())
            {
            mSendButton->setFocusProxy(0);
            mSendButton->setEnabled(false);
            updateButtonBackgroundAndColor(SEND_BUTTON_DISABLED,SEND_BUTTON_DISABLED_COLOR);
            }
        }
    
    //Check done for optimization
    //Only if content is deleted we need to call encodingsettings again
    if (mPrevBuffer.isEmpty() || str.size() <= mPrevBuffer.size())
    {
        mPluginInterface->setEncodingSettings(EFalse,ESmsEncodingNone,mCharSupportType);
    }
    
    TInt numOfRemainingChars;
    TInt numOfPDUs;
    TBool unicodeMode;
    TSmsEncoding alternativeEncodingType;
    QString string = str;
    mPluginInterface->getNumPDUs(string, numOfRemainingChars, numOfPDUs,
        unicodeMode, alternativeEncodingType);

    //Set char counter value
    QString display = QString("%1(%2)").arg(numOfRemainingChars).arg(numOfPDUs);
    mCharCounter->setText(display);
    
    if(numOfPDUs > 1 || numOfRemainingChars <= KShowCounterLimit  )
    {
        mCharCounter->setVisible(true);
        mBackgroundItem->setVisible(true);
    }
    else
    {
        mCharCounter->setVisible(false);
        mBackgroundItem->setVisible(false);
    }
    
    //Check if sms segment limit has been reached
    bool unicode = (unicodeMode) ? true : false;
    int contentSize = mEditorUtils->UTF8Size(string);
    int maxSmsSize =  0;
    TRAP_IGNORE(maxSmsSize = mEditorUtils->MaxSmsMsgSizeL(unicode));
    
    if(contentSize > maxSmsSize)
    {        
        //Give breathing time for ITUT to complete its processing
        //without this there is a crash as ITUT cant complete its processing
        mSmsCharLimitReached = true;
        QTimer::singleShot(50, this, SLOT(handleSmsCharLimitReached()));   
        if(mPrevBuffer.isEmpty())
        {
            //Chop the last char and set it to the editor
            string.chop(1);
            mPrevBuffer = string;
        }
    }
    else
    {                
        //Save the previous buffer
        mPrevBuffer = string;        
    }
}

//---------------------------------------------------------------
// MsgEditorWidget::handleSmsCharLimitReached
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::handleSmsCharLimitReached()
{
    mSmsCharLimitReached = false;
    
    HbMessageBox::question(LOC_SMS_CHAR_LIMIT_REACHED,
                           this,SLOT(onSmsCharLimitReached(HbAction*)),
                           HbMessageBox::Ok | HbMessageBox::Cancel);


}

//---------------------------------------------------------------
// MsgEditor::onClicked
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onClicked()
    {    
    mSendButton->setFocusProxy(0);
    mMsgEditor->setFocusProxy(mSendButton);
    
    this->scene()->clearFocus();
    this->scene()->setFocusItem(mSendButton);
    
    mMsgEditor->setFocusProxy(0);

    mMsgEditor->setCursorVisibility(Hb::TextCursorHidden);
    
    emit sendMessage();
    }

//---------------------------------------------------------------
// MsgEditor::updateButtonBackgroundAndColor
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::updateButtonBackgroundAndColor(const QString& bg,const QString& iconColor) 
    { 
    HbFrameItem* drawer = static_cast<HbFrameItem*>(mSendButton->backgroundItem()); 
    if(drawer) 
        { 
        drawer->frameDrawer().setFrameGraphicsName(bg);   
        } 
    QColor color = HbColorScheme::color(iconColor);
    
    if(color.isValid())
    	 {
    	 mSendButton->icon().setColor(color);
    	 }	    
    } 
    

//---------------------------------------------------------------
// MsgEditor::onPressed
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onPressed()
    {
    updateButtonBackgroundAndColor(SEND_BUTTON_PRESSED,SEND_BUTTON_PRESSED_COLOR);
    }

//---------------------------------------------------------------
// MsgEditor::onReleased
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onReleased()
    {
    updateButtonBackgroundAndColor(SEND_BUTTON_NORMAL,SEND_BUTTON_NORMAL_COLOR);
    }
	
//---------------------------------------------------------------
// MsgEditor::setEncodingSettingsL
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::setEncodingSettingsL()
{ 
    if( mPluginLoader )
    {
        delete mPluginLoader;
    }
    mPluginLoader = new UniEditorPluginLoader(this);

    mPluginInterface =
                        mPluginLoader->getUniEditorPlugin(ConvergedMessage::Sms);    

    QT_TRAP_THROWING( CSmsSettings* settings = CSmsSettings::NewLC();
        CSmsAccount* account = CSmsAccount::NewLC();
        account->LoadSettingsL( *settings );

        if( settings->CharacterSet() == TSmsDataCodingScheme::ESmsAlphabetUCS2)
        {
            mCharSupportType = TUniSendingSettings::EUniCharSupportFull;
        }
        else
        {
            mCharSupportType = TUniSendingSettings::EUniCharSupportReduced;
        }

        CleanupStack::PopAndDestroy( account );
        CleanupStack::PopAndDestroy( settings ));

    //Set the mPrevBuffer to NULL initially
    mPrevBuffer = QString();
    
    mSmsCharLimitReached = false;
    
    mCharCounter->setVisible(false);
    mBackgroundItem->setVisible(false);
}

//---------------------------------------------------------------
// MsgEditor::MsgEditor
// @see header
//---------------------------------------------------------------
MsgEditor::MsgEditor(QGraphicsItem *parent)
:HbLineEdit(parent)
    {
     this->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
     this->setPlaceholderText(LOC_HINT_TEXT);
    }

//---------------------------------------------------------------
// MsgEditor::focusInEvent
// @see header
//---------------------------------------------------------------
void MsgEditor::focusInEvent(QFocusEvent *event)
	{
	HbLineEdit::focusInEvent(event);
    FOCUSITEM->setFocusProxy(this);
    setCursorVisibility(Hb::TextCursorVisible);
    emit replyStarted();    
	}

//---------------------------------------------------------------
// MsgEditor::focusOutEvent
// @see header
//---------------------------------------------------------------
void MsgEditor::focusOutEvent(QFocusEvent * event)
    {
    FOCUSITEM->setFocusProxy(0);
    setCursorVisibility(Hb::TextCursorHidden);
    HbLineEdit::focusOutEvent(event);  
    }

//---------------------------------------------------------------
// MsgEditor::inputMethodEvent
// @see header
//---------------------------------------------------------------
void MsgEditor::inputMethodEvent(QInputMethodEvent *event)
{
    /*
     * HbLineEdit is replacing '\n' chars hence
     * calling base class handler.
     */
    HbAbstractEdit::inputMethodEvent(event);
}

//---------------------------------------------------------------
// MsgEditor::onSmsCharLimitReached
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onSmsCharLimitReached(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {

        //Launch UniEditor 
        emit smsCharLimitReached();
    }
    else {
        //Set the previous content
        setContent( QString(mPrevBuffer));
    }
}

// EOF
