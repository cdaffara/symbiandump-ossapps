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
* Description: Handles the populating of default settings for pop and imap mailboxes
*
*/

#ifndef NMAPIPOPIMAPSETTINGSMANAGER_H
#define NMAPIPOPIMAPSETTINGSMANAGER_H


namespace EmailClientApi {
class NmApiMailboxSettingsData;
class NmApiPopImapSettingsManager 
{
public:
    NmApiPopImapSettingsManager();
    ~NmApiPopImapSettingsManager();
    bool populateDefaults(const QString &mailboxType, NmApiMailboxSettingsData &data);

private:
    void populateImapDefaultsL(NmApiMailboxSettingsData &data);
    void populatePopDefaultsL(NmApiMailboxSettingsData &data);
    void populateSmtpDefaultsL(NmApiMailboxSettingsData &data);
    void populateCenrepDefaults(NmApiMailboxSettingsData &data);
    
};
}//namespace

#endif // NMAPIPOPIMAPSETTINGSMANAGER_H
