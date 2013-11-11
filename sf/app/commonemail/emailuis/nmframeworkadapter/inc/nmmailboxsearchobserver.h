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

#ifndef NMMAILBOXSEARCHOBSERVER_H_
#define NMMAILBOXSEARCHOBSERVER_H_

#include <QObject>
#include <MFSMailBoxSearchObserver.h>

class NmId;


class NmMailboxSearchObserver : public QObject,
                                public MFSMailBoxSearchObserver
{
    Q_OBJECT

public:

    NmMailboxSearchObserver();

    ~NmMailboxSearchObserver();


public: // From MFSMailboxSearchObserver

    void MatchFoundL(CFSMailMessage *aMatchMessage);

    void SearchCompletedL();

    void ClientRequiredSearchPriority(TInt *apRequiredSearchPriority); 
    
    
signals:

    void matchFound(const NmId &messageId, const NmId &folderId);

    void searchComplete();
};


#endif /* NMMAILBOXSEARCHOBSERVER_H_ */

// End of file.
