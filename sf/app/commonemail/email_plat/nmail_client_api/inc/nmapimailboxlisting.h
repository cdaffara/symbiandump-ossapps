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

#ifndef NMAPIMAILBOXLISTING_H_
#define NMAPIMAILBOXLISTING_H_


#include <nmapimessagetask.h>
#include <nmapidef.h>

class QObject;

namespace EmailClientApi
{
class NmApiMailboxListingPrivate;
class NmApiMailbox;

/*!
   \class Class for creating list of all mailboxes
 */
class NMAPI_EXPORT NmApiMailboxListing : public NmApiMessageTask
{
    Q_OBJECT
public:
    /*!
       Constructor of class. It set start values.
     */
    NmApiMailboxListing(QObject *parent);
    /*!
       Destructor of class. It release engine to be safe if manual releasing won't work.
     */
    ~NmApiMailboxListing();

    enum {MailboxListingFailed = -1};

    /*! 
       \brief Returns results after mailboxesListed signal is received.
       
        Caller gets ownership of mailboxes. Returns true if results were available.
        Before calling this method, cancel and start should be called, 
        because after second call it returns empty list.
        At start it clear inputlist of mailboxes.
        
        \return Return true if results were avaible
        \param mailboxes List of mailboxes to filled. On start is cleared. 
     */
    bool getMailboxes(QList<EmailClientApi::NmApiMailbox> &mailboxes);

    /*!
       \brief Return info if listing is running
       
       \return Return true if listing is running
     */
    bool isRunning() const;

    signals:
    /*!
       
       emitted when listing is available, count is number of mailboxes found
       or MailboxListingFailed if listing failed
       
       \param count Count of mailboxes inside class.
     */
    void mailboxesListed(qint32 count);

public slots:
    /*!
       \brief Starts gathering mailbox list.
       
       In first turn it will get whole mailboxlist. 
       Then it initialize core arguments and emits signal when ready.
       
       To asynchronous operation can be used \sa QTimer::singleShot on this method.
       Example:
       <code> 
       QTimer::singleShot(0,nmMailboxListing,SLOT(start());
       </code>
       
       \return Return true if everything go good and core of listing works good.
       
     */
    bool start();

    /*!
       \brief Stop gathering mailbox list.
       
       In first it change state of listing.
       Then it release engine.
       On end it clears list of mailboxes and emits \sa NmApiMessageTask::canceled() signal.
     */
    void cancel();

private:
    NmApiMailboxListingPrivate *mNmApiMailboxListingPrivate;
};

}

#endif /* NMAPIMAILBOXLISTING_H_ */
