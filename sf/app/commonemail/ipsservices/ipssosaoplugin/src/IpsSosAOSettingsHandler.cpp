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
*
*/

#include "ipssosaopluginheaders.h"

const TInt KAOMinutesInHour = 60;
   
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSettingsHandler* CIpsSosAOSettingsHandler::NewL( 
        CMsvSession& aSession,
        TMsvId aMailboxId )
    {
    CIpsSosAOSettingsHandler* self = new(ELeave)CIpsSosAOSettingsHandler(aSession);
    CleanupStack::PushL(self);
    self->ConstructL( aMailboxId );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSettingsHandler::CIpsSosAOSettingsHandler(CMsvSession& aSession) :
        iSession(aSession)
    {
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSettingsHandler::~CIpsSosAOSettingsHandler()
    {
    delete iExtMgr;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::ConstructL( TMsvId aMailboxId )
    {
    TMsvId dummy;    
    iSession.GetEntry(aMailboxId, dummy, iEntry);
    
    iExtMgr = new NmIpsSosAoExtendedSettingsManager(MboxId());
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOSettingsHandler::GetFSPluginId() const
    {
    TInt pluginId = 0;
    if ( iEntry.iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        pluginId = IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID;
        }
    else if ( iEntry.iMtm.iUid == KSenduiMtmPop3UidValue )
        {
        pluginId = IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID;
        }
    return pluginId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
NmId CIpsSosAOSettingsHandler::MboxId() const
    {
    NmId id;
    id.setId32(iEntry.Id());
    id.setPluginId32(GetFSPluginId());
    return id;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::GetSubscribedImapFoldersL( 
            TMsvId /*aServiceId*/, 
            RArray<TMsvId>& /*aFoldersArray*/ )
    {
    //folders not supported
    }
    
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::ConstructImapPartialFetchInfo( 
        TImImap4GetPartialMailInfo& aInfo, 
        CImImap4Settings& aImap4Settings )
    {
    TInt sizeLimit = aImap4Settings.BodyTextSizeLimit();
    
    if ( sizeLimit == KIpsHeadersOnly )
        {
        aInfo.iTotalSizeLimit = KIpsHeadersOnly;
        }
    else if ( sizeLimit == KIpsFullBodyAndAttas )
        {        
        aInfo.iTotalSizeLimit = KMaxTInt;
        aInfo.iAttachmentSizeLimit = KMaxTInt;
        aInfo.iBodyTextSizeLimit = KMaxTInt;
        aInfo.iMaxEmailSize = KMaxTInt;
        aInfo.iPartialMailOptions = ENoSizeLimits;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
        }
    else if ( sizeLimit == KIpsFullBodyOnly )
        {
        aInfo.iTotalSizeLimit = KMaxTInt; 
        aInfo.iAttachmentSizeLimit = 0;
        aInfo.iBodyTextSizeLimit = KMaxTInt;
        aInfo.iMaxEmailSize = KMaxTInt;
        aInfo.iPartialMailOptions = EBodyAlternativeText;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
        }
    else
        {
        aInfo.iTotalSizeLimit = sizeLimit*1024; 
        // set zero when it not documentated does total size overrides these 
        aInfo.iAttachmentSizeLimit = 0;
        aInfo.iMaxEmailSize = sizeLimit*1024;
        aInfo.iBodyTextSizeLimit = sizeLimit*1024;
        aInfo.iPartialMailOptions = EBodyAlternativeText;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
        }
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TIpsSetDataAoStates 
    CIpsSosAOSettingsHandler::AlwaysOnlineState() const
    {
    TInt prof = IpsServices::EmailSyncProfileManualFetch;
    IpsServices::TIpsSetDataAoStates ret = IpsServices::EMailAoOff;
    
    QVariant state;
    TBool ok = GetSettingValue(IpsServices::ReceptionActiveProfile, state);
    
    if( ok )
        {
        prof = state.toInt();        
        ret = ProfileIntoAOState(prof);
        }   
    
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetAlwaysOnlineState(
    const IpsServices::TIpsSetDataAoStates aAlwaysOnlineState )
    {
    //we're allowed to switch ourselves off, not on.
    if ( aAlwaysOnlineState == IpsServices::EMailAoOff )
        {
        if(!SetSettingValue(
                IpsServices::ReceptionActiveProfile, 
                IpsServices::EmailSyncProfileManualFetch))
            {
        
            CreateKeyValuePair(
                    IpsServices::ReceptionActiveProfile, 
                    IpsServices::EmailSyncProfileManualFetch);
            }
        }
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TIpsSetDataEmnStates 
    CIpsSosAOSettingsHandler::EmailNotificationState() const
    {
    IpsServices::TIpsSetDataEmnStates ret = IpsServices::EMailEmnOff;

    /* Not supported yet
    QVariant state = NULL;
    TBool ok = GetSettingValue(IpsServices::EmailNotificationState, state);
        
    if(ok)
        {
        ret = static_cast<IpsServices::TIpsSetDataEmnStates>(state.toInt());
        }
    */
        
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetEmailNotificationState(
    const IpsServices::TIpsSetDataEmnStates /*aEmnState*/ )
    {
    /* Not supported yet
    if(!SetSettingValue(IpsServices::EmailNotificationState, aEmnState))
        {
        CreateKeyValuePair(IpsServices::EmailNotificationState, aEmnState);
        }
    */
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::FirstEmnReceived() const
    {
    TBool ret = EFalse;
    
    /* Not supported yet
    QVariant state = NULL;
    TBool ok = GetSettingValue(IpsServices::FirstEmnReceived, state);
            
    if(ok)
        {
        ret = static_cast<TBool>(state.toInt());
        }
    */
    
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetFirstEmnReceived( TBool /*aValue*/ )
    {
    /* Not supported yet
    if(!SetSettingValue( IpsServices::FirstEmnReceived, aValue ))
        {
        CreateKeyValuePair(IpsServices::FirstEmnReceived, aValue);
        }
    */
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::EmnReceivedButNotSyncedFlag() const
    {
    TBool ret = EFalse; 

    /* Not supported yet

    QVariant state = NULL;
    TBool ok = GetSettingValue(IpsServices::EmnReceivedNotSynced, state);
                
    if(ok)
        {
        ret = static_cast<TBool>(state.toInt());
        }
    */

    return ret;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetEmnReceivedButNotSyncedFlag( TBool /*aFlag*/ )
    {
    /* Not supported yet
    
    if(!SetSettingValue(IpsServices::EmnReceivedNotSynced, aFlag))
        {
        CreateKeyValuePair(IpsServices::EmnReceivedNotSynced, aFlag);
        }
    */
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TUint CIpsSosAOSettingsHandler::SelectedWeekDays() const
    {
    TUint ret = 0;
    
    QVariant state = NULL;
    TBool ok = GetSettingValue(IpsServices::ReceptionWeekDays, state);
                    
    if(ok)
        {
        ret = static_cast<TUint>(state.toUInt());
        }
    
    return ret;
    }

// ----------------------------------------------------------------------------
// returns start time as seconds from midnight (00:00:00)
// ----------------------------------------------------------------------------
TTime CIpsSosAOSettingsHandler::SelectedTimeStart() const
    {
    QVariant value = NULL;
    TBool ok = GetSettingValue(IpsServices::ReceptionDayStartTime, value);
    // set date time to zero
    TTime zeroTime( 0 );
    TDateTime time = zeroTime.DateTime();
    
    if(ok)
        {
        TInt total = value.toInt();
        TInt minutes = total%KAOMinutesInHour;
        TInt hour = total/KAOMinutesInHour;
        time.SetHour(hour);
        time.SetMinute(minutes);
        }
    
    TTime ret(time);
    return ret;       
    }

// ----------------------------------------------------------------------------
// returns stop time as seconds from midnight (00:00:00)
// ----------------------------------------------------------------------------
TTime CIpsSosAOSettingsHandler::SelectedTimeStop() const
    {
    QVariant value = NULL;
    TBool ok = GetSettingValue(IpsServices::ReceptionDayEndTime, value);
    // set date time to zero
    TTime zeroTime( 0 );
    TDateTime time = zeroTime.DateTime();
    
    if(ok)
        {
        TInt total = value.toInt();
        TInt minutes = total%KAOMinutesInHour;
        TInt hour = total/KAOMinutesInHour;
        time.SetHour(hour);
        time.SetMinute(minutes);
        }
    
    TTime ret(time);
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetLastUpdateInfo( 
        const IpsServices::TAOInfo& aLastUpdateInfo )
    {
    TInt high = I64HIGH(aLastUpdateInfo.iLastSuccessfulUpdate.Int64());
    TInt low = I64LOW(aLastUpdateInfo.iLastSuccessfulUpdate.Int64());
   
    if(!SetSettingValue(IpsServices::AoLastSuccessfulUpdateL, low))
        {
        CreateKeyValuePair(IpsServices::AoLastSuccessfulUpdateL, low);
        }
        
    if(!SetSettingValue(IpsServices::AoLastSuccessfulUpdateH, high))
        {
        CreateKeyValuePair(IpsServices::AoLastSuccessfulUpdateH, high);
        }
        
    if(!SetSettingValue(
            IpsServices::AoLastUpdateFailed, 
            aLastUpdateInfo.iLastUpdateFailed))
        {
        CreateKeyValuePair(IpsServices::AoLastUpdateFailed, 
                aLastUpdateInfo.iLastUpdateFailed);
        }
        
    if(!SetSettingValue(
            IpsServices::AoUpdateSuccessfulWithCurSettings, 
            aLastUpdateInfo.iUpdateSuccessfulWithCurSettings))
        {
        CreateKeyValuePair(IpsServices::AoUpdateSuccessfulWithCurSettings, 
                aLastUpdateInfo.iUpdateSuccessfulWithCurSettings);
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TAOInfo CIpsSosAOSettingsHandler::LastUpdateInfo() const
    {
    IpsServices::TAOInfo info;

    TDateTime time;        
    info.iLastSuccessfulUpdate = time; 
    info.iUpdateSuccessfulWithCurSettings = ETrue;
    QVariant high=NULL;
    QVariant low=NULL;
    QVariant fail=NULL;
    QVariant success=NULL;
        
    TBool ok = GetSettingValue(IpsServices::AoLastSuccessfulUpdateH, high);
    if( ok )
        {
        ok = GetSettingValue(IpsServices::AoLastSuccessfulUpdateL, low);
        }
     if( ok )
         {
         info.iLastSuccessfulUpdate = MAKE_TINT64( high.toInt(), low.toInt() );
         }
    
    ok = GetSettingValue(IpsServices::AoLastUpdateFailed, fail);
    
    if( ok )
        {
        info.iLastUpdateFailed = static_cast<TBool>(fail.toInt());
        }
    
    ok = GetSettingValue(IpsServices::AoUpdateSuccessfulWithCurSettings, success);
    
    if( ok )
        {
        info.iUpdateSuccessfulWithCurSettings = static_cast<TBool>(success.toInt());
        }
    
    return info;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOSettingsHandler::InboxRefreshTime() const
    {
    QVariant value = NULL;
    TBool ok = EFalse;
    if( IsDaytime() )
        {
        ok = GetSettingValue( IpsServices::ReceptionRefreshPeriodDayTime, value);
        }
    else
        {
        ok = GetSettingValue(IpsServices::ReceptionRefreshPeriodOther, value);
        }    
    
    TInt ret = 0;
    if( ok )
        {
        ret = value.toInt();
        }
    
    return ret;
    }



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
HBufC* CIpsSosAOSettingsHandler::EmailAddressL()
    {
    CEmailAccounts* acc = CEmailAccounts::NewLC();
    TSmtpAccount smtp;
    acc->GetSmtpAccountL( iEntry.iRelatedId , smtp );
    CImSmtpSettings* smtpSet = new (ELeave) CImSmtpSettings();
    CleanupStack::PushL( smtpSet );
    acc->LoadSmtpSettingsL( smtp, *smtpSet );
    HBufC* address = smtpSet->EmailAddress().AllocL();
    CleanupStack::PopAndDestroy( 2, acc );
    
    return address;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::GetSettingValue(
        IpsServices::SettingItem aItem,
        QVariant& aValue) const
    {
    bool ret = false;
    if( iExtMgr )
        {
        ret = iExtMgr->readSetting(aItem, aValue);
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::SetSettingValue(
        const IpsServices::SettingItem aItem, 
        const QVariant& aData)
    {
    bool ret = false;
    
    if( iExtMgr )
        {
        ret = iExtMgr->writeSetting(aItem, aData);
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::CreateKeyValuePair(
            const IpsServices::SettingItem aKey, 
            const QVariant& aValue)
    {
    return iExtMgr->createKeyValuePair(aKey, aValue);
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::IsDaytime() const
    {
    TTime current;
    current.HomeTime();
    
    TDateTime tmp = current.DateTime();
    
    //create blanc time object for comparison.
    //real current time cannot be used because we only want to
    //compare hours & minutes
    TTime zeroTime( 0 );
    TDateTime dtNow = zeroTime.DateTime();
    dtNow.SetMinute(tmp.Minute());
    dtNow.SetHour(tmp.Hour());
    
    TTime now(dtNow);
    TTime start = SelectedTimeStart();
    TTime stop = SelectedTimeStop();
    
    TDateTime a = start.DateTime();
    TDateTime b = stop.DateTime();
    
    if(now >= start &&
        now < stop){
        return ETrue;
        }
        
    return EFalse;    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TIpsSetDataAoStates CIpsSosAOSettingsHandler::ProfileIntoAOState(
            const TInt aProfile) const
    {
    IpsServices::TIpsSetDataAoStates ret = IpsServices::EMailAoOff;
    
    if ( aProfile != IpsServices::EmailSyncProfileManualFetch )
        {
        ret = IpsServices::EMailAoAlways;
        }
    
    return ret;
    }
    
// End of file

