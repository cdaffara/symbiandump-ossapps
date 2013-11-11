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
* Description:  Model for handling indicator data.
*
*/

#ifndef SNSRINDICATORMODEL_H
#define SNSRINDICATORMODEL_H

#include <qobject.h>
#include "snsrindicatorinfo.h"
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrIndicators)

class HbIndicatorInterface;
#ifdef Q_OS_SYMBIAN
class XQSettingsManager;
class XQSettingsKey;
#endif //Q_OS_SYMBIAN

class SnsrIndicatorModel : public QObject
{
    Q_OBJECT

public:
    
    SnsrIndicatorModel(QObject *parent = 0);
    virtual ~SnsrIndicatorModel();
    
public slots:
    
    void handleActiveIndicators(const QList<HbIndicatorInterface*> &activeIndicators);
    void handleActivatedIndicator(HbIndicatorInterface *activatedIndicator);
    void handleDeactivatedIndicator(HbIndicatorInterface *deactivatedIndicator);
    
    void handleUpdatedIndicator();
    
    void initializeIndicatorWidget();
    
#ifdef Q_OS_SYMBIAN
    void offlineValueChanged( const XQSettingsKey &key, const QVariant &value);    
#endif // Q_OS_SYMBIAN
    
signals:

    void indicatorsUpdated(const QList<SnsrIndicatorInfo> &activeIndicators);
    void allIndicatorsDeactivated();

private:

    bool addIndicator(const SnsrIndicatorInfo &indicatorInfo);
    bool isIndicatorAlreadyAdded(const SnsrIndicatorInfo &indicatorInfo) const;
    
    bool findAndRemoveIndicator(const SnsrIndicatorInfo &indicatorInfo);
   
    bool showIndicatorInScreensaver(const HbIndicatorInterface &indicatorInterface,
                                    SnsrIndicatorInfo &indicatorInfo);
    
    void connectToIndicatorsUpdateSignal(const HbIndicatorInterface &indicatorInterface);

    bool anyActiveIndicators() const;
    
    void emitChangeSignal();
    void updateIndicatorsToShowList();
    
    void initializeOfflineModeIndication();
    void getCurrentOfflineState();
 
private:
    
    bool mOfflineStateOn;
    SnsrIndicatorInfo mOfflineIndicatorInfo;
#ifdef Q_OS_SYMBIAN
    XQSettingsKey *mOfflineKey;
    XQSettingsManager *mSettingsManager;
#endif //Q_OS_SYMBIAN
    
    // Save universal indicator statuses into these 2 lists.
    // Active notification indicators in reversed chronological order:
    // the first item is the most recent.
    QList<SnsrIndicatorInfo> mNotificationIndicators;
    // Screensaver should show only silent indicator from this category.
    QList<SnsrIndicatorInfo> mSettingIndicators;
    
    // This is a combined ordered list of all indicators that should
    // be shown in UI: offline + notification indicators +
    // settings indicators
    QList<SnsrIndicatorInfo> mIndicatorsToShow;

    Q_DISABLE_COPY(SnsrIndicatorModel)
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrIndicators)
};

#endif // SNSRINDICATORMODEL_H

