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

#ifndef ENGINEWRAPPER_H
#define ENGINEWRAPPER_H

#include <QObject>
#include <QColor>
#include <QList>
#include <QTime>
#include <QTimer>

enum SettingThreadPriorityTypes
{
    EThreadPriorityTypeMuchLess = 0,
    EThreadPriorityTypeLess,
    EThreadPriorityTypeNormal,
    EThreadPriorityTypeMore,
    EThreadPriorityTypeMuchMore,
    EThreadPriorityTypeRealTime,
    EThreadPriorityTypeAbsoluteVeryLow,
    EThreadPriorityTypeAbsoluteLow,
    EThreadPriorityTypeAbsoluteBackground,
    EThreadPriorityTypeAbsoluteForeground,
    EThreadPriorityTypeAbsoluteHigh
};

enum SettingCPUModes
{
    ECPUModeNotSet = -1,
    ECPUModeCPUTime,
    ECPUModeNOPs
};

enum SettingDataPopupVisbilities
{
    EDataPopupVisbilityAlwaysOn = 0,
    EDataPopupVisbilityBackgroundOnly,
    EDataPopupVisbilityAlwaysAlwaysOff
};

enum SettingDataPopupLocations
{
    EDataPopupLocationTopRight = 0,
    EDataPopupLocationBottomMiddle
};

enum SettingSources
{
    ESourceCPU = 0,
    ESourceRAM,
    ESourceC,
    ESourceD,
    ESourceE,
    ESourceF,
    ESourceG,
    ESourceH,
    ESourceI,
    ESourcesLength // this should be always the last!
};

enum SettingLoggingMode
{
    ELoggingModeRDebug = 0,
    ELoggingModeLogFile,
    ELoggingModeRDebugLogFile
};


struct SampleData
{
    qint64 mFree;
    qint64 mSize;
    qint64 mTimeFromStart;
};

class SampleEntry
{
public:
    QString description() const { return mDescription; }
    QString unitShort() const { return mUnitShort; }
    QString unitLong() const { return mUnitLong; }
    int driveNumber() const { return mDriveNumber; }
    QColor graphColor() const { return mGraphColor; }

    int sampleCount() const { return mSampleData.length(); }
    const SampleData &sample(int index) const { return mSampleData[index]; }
private:
    QString mDescription;
    QString mUnitShort;
    QString mUnitLong;
    int mDriveNumber;
    QColor mGraphColor;

    QList<SampleData> mSampleData;

friend class EngineWrapper;
};

class PerfMonSources
{
public:
    int count() const { return 9; }
    int isEnabled (int index) const { return mSources[index]; }
    void setEnabled(int index, bool enabled) { mSources[index] = enabled; }
    QList<int> enabledIndexes() const
    {
        QList<int> indexes;
        for(int i=0; i<count(); i++) {
            if (isEnabled(i))
                indexes.append(i);
        }
        return indexes;
    }
    void setEnabledIndexes(const QList<int> &indexes)
    {
        for(int i=0; i<count(); i++) {
            setEnabled(i, indexes.contains(i));
        }
    }

PerfMonSources() {
    memset(mSources, 0, sizeof mSources);
    mSources[0] = mSources[1] = mSources[2] = true;
}

private:
    bool mSources[9];
};

class PerfMonSettings
{
public:
    int heartBeat() const { return mHeartBeat; }
    int maxSamples() const { return mMaxSamples; }
    int priority() const { return mPriority; }
    int cpuMode() const { return mCPUMode; }
    bool keepBacklightOn() const { return mKeepBacklightOn; }

    int dataPopupVisibility() const { return mDataPopupVisibility; }
    int dataPopupLocation() const { return mDataPopupLocation; }
    PerfMonSources &dataPopupSources() { return mDataPopupSources; }
    const PerfMonSources &dataPopupSources() const { return mDataPopupSources; }

    int graphVerticalBarPeriod() const { return mGraphsVerticalBarPeriod; }
    PerfMonSources &graphSources() { return mGraphSources; }
    const PerfMonSources &graphSources() const { return mGraphSources; }

    int loggingMode() const { return mLoggingMode; }
    QString loggingFilePath() const { return mLoggingFilePath; }
    PerfMonSources &loggingSources() { return mLogSources; }
    const PerfMonSources &loggingSources() const { return mLogSources; }

    bool loggingEnabled() { return mLoggingEnabled; }


    void setHeartBeat(int heartBeat) { mHeartBeat = heartBeat; }
    void setMaxSamples(int maxSamples) { mMaxSamples = maxSamples; }
    void setPriority(int priority) { mPriority = priority; }
    void setCpuMode(int cpuMode) { mCPUMode = cpuMode; }
    void setKeepBacklightOn(bool backlightOn) { mKeepBacklightOn = backlightOn; }

    void setDataPopupVisibility(int visibility) { mDataPopupVisibility = visibility; }
    void setDataPopupLocation(int location) { mDataPopupLocation = location; }

    void setGraphVerticalBarPeriod(int period) { mGraphsVerticalBarPeriod = period; }

    void setLoggingMode(int mode) { mLoggingMode = mode; }
    void setLoggingFilePath(const QString& filePath) { mLoggingFilePath = filePath; }

    void setLoggingEnabled(bool enabled) { mLoggingEnabled = enabled; }

private:
    friend class EngineWrapper;

    PerfMonSettings() : mHeartBeat(600), mMaxSamples(64), mPriority(2), mCPUMode(1),
        mKeepBacklightOn(true), mDataPopupVisibility(0), mDataPopupLocation(0),
        mGraphsVerticalBarPeriod(5), mLoggingMode(1), mLoggingFilePath("C:\\data\\perfmon.log"),
        mLoggingEnabled(true) {}


private:
    int mHeartBeat;
    int mMaxSamples;
    int mPriority;
    int mCPUMode;
    bool mKeepBacklightOn;

    int mDataPopupVisibility;
    int mDataPopupLocation;
    PerfMonSources mDataPopupSources;

    int mGraphsVerticalBarPeriod;
    PerfMonSources mGraphSources;

    int mLoggingMode;
    QString mLoggingFilePath;
    PerfMonSources mLogSources;

    bool mLoggingEnabled;
};

/**
 * Mock engine to simulate perfmon engine for non-symbian development.
 */
class EngineWrapper : public QObject
 {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    EngineWrapper();

    /**
     * Destructor
     */
    ~EngineWrapper();

public:
    const QList<SampleEntry>& sampleEntries() const { return mEntries; }

    PerfMonSettings &settings() { return mSettings; }
    const PerfMonSettings &settings() const { return mSettings; }

    void setLoggingEnabled(bool enabled) { mSettings.setLoggingEnabled(enabled); }

public slots:

    bool updateSettings();
    bool initialize();
    void finalize();

signals:

    void samplesUpdated();
    void settingsUpdated();

private slots:

    void update();

private:

    QList<SampleEntry> mEntries;

    QTimer mTimer;

    QTime mStartTime;

    PerfMonSettings mSettings;
};

#endif //ENGINEWRAPPER_H
