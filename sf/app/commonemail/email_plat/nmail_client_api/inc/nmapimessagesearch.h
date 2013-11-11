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
 *     Email messages search public header.
 */

#ifndef NMAPIMESSAGESEARCH_H_
#define NMAPIMESSAGESEARCH_H_

#include <nmapidef.h>
#include <nmapimessagetask.h>
#include <nmapicommon.h>
#include <nmapimessage.h>

class NmApiMessageSearchPrivate;

namespace EmailClientApi 
{

class NMAPI_EXPORT NmApiMessageSearch : public NmApiMessageTask
{
    Q_OBJECT
public:
    NmApiMessageSearch(QObject *parent, quint64 mailboxId);

    virtual ~NmApiMessageSearch();
    
    bool isRunning() const;

    bool initialise(const QList<QString> &searchStrings, EmailClientApi::NmApiMailSortCriteria sortCriteria);

public slots: 

    bool start();

    void cancel();
    
signals:

    void messageFound(EmailClientApi::NmApiMessage &message);

    void searchComplete(int result = NmApiNoError);

private:
    NmApiMessageSearchPrivate* d;
};

} // namespace EmailClientApi
#endif /* NMAPIMESSAGESEARCH_H_ */
