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

#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <hbview.h>

class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;

class EngineWrapper;
class PerfMonSettings;
class PerfMonSources;

class SettingsView : public HbView
{
    Q_OBJECT

signals:
    void finished(bool ok);

public:
    SettingsView(EngineWrapper &engine);

private slots:
    void dataItemDisplayed(const QModelIndex &index);
    void logModeChanged(int index);
    void accept();
    void reject();

private:
    void createModel(HbDataFormModel &model);
    void load(const PerfMonSettings& settings);
    void save(PerfMonSettings& settings);
    void createLogFilePathItem();
    void removeLogFilePathItem();
    
private:
    EngineWrapper &mEngine;
    HbDataForm *mSettingsForm;
    HbDataFormModel *mModel;

    HbDataFormModelItem* mHeartBeatItem;
    HbDataFormModelItem* mMaxSamplesItem;
    HbDataFormModelItem* mPriorityItem;
    HbDataFormModelItem* mCpuSamplingItem;
    HbDataFormModelItem* mKeepBacklightItem;
    HbDataFormModelItem* mPowerMonitoringEnabledItem;
    HbDataFormModelItem* mPopupVisibilityItem;
    HbDataFormModelItem* mPopupLocationItem;
    HbDataFormModelItem* mPopupSourcesItem;
    HbDataFormModelItem* mVerticalBarPeriodItem;
    HbDataFormModelItem* mGraphSourcesItem;

    HbDataFormModelItem* mLogPage;
    HbDataFormModelItem* mLogModeItem;
    HbDataFormModelItem* mLogFilePathItem;
    HbDataFormModelItem* mLogSourcesItem;
    
    QString mLogFilePath;
};

#endif // SETTINGSVIEW_H
