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

#ifndef NMAPIFOLDERLISTING_H_
#define NMAPIFOLDERLISTING_H_

#include <nmapimessagetask.h>
#include <nmapidef.h>

class NmApiEngine;

namespace EmailClientApi
{

class NmApiFolderListingPrivate;
class NmApiMailbox;
class NmApiFolder;
/*!
   \class Class for creating list of all folders
 */
class NMAPI_EXPORT NmApiFolderListing : public NmApiMessageTask
{
    Q_OBJECT
public:
    /*!
       Constructor of class. 
     */
    NmApiFolderListing(QObject *parent, const quint64 &mailboxId);
    /*!
       Destructor of class. 
     */
    ~NmApiFolderListing();

    enum {FolderListingFailed = -1};

    /*! 
       \brief Returns results after foldersListed signal is received.
       
        Before calling this method, cancel and start should be called, 
        because after second call it returns empty list..
        It also at start clear inputlist of NmFolder.
     */
    bool getFolders(QList<EmailClientApi::NmApiFolder> &folders);

    /*!
       \brief Returns info if listing is running
     */
    bool isRunning() const;

    signals:
    /*!
       emitted when listing is available, count is number of folders found
       or FolderListingFailed if listing failed
     */
    void foldersListed(qint32 count);

public slots:
    /*!
       \brief Starts gathering folders list.
       
       In first turn it will get whole folderlist. 
       \todo After that it will wait for folder events.
       If start works, it do nothing.
       
       To asynchronous operation ce be used \sa QTimer::singleShot on this method.
       Example:
       <code> 
       QTimer::singleShot(0,nmFolderListing,SLOT(start());
       </code>
       
     */
    bool start();

    /*!
       \brief Stops gathering folder list.
       
       In first it change state of listing.
       Then it release engine.
       On end it clears list of folders and emits \sa NmApiMessageTask::canceled() signal.
     */
    void cancel();

private:
    NmApiFolderListingPrivate *mFolderListing;
};
}

#endif /* NMAPIFOLDERLISTING_H_ */
