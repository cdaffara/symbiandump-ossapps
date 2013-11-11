/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class declaration for monitoring network info change
*
*/


#ifndef IRQNWKINFOOBSERVER_H_
#define IRQNWKINFOOBSERVER_H_

#include <QObject>
#include <QString>
#include <QMutex>

#include "irqnwkinfoobserverexport.h"

class IRQNwkInfoObserverPrivate;

class IRQNWKINFOOBSERVER_DLL_EXPORT IRQNwkInfoObserver : public QObject
{
    Q_OBJECT
    
public:
    /*!
     *  Get the instance of IRQNwkInfoObserver
     *  @return IRQNwkInfoObserver*
     */
    static  IRQNwkInfoObserver* openInstance();

    /*!
     *  Close the instance of IRQNwkInfoObserver
     */
    void closeInstance();

public:   
    /*!
     *  start monitoring network info change
     */
    void startMonitorNwkInfo();

    /*!
     *  stop monitoring network info change
     */    
    void stopMonitorNwkInfo();

signals:

    void currentNwkChanged(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC);
    
    void homeNwkChanged(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC);  
    
private:

    /*!
     * Constructor.
     */
    IRQNwkInfoObserver();
    
    /*!
     * Destructor.
     */    
    ~IRQNwkInfoObserver();
    
    
private:

    /**
     * Number of objects currently referring to the singleton object IRQNwkInfoObserver
     */
    int mRefCount;
    
    /**
     * The instance of IRQNwkInfoObserver singleton
     */
    static IRQNwkInfoObserver *mInstance;
    
    /**
     * Mutex for the thread-safe of openInstance()
     */  
    static QMutex mMutex;
    
    /**
     *  IRQNwkInfoObserver instance
     */
    IRQNwkInfoObserverPrivate* d_ptr;
    
    /**
     *  The successful mark for initialization of private data
     */
    bool mInitPrivateSuccess;

    Q_DISABLE_COPY(IRQNwkInfoObserver)
    
    friend class IRQNwkInfoObserverPrivate; 
};

#endif // IRQNWKINFOOBSERVER_H_
