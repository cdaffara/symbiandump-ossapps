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

#ifndef NMAPIENVELOPELISTING_H_
#define NMAPIENVELOPELISTING_H_

#include <QList>

#include <nmapimessagetask.h>
#include <nmapidef.h>

class QObject;

class NmApiEngine;

// list messages in particular folder.
namespace EmailClientApi
{
class NmApiEnvelopeListingPrivate;
class NmApiMessageEnvelope;

class NMAPI_EXPORT NmApiEnvelopeListing : public NmApiMessageTask
{
    Q_OBJECT
public:
    /*!
       Constructor of class. It set start values.
     */
    NmApiEnvelopeListing(QObject *parent, const quint64 folderId, const quint64 mailboxId);

    /*!
       Destructor of class. It release engine to be safe if manual releasing won't work.
     */
    ~NmApiEnvelopeListing();

    enum {EnvelopeListingFailed = -1};

    /*! 
       \brief Returns results after envelopesListed signal is received.
       
        Caller gets ownership of envelopes. Returns true if results were available.
        Before calling this method, cancel and start should be called, 
        because after second call it returns empty list.
        It also at start clear inputlist of NmApiMessageEnvelope.
        
        \param envelopes envelopes
     */
    bool getEnvelopes(QList<EmailClientApi::NmApiMessageEnvelope> &envelopes);

    /*!
       \brief Return info if listing is running
     */
    bool isRunning() const;
    signals:
    /*!
       Emitted when listing is available, count is number of envelopes found
       or EnvelopeListingFailed if listing failed
     */
    void envelopesListed(qint32 count);

public slots:
    /*!
       \brief Starts gathering envelopes list.
       
       In first turn it will get whole folderlist. 
       If start works, it do nothing.
       
       To asynchronous operation ce be used \sa QTimer::singleShot on this method.
       Example:
       \code
       QTimer::singleShot(0,nmEnvelopeListing,SLOT(start());
       \endcode
       
     */
    bool start();

    /*!
       \brief Stop gathering envelope list.
       
       In first it change state of listing.
       Then it release engine.
       On end it clears list of envelopes and emits \sa NmApiMessageTask::canceled() signal.
     */
    void cancel();

private:
    NmApiEnvelopeListingPrivate *mListingPrivate;
};
}

#endif /* NMAPIENVELOPELISTING_H_ */
