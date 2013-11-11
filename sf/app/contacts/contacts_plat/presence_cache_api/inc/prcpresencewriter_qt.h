/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for presence cache reader and writer.
*
*/

#ifndef _PRESENCE_WRITER__H_
#define _PRESENCE_WRITER__H_

#include <QObject>
#include <presencecacheglobal_qt.h>
class PrcPresenceBuddyInfoQt;
class PrcPresenceWriterPrivate;


/**
 * Implementation for presence cache writer. 
 */
class PRESENCECACHE_EXPORT PrcPresenceWriter : public QObject
    {
    Q_OBJECT
public:

    /**
     * Factory method to instantiate CPresenceCacheClient
     * @return The new PresenceWriter object. Object
     * ownership is returned to caller.
     */
     static PrcPresenceWriter* createWriter();
    
    
    /**
     * Public destructor.
     */
     virtual ~PrcPresenceWriter();
    
 
private:

    /**
    * C++ constructor
    */
    PrcPresenceWriter();
      
public: 
   
   /**
    * Writes presence info to cache for a given buddy.
    * @param presenceBuddyInfo contains xsp identity and presence
    * @return true if operation successful
    */ 
     bool writePresence(const PrcPresenceBuddyInfoQt& presenceBuddyInfo) ;
    
    /**
      * Instantiates new buddy presence info object.
      *
      * @return New buddy presence info instance. Object ownership
      *         is returned to caller.
      */
     PrcPresenceBuddyInfoQt* newBuddyPresenceInfo() ;
    
    /**
     * Delete all buddies presence associated with given service.
     *
     * @param serviceName service name as in sp table
     * @return True if operation successful
     */
     bool  deleteService(const QString& serviceName);
    /**
    * Deletes all presence related to given xsp identity.
    
    * @param identity identity for buddy to be removed, in xsp format
    * @return true if operation successful
    */
     bool deletePresence(const QString& identity) ;
    /**
    * Cancels any async write operation going on. After calling this
    * method no callback will be received from asycn write method.
    * @return true if operation successful
    */
     bool cancelWrite();    
    
   /** presence cache writer Asynchronous method
     * Writes buddy presence info objects to cache. Emits 
     * handlePresencewrite signal. 
     *
     * @param presenceBuddyInfos presence infos for all buddies. 
     *         Ownership remains to caller.
     * @return true if request submitted successfully
     */    
     bool writeMultiplePresence(const QList<PrcPresenceBuddyInfoQt*>& presenceBuddyInfos);         


signals:
    /*
     * This signal is emitted in response to asynchronous method writeMultiplePresence.
     * Signal emitted when the operation is complete
     * @param success = true, if operation successful
     */
    void signalPresenceWrite(bool success);

private:
    /*
     * Private implementation
     * own
     */
    PrcPresenceWriterPrivate* d ;
    };

#endif // _PRESENCE_WRITER__H_



