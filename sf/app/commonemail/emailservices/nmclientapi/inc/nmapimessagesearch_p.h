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
*       Message search private implementation.
*/

#ifndef NMAPIMESSAGESEARCHPRIVATE_H_
#define NMAPIMESSAGESEARCHPRIVATE_H_

#include <MFSMailBoxSearchObserver.h>
#include <CFSMailBox.h>
#include <nmapicommon.h>
#include <nmapimessage.h>

class NmApiEngine;
class CFSMailBox;

class NmApiMessageSearchPrivate : public QObject,
                                    public MFSMailBoxSearchObserver
{
    Q_OBJECT

public:
    
    NmApiMessageSearchPrivate(const quint64 mailboxId, QObject *parent = 0);

    ~NmApiMessageSearchPrivate();

    bool initialise(const QList<QString> &searchStrings, EmailClientApi::NmApiMailSortCriteria sortCriteria);
    
    bool isRunning() const;
    
    bool start();
    
    void cancel();

public: // From MFSMailboxSearchObserver

    void MatchFoundL(CFSMailMessage *aMatchMessage);

    void SearchCompletedL();

    void ClientRequiredSearchPriority(TInt *apRequiredSearchPriority); 

private:
    
    void initialiseMailbox();
    
signals:

    void messageFound(EmailClientApi::NmApiMessage &message);

    void searchComplete(int result = 0); 
    
    void canceled();
    
private:
    
    RPointerArray<TDesC> mSearchStrings;
    TFSMailSortCriteria mSortCriteria;
    TFSMailMsgId mMailBoxId;
    CFSMailBox *mMailBox;
    bool mIsRunning;
    NmApiEngine *mEngine;
    
};

#endif /* NMAPIMESSAGESEARCHPRIVATE_H_ */
