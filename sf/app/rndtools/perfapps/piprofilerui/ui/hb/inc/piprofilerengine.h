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
*
*/

#ifndef PIPROFILEREGINE_H
#define PIPROFILEREGINE_H

#include <qobject.h>
#include <qlist.h>

#include "pluginattributes.h"

class PIProfilerEnginePrivate;
class GeneralAttributes;

enum ProfilerEngineStatus
{
    PI_PROFILING = 0, PI_FINISHED_SUCCEFULLY, PI_ERROR
};
enum ProfilingMode
{
    PI_PROFILINGMODENORMAL = 0, PI_PROFILINGMODETIMED
};
enum ProfilingOutput
{
    PI_DEBUG_OUTPUT = 0, PI_FILE_OUTPUT
};

class PIProfilerEngine: public QObject
{
Q_OBJECT
public:

    /**
     * Constructor
     */
    PIProfilerEngine();

    /**
     * Initilises engine
     */
    bool init();

    /**
     * Destructor
     */
    ~PIProfilerEngine();

    /**
     * Gets PI Profiler general settings
     */
    void getGeneralSettings(GeneralAttributes &settings);

    /**
     * Notifys engine that ui is ready
     */
    void notifyUIReady();

    /**
     * Saves general settings into engine
     */
    bool saveGeneralSettings(GeneralAttributes &settings);

    /**
     * Saves plug-in settings into engine
     */
    bool savePluginSettings(const PluginAttributes &pluginAttributes);

    /**
     * Starts normal profiling
     */
    bool startProfiling();

    /**
     * Starts timed profiling 
     */
    bool startTimedProfiling();

    /**
     * Stops profiling
     */
    void stopProfiling();

    /**
     * gets time limit for timed profiling
     */
    int getTimeLimit();

    /**
     * Sets PI profiler engine to continue running after ui is closed.
     */
    void leaveProfilingOnAfterClosing();

    bool checkTraceLocationSanity(QString &location);

signals:

    /**
     * Signal that is emitted when plug-in list is loaded
     */
    void pluginListUpdated(QList<PluginAttributes> &samplerList);

    /**
     * Signal that is emitted when profiling status is changed
     */
    void profilingStatusChanged(ProfilerEngineStatus status, const QString &text = 0,
        ProfilingMode profilingMode = PI_PROFILINGMODENORMAL, ProfilingOutput output =
            PI_FILE_OUTPUT);

private:

    // private implementation
    PIProfilerEnginePrivate *mPrivate;

private:
    // Friend classes

    friend class PIProfilerEnginePrivate;

};

#endif // PIPROFILEREGINE_H
