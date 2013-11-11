/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmapiheaders.h"




namespace EmailClientApi
{

NmApiMailboxSettingsDataPrivate::NmApiMailboxSettingsDataPrivate() :
    mId(0), mSettings(new QHash<int, QVariant>())
{
    NM_FUNCTION;
    Q_CHECK_PTR( mSettings );
}

NmApiMailboxSettingsDataPrivate::~NmApiMailboxSettingsDataPrivate()
{
    NM_FUNCTION;
}

/*!
 \fn setMailboxId 
 \param mailboxId - valid mailbox id as quint64.
 
 Sets the mailbox id               
 */
void NmApiMailboxSettingsDataPrivate::setMailboxId(quint64 mailboxId)
{
    NM_FUNCTION;
    mId = mailboxId;
}

/*!
 \fn mailboxId
 \return quint64 - the mailbox id.
 
 Gets the mailbox id.              
 */
quint64 NmApiMailboxSettingsDataPrivate::mailboxId() const
{
    NM_FUNCTION;
    return mId;
}

/*!
 \fn setValue 
 \param key - setting data to be set.
 \param settingValue - Value to be set as QVariant.

 Sets individual setting value.        
 */
void NmApiMailboxSettingsDataPrivate::setValue(int key, const QVariant &settingValue)
{
    NM_FUNCTION;
    (*mSettings)[key] = settingValue;
}

/*!
 \fn getValue
 \param key - setting data to get.
 \param settingValue - On return holds as QVariant.
 \return true if succesfull, false otherwise.
 
 Get individual setting value.        
 */
bool NmApiMailboxSettingsDataPrivate::getValue(int key, QVariant &settingValue) const
{
    NM_FUNCTION;
    if (mSettings->contains(key)) {
        settingValue = (*mSettings)[key];
        return true;
    }
    return false;
}

/*!
 \fn validateString
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - if this key was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateString(int key, QVariant& val, bool& validated) const
{
    NM_FUNCTION;
    bool ret = true;
    switch (key) {     
     case OutgoingPassword:
     case IncomingPassword:     
     case FolderPath: 
     case IncomingLoginName:
     case MailboxName:
     case MyName:
     case ReceptionActiveProfile:
     case IncomingMailServer:
     case OutgoingMailServer:
     case OutgoingLoginName:
     case EmailAddress:
     case ReplyAddress:
     case EmailAlias:               
     case IncomingMailUsesAuthentication:
     case OutgoingMailUsesAuthentication:
     case IncomingMailSecurityType:
     case OutgoingMailSecurityType:
     case EmailNotificationState:         
     case AlwaysOnlineState:{
         validated=true;
         if (val.type() != QVariant::String) {
             ret = false;
             break;
         }

         QString sz = val.toString();                   
         ret = validateStringValue(key, sz);
         break;
     }
         
     default: {
         validated = false;
         ret = false;
         break;
         }
    }    
    return ret;
}

/*!
 \fn validateStringValue
 \param key - setting data to validate.
 \param val - value to validate. 
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateStringValue(int key, QString& val) const
{
    NM_FUNCTION;
    int ret = true;
    bool valid = true, validated = false;
    
    valid = validateStringGeneral(key ,val, validated);
    if (validated && !valid) {
            ret = false;
    }
    
    if( !validated ) {
        valid = validateEmailAddress(key ,val, validated);
        if (validated && !valid) {
            ret = false;
        }
    }
    
    if( !validated ) {
        valid = validateUsesAuthentication(key ,val, validated);
        if (validated && !valid) {
            ret = false;
        }
    }
    
    if( !validated ) {
        valid = validateSecurityType(key ,val, validated);
        if (validated && !valid) {
            ret = false;
        }
    }
    
    if( !validated ) {
        valid = validateAOLState(key ,val, validated);
        if (validated && !valid) {
            ret = false;
        }
    }        
    return ret;
}

/*!
 \fn validateStringGeneral
 \param key - setting data to validate.
 \param val - value to validate.
 \param handled - true if method value was handled by method.
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateStringGeneral(int key, QString& /*val*/, bool& handled) const
{
    NM_FUNCTION;
    int ret = true;
    switch (key) {     
         case OutgoingPassword:
         case IncomingPassword:     
         case FolderPath:
         case IncomingLoginName:
         case MailboxName:
         case MyName:
         case ReceptionActiveProfile:
         case IncomingMailServer:
         case OutgoingMailServer:
         case OutgoingLoginName: {
             // Allready validated that values are string, 
             // otherwise method is not called
             handled = true;         
             break;
         }
         default: {
             handled = false;
             ret = false;
             break;
         }    
    } 
    return ret;
}
         
