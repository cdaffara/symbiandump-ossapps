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
* Description:  Declaration of the FTU default runtime.
*
*/


#ifndef FTU_RUNTIMES_DEFAULTRUNTIME_H
#define FTU_RUNTIMES_DEFAULTRUNTIME_H

#ifdef LOGGING
#define QDEBUG(a)   qDebug() <<a
#else
#define QDEBUG(a)  
#endif //LOGGING


#include "ftutest_global.h"
#include <QHash>
#include <QStateMachine>

FTU_TEST_CLASS(FtuDefaultRuntimeProviderTest)

// Forward declarations
class QStateMachine;
class HbMainWindow;
class FtuContentService;



/**  @ingroup group_ftudefaultruntimeprovider
 * @brief Default implementation of the ftu runtime.
 *
 * Creates an execution context (EC) and populates it with
 * runtime services. States are loaded from state provider
 * plugins. Each state is given an access to the EC. States
 * are added to a state machine. Finally, the state machine
 * is started.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuDefaultRuntime : public QStateMachine
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.
     * @param parent - address of parent QObject instance, by default none
     */
    FtuDefaultRuntime(QObject* = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    ~FtuDefaultRuntime();

private:
    /**
     * Copy constructor
     */
    Q_DISABLE_COPY(FtuDefaultRuntime)
    
public slots:

private slots:

    /**
     * Called after the state machine has started.
     * @since S60 ?S60_version.
     */
    void handleStateMachineStarted();

    /**
     * Called after the state machine has stopped.
     * @since S60 ?S60_version.
     */
    void handleStateMachineStopped();

signals:

    /**
     * Emission of this signal initiates a transition to the final state.     
     */
    void event_exit() ;
    
private:

    /**
     * Creates content service parts
     * @since S60 ?S60_version.
     */
    void createContentServiceParts();

	/**
     * Creates gui service parts
     * @since S60 ?S60_version.
     */
    void createGuiServiceParts();
    
	/**
     * Creates states
     * @since S60 ?S60_version.
     */
    void createStates();

    /**
    * Assign services to states based on value of property HS_SERVICES_REGISTRATION_KEY
    * @since S60 ?S60_version.
    */
    void assignServices();

    /**
     * Function create and initialize services shared between states
     */
    void initializeRuntimeServices();
    
private:


    /**
     * Content service delivered to runtime's states.
     */
    FtuContentService *mFtuContentService;

    /**
	 * Window to visualize scene
	 */
    HbMainWindow *mWindow;
    
    /**
     * Contains address of runtime service.
     * Ownership transfered to application. Don't remove it.
     */
    QHash<QString,QObject*> mServices;
    
    FTU_TEST_FRIEND_CLASS(FtuDefaultRuntimeProviderTest)

};

#endif // FTU_RUNTIMES_DEFAULTRUNTIME_H

