/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Bio control for Provisioning documents.
*
*/


#include <CpMessage.h>
#include <HbMessageBox>
#include <CWPBioControl.h>

// ----------------------------------------------------------------------------
// CpMessage::CpMessage
// Description: Construtor
// ----------------------------------------------------------------------------

CpMessage::CpMessage(CWPBioControl* bioControl)
    {    
    iBioControl = bioControl;
    }

// ----------------------------------------------------------------------------
// CpMessage::~CpMessage
// Description: Destructor
// ----------------------------------------------------------------------------

CpMessage::~CpMessage()
    {    
    }

// ----------------------------------------------------------------------------
// CpMessage::inValidPinMessage
// Description: inValidPinMessage
// ----------------------------------------------------------------------------


void CpMessage::inValidPinMessage()
    {
    HbMessageBox *box = new HbMessageBox(hbTrId("code error"),HbMessageBox::MessageTypeWarning);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->open();
    }

// ----------------------------------------------------------------------------
// CpMessage::saveSettingMessage
// Description: saveSettingMessage
// ----------------------------------------------------------------------------


void CpMessage::saveSettingMessage()
    {
    HbMessageBox *box = new HbMessageBox(hbTrId("txt_device_update_info_settings_are_already_saved"),HbMessageBox::MessageTypeQuestion);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setTimeout(HbPopup::NoTimeout);
    box->clearActions(); 
		HbAction * action1 = new HbAction(hbTrId("txt_common_button_save"));
		HbAction * action2 = new HbAction(hbTrId("txt_common_button_cancel"));	
		box->addAction(action1);
		box->addAction(action2);	
    box->open(this,SLOT(saveSettingDialogClosed(HbAction*)));
    }

// ----------------------------------------------------------------------------
// CpMessage::saveSettingDialogClosed
// Description: saveSettingDialogClosed
// ----------------------------------------------------------------------------

void CpMessage::saveSettingDialogClosed(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0))
    {
      iBioControl->DoSaveL();
      iBioControl->SaveSettingStatus(); 
    }    
 }

// ----------------------------------------------------------------------------
// CpMessage::settingNotSaved
// Description: settingNotSaved
// ----------------------------------------------------------------------------


void CpMessage::settingNotSaved()
    {
    HbMessageBox *box = new HbMessageBox(hbTrId("txt_device_update_info_settings_could_not_be_saved"),HbMessageBox::MessageTypeInformation);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setTimeout(HbPopup::NoTimeout);
    //box->open(this,SLOT(saveSettingDialogClosed(HbAction*)));
    box->open();
    }
    
// ----------------------------------------------------------------------------
// CpMessage::setAsDefault
// Description: setAsDefault
// ----------------------------------------------------------------------------

void CpMessage::setAsDefault()
    {
    HbMessageBox *box = new HbMessageBox(hbTrId("txt_device_update_info_set_the_received_settings"),HbMessageBox::MessageTypeQuestion);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setTimeout(HbPopup::NoTimeout);
    box->clearActions(); 
		HbAction * action1 = new HbAction(hbTrId("txt_device_update_button_set_as_default"));
		HbAction * action2 = new HbAction(hbTrId("txt_common_button_cancel"));	
		box->addAction(action1);
		box->addAction(action2);	
    box->open(this,SLOT(setAsDefaultDialogClosed(HbAction*)));
        
    }

// ----------------------------------------------------------------------------
// CpMessage::setAsDefaultDialogClosed
// Description: setAsDefaultDialogClosed
// ----------------------------------------------------------------------------

void CpMessage::setAsDefaultDialogClosed(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0))
    {
      iBioControl->saveDefaultSetting();      
    }    
 }    

// ----------------------------------------------------------------------------
// CpMessage::autheticationFailure
// Description: autheticationFailure
// ----------------------------------------------------------------------------

void CpMessage::autheticationFailure()
    {
        HbMessageBox *box = new HbMessageBox(hbTrId("txt_device_update_info_verification_failed_and_mes"),HbMessageBox::MessageTypeWarning);
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->setTimeout(HbPopup::NoTimeout);
        box->open(this,SLOT(autheticationFailureDialogClosed(HbAction*)));                    
    }

// ----------------------------------------------------------------------------
// CpMessage::autheticationFailureDialogClosed
// Description: autheticationFailureDialogClosed
// ----------------------------------------------------------------------------

void CpMessage::autheticationFailureDialogClosed(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0))
    {
      iBioControl->authenticationFailed();      
    }    
 }    

// ----------------------------------------------------------------------------
// CpMessage::senderUnknown
// Description: senderUnknown
// ----------------------------------------------------------------------------

void CpMessage::senderUnknown()
    {
    HbMessageBox *box = new HbMessageBox(hbTrId("txt_device_update_info_ender_of_the_message_is_unt"),HbMessageBox::MessageTypeQuestion);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setTimeout(HbPopup::NoTimeout);
    box->clearActions(); 
		HbAction * action1 = new HbAction(hbTrId("txt_common_button_continue"));
		HbAction * action2 = new HbAction(hbTrId("txt_common_button_cancel"));	
		box->addAction(action1);
		box->addAction(action2);	
    box->open(this,SLOT(senderUnknownDialogClosed(HbAction*)));
    }

// ----------------------------------------------------------------------------
// CpMessage::senderUnknownDialogClosed
// Description: senderUnknownDialogClosed
// ----------------------------------------------------------------------------

void CpMessage::senderUnknownDialogClosed(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0))
    {
      iBioControl->senderUnknownContinue();      
    }    
    else
    {
      iBioControl->senderUnknownCancel();      
    }
 }    
