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

#include "perfmon_engine.h"

#include <QObject>
#include <QColor>
#include <QList>
#include <QVariant>

struct SampleData
{
    qint64 mFree;
    qint64 mSize;
    qint64 mTimeFromStart;
};

class SampleEntry
{
public:
    SampleEntry(const TSampleEntry& sampleEntry) : mEntry(&sampleEntry) {}

    QString description() const { return QString((QChar*)mEntry->iDescription.Ptr(), mEntry->iDescription.Length()); }
    QString unitShort() const { return QString((QChar*)mEntry->iUnitTypeShort.Ptr(), mEntry->iUnitTypeShort.Length()); }
    QString unitLong() const { return QString((QChar*)mEntry->iUnitTypeLong.Ptr(), mEntry->iUnitTypeLong.Length()); }
    int driveNumber() const { return mEntry->iDriveNumber; }
    QColor graphColor() const { return QColor(mEntry->iGraphColor.Red(), mEntry->iGraphColor.Green(), mEntry->iGraphColor.Blue()); }

    int sampleCount() const { return mEntry->iSampleDataArray->Count(); }
    inline const SampleData& sample(int index) const { return reinterpret_cast<SampleData&>(mEntry->iSampleDataArray->At(index)); }
private:
    const TSampleEntry *mEntry;

friend class EngineWrapper;
};

class PerfMonSources
{
public:
    int count() const { return ESourcesLength; }
    int isEnabled (int index) const { return mSources.iSrcEnabled[index]; }
    void setEnabled(int index, bool enabled) { mSources.iSrcEnabled[index] = enabled; }
    QList<QVariant> enabledIndexes() const {
        QList<QVariant> indexes;
        for(int i=0; i<count(); i++) {
            if (isEnabled(i))
                indexes.append(i);
        }
        return indexes;
    }
    void setEnabledIndexes(const QList<QVariant> &indexes)
    {
        for (int i=0; i<count(); i++) {
            setEnabled(i, false);
        }
        foreach (const QVariant& index, indexes) {
        	setEnabled(index.toInt(), true);
        }
    }

private:
    PerfMonSources(TPerfMonSources &sources) : mSources(sources) {}
    friend class PerfMonSettings;

private:
    TPerfMonSources &mSources;
};

class PerfMonSettings
{
public:
    int heartBeat() const { return mSettings.iHeartBeat; }
    int maxSamples() const { return mSettings.iMaxSamples; }
    int priority() const { return mSettings.iPriority; }
    int cpuMode() const { return mSettings.iCPUMode; }
    bool keepBacklightOn() const { return mSettings.iKeepBacklightOn; }
    bool powerMonitoringEnabled() const { return mSettings.iPowerMonitoringEnabled; }

    int dataPopupVisibility() const { return mSettings.iDataPopupVisibility; }
    int dataPopupLocation() const { return mSettings.iDataPopupLocation; }
    PerfMonSources &dataPopupSources() { return mDataPopupSources; }
    const PerfMonSources &dataPopupSources() const { return mDataPopupSources; }

    int graphVerticalBarPeriod() const { return mSettings.iGraphsVerticalBarPeriod; }
    PerfMonSources &graphSources() { return mGraphSources; }
    const PerfMonSources &graphSources() const { return mGraphSources; }

    int loggingMode() const { return mSettings.iLoggingMode; }
    QString loggingFilePath() const {
        return QString((QChar*)mSettings.iLoggingFilePath.Ptr(),
                       mSettings.iLoggingFilePath.Length());
    }
    PerfMonSources &loggingSources() { return mLogSources; }
    const PerfMonSources &loggingSources() const { return mLogSources; }

    bool loggingEnabled() { return mSettings.iLoggingEnabled; }


    void setHeartBeat(int heartBeat) { mSettings.iHeartBeat = heartBeat; }
    void setMaxSamples(int maxSamples) { mSettings.iMaxSamples = maxSamples; }
    void setPriority(int priority) { mSettings.iPriority = priority; }
    void setCpuMode(int cpuMode) { mSettings.iCPUMode = cpuMode; }
    void setKeepBacklightOn(bool backlightOn) const { mSettings.iKeepBacklightOn = backlightOn; }
    void setPowerMonitoringEnabled(bool powerEnabled) const { mSettings.iPowerMonitoringEnabled = powerEnabled; }

    void setDataPopupVisibility(int visibility) { mSettings.iDataPopupVisibility = visibility; }
    void setDataPopupLocation(int location) { mSettings.iDataPopupLocation = location; }

    void setGraphVerticalBarPeriod(int period) { mSettings.iGraphsVerticalBarPeriod = period; }

    void setLoggingMode(int mode) { mSettings.iLoggingMode = mode; }
    void setLoggingFilePath(const QString& filePath) { mSettings.iLoggingFilePath = filePath.utf16(); }

private:
    // only EngineWrapper can create Settings instance
    PerfMonSettings(TPerfMonSettings &settings) :
            mSettings(settings),
            mDataPopupSources(settings.iDataPopupSources),
            mGraphSources(settings.iGraphsSources),
            mLogSources(settings.iLoggingSources)
    {}
    friend class EngineWrapper;

private:
    TPerfMonSettings &mSettings;
    PerfMonSources mDataPopupSources;
    PerfMonSources mGraphSources;
    PerfMonSources mLogSources;
};

/**
 * class that is used for communicating between Symbian and Qt code.
 */
class EngineWrapper : public QObject, public CPerfMonEngine
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

    int SampleEntryPosToSettingPos(int entryPos) const;

public slots:

    bool updateSettings();
    bool initialize();
    void finalize();
    void setLoggingEnabled(bool enabled);

signals:

    void samplesUpdated();
    void settingsUpdated();

protected:
    void SendDrawEventToContainersL();

private:
    QList<SampleEntry> mEntries;

    void createSampleEntriesArray();

    PerfMonSettings mSettings;
};

#endif //ENGINEWRAPPER_H
