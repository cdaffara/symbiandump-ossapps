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

#ifndef PIPROFILEREGINEPRIVATE_H
#define PIPROFILEREGINEPRIVATE_H

#include <e32base.h> 
#include <piprofiler/ProfilerAttributes.h>
#include <piprofiler/ProfilerEngineStatusChecker.h> 

class TSamplerAttributes;
class PIMainView;
class PluginAttributes;
class GeneralAttributes;
class SettingItem;
class CProfilerEngineStatusChecker;
class PIProfilerEngine;
class PIProfilerEnginePrivate: public MProfilerStatusObserver
{

public:

    enum TProfilingMode
    {
        EProfilingModeNormal = 0, EProfilingModeTimed
    };

    /**
     * Constructor
     */
    PIProfilerEnginePrivate(PIProfilerEngine *aEngine);

    /**
     * Destructor
     */
    ~PIProfilerEnginePrivate();

    /**
     * Initializesprofiler engine
     */
    bool Init();

    /**
     * Method that is called when UI is ready
     */
    void NotifyUIReady();

    /**
     * Saves plug-in settings into engine
     */
    bool SavePluginSettings(const PluginAttributes &pluginAttributes);

    /**
     * Starts normal profiling
     */
    bool StartProfiling();

    /**
     * Starts timed profiling 
     */
    bool StartTimedProfiling();

    /**
     * Stops profiling
     */
    void StopProfiling();

    /**
     * MProfilerStatusObserver method that is called when profiler engine's status is changed
     */
    void HandleProfilerStatusChange(KProfilerStatus aStatus);

    /**
     * MProfilerStatusObserver method that is called when profiler error is occured.
     */
    void HandleProfilerErrorL(TInt aError);

    /**
     * Gets PI Profiler general settings
     */
    void GetGeneralSettings(GeneralAttributes &settings);

    /**
     * Saves general settings into engine
     */
    bool SaveGeneralSettings(GeneralAttributes &settings);

    /**
     * gets time limit for timed profiling
     */
    int GetTimeLimit();

    /**
     * Sets PI profiler engine to continue running after ui is closed.
     */
    void LeaveProfilingOnAfterClosing();

    bool CheckTraceLocationSanity(QString &location);

private:

    /**
     * Saves general settings into engine
     */
    void SaveGeneralSettingsL();

    /**
     * loads general settings from engine
     */
    TInt LoadGeneralSettingsL();

    /**
     * Starts to profile
     */
    void StartAllSamplerItemsL(TProfilingMode aProfilingMode);

    // Converters for symbian to qt and qt to symbian 

    /**
     * TSamplerAttributes -> SamplerAttributes
     */
    static void convertTSamplerAttributesToPluginAttributes(TSamplerAttributes &tSamplerAttributes,
        PluginAttributes &samplerAttributes);

    /**
     * TSettingItem -> SettingItem
     */
    static void convertTSettingItemToSettingItem(TSettingItem &tSettingItem,
        SettingItem &settingItem);

    /**
     * SettingItem -> TSettingItem
     */
    static void convertSettingItemToTSettingItem(TSettingItem &tSettingItem,
        const SettingItem &settingItem);

    /**
     * TGeneralAttributes -> GeneralAttributes
     */
    static void convertTGeneralAttributesToGeneralAttributes(
        TGeneralAttributes &tSamplerAttributes, GeneralAttributes &samplerAttributes);

    /**
     * GeneralAttributes -> TGeneralAttributes
     */
    static void convertGeneralAttributesToTGeneralAttributes(
        TGeneralAttributes &tSamplerAttributes, GeneralAttributes &samplerAttributes);

    /**
     * Launches PI Profiler engine
     */
    int LaunchEngineL();

    /**
     * Terminates PI Profiler engine
     */
    void terminateEngine();

    /**
     * Gets plug-ins from engine
     */
    void addNewSamplers(CArrayFixFlat<TSamplerAttributes>& aAttributes);

    /**
     * Loads plug-ins
     */
    void loadPlugins();

    /**
     * Finds PI Profiler Engine process
     */
    TInt FindProcessL(RProcess& aProc);

    /**
     * Attaches client into PI Profiler engine
     */
    void AttachClient();

    /**
     * Checks trace location sanity
     */
    TBool CheckTraceLocationSanityL(TBuf8<KPrefixMaxLength>& aAttr);

private:

    // Should Engine leaved on after ui is closed
    bool iLeaveProfilingOnAfterClosing;

    // Public part of engine
    PIProfilerEngine *iPublic;

    // Sampler Attributes
    CArrayFixFlat<TSamplerAttributes>* iSamplerAttributes;

    // General attributes
    TGeneralAttributes iGeneralAttributes;

    // Engine status checker
    CProfilerEngineStatusChecker* iStatusChecker;

};

#endif // PIPROFILEREGINEPRIVATE_H
