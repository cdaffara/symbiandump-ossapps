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
*     Class to handle imap/pop/smtp settings & cenrep
*
*/


#ifndef IPSSOSAOSETTINGSHANDLER_H_
#define IPSSOSAOSETTINGSHANDLER_H_

#include <e32base.h> 

#include "ipsplgcommon.h"
#include "nmipssettingitems.h"


class CMsvSession;
class TMsvEntry;
class NmId;
class QVariant;


const TInt KIpsHeadersOnly           = -2;
const TInt KIpsFullBodyAndAttas      = -1;
const TInt KIpsFullBodyOnly          = -3;

class NmIpsSosAoExtendedSettingsManager;
/**
* class CIpsSosAOSettingsHandler
*
* @lib ipssosaoplugin.lib
*/
NONSHARABLE_CLASS (CIpsSosAOSettingsHandler) : public CBase
    {
    
public:
    
    /*
     * NewL
     */
    static CIpsSosAOSettingsHandler* NewL( 
            CMsvSession& aSession, TMsvId aMailboxId );
    
    /*
     * Destructor
     */
    virtual ~CIpsSosAOSettingsHandler();
    
    /**
     * List folders subscribed to be synced.
     * Not supported.
     */
    void GetSubscribedImapFoldersL( 
            TMsvId aServiceId, 
            RArray<TMsvId>& aFoldersArray );
    
    /**
     * Reads TImImap4GetPartialMailInfo data from settings
     * @param aInfo return parameter
     * @param aImap4Settings reference to settings 
     */
    void ConstructImapPartialFetchInfo( 
            TImImap4GetPartialMailInfo& aInfo, 
            CImImap4Settings& aImap4Settings );
    
    /**
     * @return state of alwaysonline
     */
    IpsServices::TIpsSetDataAoStates AlwaysOnlineState() const;
    
    /**
     * @param aAlwaysOnlineState, new state for alwaysonline
     */
    void SetAlwaysOnlineState(
        const IpsServices::TIpsSetDataAoStates aAlwaysOnlineState );
    
    /**
     * Fetches the state of email notification -flag
     * @return ETrue or EFalse, according to flag state
     */
    IpsServices::TIpsSetDataEmnStates EmailNotificationState() const;
    
    /**
     * Sets the state of email notification -flag
     */
    void SetEmailNotificationState(
        const IpsServices::TIpsSetDataEmnStates aEmnState );
        
    /**
     * Returns flag which indicates has mailbox received 
     * it's first OMA EMN. 
     */
    TBool FirstEmnReceived() const;
    
    /**
     * Sets a flag when mailbox receives it's first OMA EMN.
     * (email notification)
     * @param flag value
     */
    void SetFirstEmnReceived( TBool aValue );
    
    /**
    * returns value of this flag
    * @return is flag set
    */
    TBool EmnReceivedButNotSyncedFlag() const;
        
    /**
    * sets flag value
    * @param flag value
    */
    void SetEmnReceivedButNotSyncedFlag( TBool aFlag );
    
    /**
     * @return week days selected when sync is active
     */
    TUint SelectedWeekDays() const;
    
    /**
     * @return start time 
     */
    TTime SelectedTimeStart() const;
    
    /**
     * @return stop time
     */
    TTime SelectedTimeStop() const;
    
    /**
     * @param aLastUpdateInfo
     */
    void SetLastUpdateInfo( const IpsServices::TAOInfo& aLastUpdateInfo );

    /**
     * @return info about last update
     */
    IpsServices::TAOInfo LastUpdateInfo() const;
    
    /**
     * @return refresh interval
     */
    TInt InboxRefreshTime() const;
    
    
    /**
     * Gets email address from smtp settings.
     * @return pointer to emailaddress. ownership is transferred.
     */
    HBufC* EmailAddressL();
    
private:
    
    /*
     * Constructor 
     */
    CIpsSosAOSettingsHandler(CMsvSession& aSession);
        
private:
    
    /*
     * 2nd phase constructor 
     */
    void ConstructL( TMsvId aMailboxId );
    
    
    /**
     * Gets FS plugin id related to protocol
     * @return FS plugin id
     */
    TInt GetFSPluginId() const;
    
    /**
     * Constructs NmId type mailbox id from TMsvId and plugin's id.
     */
    NmId MboxId() const;
    
    /**
     * Reads values from extended settings
     * @param aItem wanted item id
     * @param aValue return parameter
     * @return did succeed
     */
    TBool GetSettingValue(IpsServices::SettingItem aItem, QVariant& aValue) const;
    
    /**
     * Sets setting value into extended settings
     * @param aItem item which to set
     * @param items data
     * @return did succeed
     */
    TBool SetSettingValue(
            const IpsServices::SettingItem aItem, 
            const QVariant& aData);
    
    /**
     * Creates a new key-value pair into cenrep
     * @param aKey key to be created
     * @aValue initial value for the key
     * @return did succeed
     */
    TBool CreateKeyValuePair(
            const IpsServices::SettingItem aKey, 
            const QVariant& aValue);
    
    /**
     * method to determine daytime and "other" time
     */
    TBool IsDaytime() const;
    
    /**
     * Maps TIPsSetProfiles into TIpsSetDataAoStates 
     */
    IpsServices::TIpsSetDataAoStates ProfileIntoAOState(
            const TInt aProfile) const;
    
    
private:
    NmIpsSosAoExtendedSettingsManager* iExtMgr;//owned
    TPckgBuf<TIpsPlgPropertyEvent>  iPropertyBuf;
    TMsvEntry                       iEntry;
    CMsvSession&                    iSession;
    };

#endif /*IPSSOSAOSETTINGSHANDLER_H_*/