/*!
 \fn validateEmailAddress
 \param key - setting data to validate.
 \param val - value to validate.
 \param handled - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateEmailAddress(int key, QString& val, bool& handled) const
{
    NM_FUNCTION;
    int ret = true;
    switch (key) {        
        case EmailAddress:
        case ReplyAddress: {
            handled = true;
            if (val.size() == 0) {             
                 ret = false;
                 break;
            }
            if (!(val.contains("@"))) {
               ret = false;
            }
            break;
        }
        case EmailAlias:  {
            handled = true;            
            break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}
     
/*!
 \fn validateUsesAuthentication
 \param key - setting data to validate.
 \param val - value to validate.
 \param handled - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateUsesAuthentication(int key, QString& val, bool& handled) const
{ 
    NM_FUNCTION;
    int ret = true;
    switch (key) { 
        case IncomingMailUsesAuthentication:
        case OutgoingMailUsesAuthentication: {
            handled = true;
            if (val.size() == 0) {
                ret = false;
                break;
            }
            if (!(val.contains("none") ||
              val.contains("UserAuthentication") ||
                  (key == OutgoingMailUsesAuthentication &&
                   val.contains("SameAsIncoming")) )) {
                ret = false;
            }
            break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}
        
/*!
 \fn validateSecurityType
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateSecurityType(int key, QString& val, bool& handled) const
{ 
    NM_FUNCTION;
    int ret = true;
    switch (key) {
         case IncomingMailSecurityType:
         case OutgoingMailSecurityType: {
            handled = true;
            if (val.size() == 0) {
                ret = false;
                break;
            }
            if (!(val.contains("StartTls") || 
                  val.contains("SSLTls") || 
                  val.contains("none"))) {
                ret = false;
            }
            break;
         }
         default: {
             handled = false;
             ret = false;
             break;
         }
     }
     return ret;
}

/*!
 \fn validateAOLState
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual string type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateAOLState(int key, QString& val, bool& handled) const
{ 
    NM_FUNCTION;
    int ret = true;
    switch (key) {
        case EmailNotificationState:         
        case AlwaysOnlineState:{
         handled = true;
         if (val.size() == 0) {
              ret = false;
              break;
          }
          if (!( (val.contains("always") && key == AlwaysOnlineState) || 
                 (val.contains("automatic") && key == EmailNotificationState) || 
                  val.contains("homeOnly") || 
                  val.contains("off"))) {
              ret = false;
          }
          break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 \fn validateInteger
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual integer type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateInteger(int key, QVariant& val, bool& validated) const
{
    NM_FUNCTION;
    bool ret = true;
    switch (key) { 
        case DownloadPictures:
        case MessageDivider:
        case UserNameHidden:
        case FirstEmnReceived:
        case EmnReceivedNotSynced:
        case AoLastUpdateFailed:
        case AoUpdateSuccessfulWithCurSettings:
        case ReceptionUserDefinedProfile:
        case ReceptionInboxSyncWindow:
        case ReceptionGenericSyncWindowInMessages:
        case IncomingPort:
        case OutgoingPort:
        case ReceptionWeekDays:
        case ReceptionDayStartTime:
        case ReceptionDayEndTime:
        case ReceptionRefreshPeriodDayTime:
        case ReceptionRefreshPeriodOther: {
            validated = true;
            if (!(val.type() == QVariant::Int ||
                  val.type() == QVariant::UInt ||
                  val.type() == QVariant::LongLong  ||
                  val.type() == QVariant::ULongLong ||
                  val.type() == QVariant::Double)) {
                ret = false;
                break;
            }            
            int ival = val.toInt();
            ret = validateIntVal(key,ival);
            break;
        }
        default: {
            validated = false;
            ret = false;
            break;
        }
     }
     return ret;
}

/*!
 \fn validateIntVal
 \param key - setting data to validate.
 \param val - value to validate. 
 \return true if valid value, false otherwise.
 
 validates individual integer type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateIntVal(int key, int val) const
{
    NM_FUNCTION;
    int ret = true;
    bool valid = true, validated = false;

    valid = validateIntGeneral(key ,val, validated);
    if (validated && !valid) {
           ret = false;
    }
    
    if( !validated ) {
       valid = validateOnOffValue(key ,val, validated);
       if (validated && !valid) {
           ret = false;
       }
    }
    
    if( !validated ) {
       valid = validateWeekdayMask(key ,val, validated);
       if (validated && !valid) {
           ret = false;
       }
    }
    
    if( !validated ) {
       valid = validateHoursInDay(key ,val, validated);
       if (validated && !valid) {
           ret = false;
       }
    }
    return ret;
}

/*!
 \fn validateOnOffValue
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual integer type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateOnOffValue(int key, int val, bool& handled) const
{
    NM_FUNCTION;
    bool ret = true;
    switch (key) {   
        // Integer: 0=Off, 1=On    
        case DownloadPictures:
        case MessageDivider:
        case UserNameHidden:
        
        // Integer: 0=false,1=true
        case FirstEmnReceived:
        case EmnReceivedNotSynced:
        case AoLastUpdateFailed:
        case AoUpdateSuccessfulWithCurSettings:
        
        // Integer: 0=Disabled, 1=Enabled
        case ReceptionUserDefinedProfile: {
            handled = true;
            if (!(0 <= val && val <= 1)) {
                ret = false;
            }
            break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 \fn validateIntGeneral
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual integer type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateIntGeneral(int key, int /*val*/, bool& handled) const
{ 
    NM_FUNCTION;
    bool ret = true;
    switch (key) {         
        // Integer: 5,15,60,240,0="When open mailbox"
        case ReceptionRefreshPeriodDayTime:
        case ReceptionRefreshPeriodOther:
            
        // Integer: 0=All messages
        case ReceptionInboxSyncWindow:
        case ReceptionGenericSyncWindowInMessages:
            
        // Integer
        case IncomingPort:
        case OutgoingPort: {
            // Allready valid thate these are integers
            handled = true;
            break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 \fn validateWeekDayMask
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual integer type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateWeekdayMask(int key, int val, bool& handled) const
{
    NM_FUNCTION;
    bool ret = true;
    switch (key) {
        // Integer bitmask of weekdays: 0x01=Mon,0x02=Tue,0x04=Wed,0x08=Thu,0x10=Fri,0x20=Sat,0x40=Sun
        case ReceptionWeekDays: {
            handled = true;
            int wkdmask = Mon | Tue | Wed | Thu | Fri | Sat | Sun;
            if ((val & wkdmask) != val) {
                ret = false;
            }
            break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 \fn validateHoursInDay
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual integer type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateHoursInDay(int key, int val, bool& handled) const
{ 
    NM_FUNCTION;
    bool ret = true;
    switch (key) {
        // Integer: 0-23
        case ReceptionDayStartTime:
        case ReceptionDayEndTime: {
            handled = true;
            if (!(0 <= val && val <= 23)) { 
                ret = false;
            }
            break;
        }
        default: {
            handled = false;
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 \fn validateBool
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual bool type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateBool(int key, QVariant& val, bool& validated) const
{
    NM_FUNCTION;
    bool ret = true;
    switch (key) {    
        
        // Boolean
        case IncomingSecureSockets:
        case IncomingSSLWrapper:
        case UseOutgoingAuthentication:
        case OutgoingSecureSockets:
        case OutgoingSSLWrapper: {
            validated = true;
            if (val.type() != QVariant::Bool)
                ret = false;
            break;
        }
        default: {
            validated = false;
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 \fn validateDateTime
 \param key - setting data to validate.
 \param val - value to validate.
 \param validated - true if value was validated by the method
 \return true if valid value, false otherwise.
 
 validates individual QDateTime type value.        
 */
bool NmApiMailboxSettingsDataPrivate::validateDateTime(int key, QVariant& val, bool& validated) const
{
    NM_FUNCTION;
    switch(key) {
        case AoLastSuccessfulUpdate: {
            if (val.type() != QVariant::DateTime) {
                validated = true;
                return false;
            }
            return true;
        }
        default: {
            validated = false;
            return false;
        }
    }
}

/*!
 \fn validateData
 \return boolean - true, everything validated OK, false otherwise
 
 Validates data in this container. 
 */   
bool NmApiMailboxSettingsDataPrivate::validateData() const
{
    NM_FUNCTION;
    QHash<int, QVariant>::const_iterator i = mSettings->constBegin();
    while (i != mSettings->constEnd()) {
        
        bool validated = false;
        bool valid = false;        
        
        int key = i.key();
        QVariant val = i.value();
        
        ++i;
        
        valid = validateString(key ,val, validated);
        if (validated) {
            if (!valid){
                return valid;
            }
            continue;
        }
        
        valid = validateInteger(key ,val, validated);
        if (validated) {
            if (!valid){
                return valid;
            }
            continue;
        }
         
        valid = validateBool(key ,val, validated);
        if (validated) {
            if (!valid){
                return valid;
            }
            continue;
        } 
         
        valid = validateDateTime(key ,val, validated);
        if (validated) {
            if (!valid){
                return valid;
            }
            continue;
        }
     }
     return true;
}

QList<int> NmApiMailboxSettingsDataPrivate::listSettings() const
{
    NM_FUNCTION;
    return mSettings->keys();
}

/*!
 \fn clearSettings 
 
 Resets data in this container. 
 */
void NmApiMailboxSettingsDataPrivate::clearSettings()
{
	mSettings->clear();
}

}//end namespace
