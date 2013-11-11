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

#ifndef _PRESENCE_READER_QT_H
#define _PRESENCE_READER_QT_H

#include <QObject>
#include <presencecacheglobal_qt.h>

class PrcPresenceReaderPrivate;
class PrcPresenceBuddyInfoQt;
class MPresenceCacheReader2; 
class MPresenceCacheReadHandler2Qt;


/**
 * Implementation for presence cache reader
 */
class PRESENCECACHE_EXPORT PrcPresenceReader : public QObject                                       
    {
    Q_OBJECT
public:

    /**
     * Factory method to instantiate PresenceReader
     * @return The new PresenceReader object. Object
     *         ownership is returned to caller.
     */
     static PrcPresenceReader* createReader();
    
    
    /**
     * Public destructor.
     */
     virtual ~PrcPresenceReader();
    
 
private:

    /**
    * C++ constructor
    */
    PrcPresenceReader();
      
public: 
    /**
     * Reads presence info from cache for a given identity, returns immediately.
     *
     * @param identity buddy id, identity must be in xsp id format
     * @return MPresenceBuddyInfoQt for the given buddy. Ownership to caller.
     *         Null if service or buddy not found.    
     */
    PrcPresenceBuddyInfoQt* presenceInfo(const QString& identity);
   
   /**
   * Returns total number of buddies stored in presence cache.
   * It includes all services, probably useful while deciding performance 
   * overheads or informing to the user.
   *
   * @return Total number of presence infos in presence cache.
   */         
    int buddyCountInAllServices();
   /**
   * Returns total number of buddies (presence infos) stored in a specific
   * service in presence cache. It includes only the given service. 
   *
   * @param serviceName as in device's service table
   * @return Buddy count or error code. 
   */    
    int buddyCountInService(const QString& serviceName) ;
   
   /**
     * Returns total number services in presence cache.
     *
     * @return Total number of services in presence cache.
     */                       
    int servicesCount();
   /**
    * Cancels any async. read operation going on. After calling this
    * method no callback will be received from asycn read method.
    * @return true if cancel successful
    */
    bool cancelRead() ;              
   /**
    * Reads all presence info from cache for a given service. Emits  
    * HandlePresenceRead signal.
    *
    * @param serviceName Service name to search from
    * @return True if requesting was successful.
    */    
    bool allBuddiesPresenceInService(const QString& serviceName) ; 
                                                  
    /**
     * Subscribe for given presence buddy's change in presence. 
     *
     * @param identity buddy id, in xsp format
     * @return true if subscribed successfully
     *
     */
    bool subscribePresenceBuddyChange( const QString& identity);
    /**
      * Unsubscribe for given presence buddy's change in presence. 
      *
      * @param identity buddy id, in xsp format
      */
     void unSubscribePresenceBuddyChange(const QString& identity);
    
    
signals:
    /*
     * This signal is emitted in response to asynchronous method allBuddiesPresenceInService.
     * Signal emitted when the operation is complete
     * @param success = true, if operation successful
     * @param buddyInfoList - list of  buddies read. Ownership of elements to callee. 
     *                        Can be empty if not found.
     */
    void signalPresenceRead(bool success, QList<PrcPresenceBuddyInfoQt*> buddyInfoList);
    
    /*
    * This signal is emitted when there are changes in any of the subscribed buddy info. 
    * @param success == true, if operation successful
    * @param presenceBuddyInfo - presence info for buddy. 
    *                           Ownership to callee. Can be NULL if success == false exist.
    */
    void signalPresenceNotification(bool success, PrcPresenceBuddyInfoQt* presenceBuddyInfo); 
    
private:
    /*
     * Private implementation
     * own
     */
    PrcPresenceReaderPrivate* d;
    };

#endif // _PRESENCE_READER_QT_H



