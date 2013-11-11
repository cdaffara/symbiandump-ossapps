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
* Description:  First Time Use application main class.
*
*/


#ifndef FTU_FIRSTTIMEUSE_H
#define FTU_FIRSTTIMEUSE_H

#include "ftutest_global.h"

#include <QObject>

#ifdef LOGGING
#define QDEBUG(a)   qDebug() <<a
#else
#define QDEBUG(a)  
#endif //LOGGING

FTU_TEST_CLASS(FtuFirstTimeUseTest)

class QStateMachine;

/**
 * @ingroup group_ftuapplication
 * @brief First Time Use application main class.
 * 
 * Loads a runtime from a runtime provider plugin. Manages 
 * the runtime execution.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuFirstTimeUse : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Parent object.
     */
    FtuFirstTimeUse(QObject* aParent = 0);
    
    /**
     * Destructor.
     * @since S60 ?S60_version.     
     */
    virtual ~FtuFirstTimeUse();

private:

    /**
     * Registers service plugins pre-installed on the device.
     */
    void registerServicePlugins();
    /**
     * Recursively registers service plugins starting from given root
     * directory. All directories containing plugins are added to
     * application's library paths at the same time.
     * @param root The root folder.
     */
    void registerServicePlugins(const QString &root);

    Q_DISABLE_COPY(FtuFirstTimeUse)

signals:

    /**
     * Emitted when the FTU application needs to exit.
     * @since S60 ?S60_version.     
     */
    void exit();

public slots:

    /**
     * Starts the runtime.
     * @since S60 ?S60_version. 
     */
    void start();
    
    /**
     * Stops the runtime.
     * @since S60 ?S60_version.     
     */
    void stop();

private slots:

    /**
     * Called after the runtime has started.
     * @since S60 ?S60_version.     
     */
    void handleRuntimeStarted();

    /**
     * Called after the runtime has stopped.
     * @since S60 ?S60_version.     
     */
    void handleRuntimeStopped();
    
    /**
     * Called after the runtime has faulted.
     * @since S60 ?S60_version.     
     */
    void handleRuntimeFaulted();

private:

    /**
     * Runtime.
     */
    QStateMachine*     mRuntime;
    
    FTU_TEST_FRIEND_CLASS(FtuFirstTimeUseTest)

};

#endif

