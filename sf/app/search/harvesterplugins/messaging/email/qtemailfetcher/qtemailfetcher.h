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
* Description: Utility class for fetching email.
*
*/

#ifndef _QEMAILFETCHER_H
#define _QEMAILFETCHER_H

#ifdef BUILD_DLL
#define DLL_EXPORT Q_DECL_EXPORT
#else
#define DLL_EXPORT Q_DECL_IMPORT
#endif

//TODO: Uncomment when available.
#include <nmapiemailservice.h>
#include <nmapieventnotifier.h>
#include <nmapimailboxlisting.h>
#include <nmapienvelopelisting.h>
#include <nmapimessageenvelope.h>
#include <nmapifolderlisting.h>
#include <QObject>
#include <nmapifolder.h>
#include "memailitemobserver.h"

using namespace EmailClientApi;

//How this works:
//1. List all mail boxes.
//2. In each mail box, list the folders
//3. In each folder, list the mails
//4. Process each email
//
//We voluntarily relinquish the processor (we sleep) each time we have completed 
//processing a given folder. This is to avoid 'hogging' the processor.


class DLL_EXPORT QEmailFetcher: public QObject
    {
    Q_OBJECT
public:
    static QEmailFetcher* newInstance(MEmailItemObserver& aObserver);
    ~QEmailFetcher();
    void StartHarvesting();
    
private:
    QEmailFetcher(MEmailItemObserver& aObserver );
    void initialize(); //helper (2nd phase constructor).
    void processNextMailbox();
    void processNextFolder();
    void processNextEnvelope();
    void NotifyHarvestingComplete();
    CSearchDocument* getSearchDocumentL( const NmApiMessageEnvelope& aEnvelope ,quint64 aMailboxId, quint64 aFolderId );
public slots: //public since they need to be called by *other* objects.
    void emailServiceIntialized( bool );
    void handleMailboxesListed( qint32 );
    void handleMailFoldersListed( qint32 );
    void processMessages( qint32 );
    //Connect to messageEvent signal
    void handleMessageEvent( EmailClientApi::NmApiMessageEvent aEvent, quint64 mailboxId, quint64 folderId, QList<quint64> messageList );
    
private:
    MEmailItemObserver& iEmailObserver;     //The parent/creator. Not owned.
    NmApiEventNotifier* iEmailEventNotifier;   //owned; triggers handleMessageEvent.
    NmApiEmailService* iEmailService;          //owned. 
    NmApiMailboxListing* iMailBoxListings;     //owned.
    NmApiFolderListing* iMailFolderList;       //owned.
    NmApiEnvelopeListing* iEnvelopeListing;    //owned.
    
    //These are needed to asynchronously process *all* mailboxes/folders.
    int iCurrentMailboxIndex;
    int iCurrentFolderIndex;
    QList<NmApiMailbox> iMailBoxes;
    QList<NmApiFolder> iFolders;
    };

#endif //_QEMAILFETCHER_H
