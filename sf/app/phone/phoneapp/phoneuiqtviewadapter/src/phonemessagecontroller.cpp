/*!
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
* Description:  Handles message editor.
*/

#include "phonemessagecontroller.h"
#include "tphonecmdparamsfidata.h"
#include "phoneconstants.h"
#include "cphonecenrepproxy.h"
#include "cphonepubsubproxy.h"
#include "phoneapplauncher.h"
#include <settingsinternalcrkeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <hbglobal.h>


PhoneMessageController::PhoneMessageController(PhoneAppLauncher &appLauncher, QObject *parent) : 
    QObject(parent),
    m_appLauncher(appLauncher)
{

}

PhoneMessageController::~PhoneMessageController()
{
}

void PhoneMessageController::openSoftRejectMessageEditor(
        TPhoneCommandParam *commandParam)
{
    TPhoneCmdParamSfiData* sfiParam =
        static_cast<TPhoneCmdParamSfiData*>( commandParam );
    
    QString toField;
    QString alias;
    QString messageBody;
    
    
    // Get message body
    TInt softRejectDefaultInUseValue = 0;
    const TInt err = CPhoneCenRepProxy::Instance()->GetInt(
        KCRUidTelephonySettings,
        KSettingsSoftRejectDefaultInUse,
        softRejectDefaultInUseValue );
    
    if (softRejectDefaultInUseValue) {
        HBufC* softRejectTxt = NULL;
        TRAP_IGNORE( softRejectTxt = HBufC::NewL( KPhoneSMSLength ) );
        
        if (softRejectTxt) {
            TPtr string( softRejectTxt->Des() );
        
            // Default txt used or not
            CPhoneCenRepProxy::Instance()->GetString(
                KCRUidTelephonySettings,
                KSettingsSoftRejectText,
                string );
            
            messageBody = QString::fromUtf16(string.Ptr(), string.Length());
            delete softRejectTxt;
        }
    } else {
        messageBody = hbTrId("txt_phone_setlabel_soft_reject_val_default_text");
    }
    
    // store the remote caller's info
    alias = QString::fromUtf16(sfiParam->Name().Ptr(), 
            sfiParam->Name().Length());
    
    toField = QString::fromUtf16(sfiParam->Number().Ptr(), 
            sfiParam->Number().Length());

    
    // open message editor
    m_appLauncher.launchMessaging(toField, alias, messageBody);
}
