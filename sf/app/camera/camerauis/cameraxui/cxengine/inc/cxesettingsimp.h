/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXESETTINGSIMP_H
#define CXESETTINGSIMP_H

#include <QMetaMethod>
#include "cxesettings.h"
#include "cxeerror.h"
#include "cxescenemodestore.h"

// forward declaration
class CxeSettingsStore;


typedef QPair<QObject*, QMetaMethod> CxeSettingListener;
typedef QList<CxeSettingListener> CxeSettingListenerList;

/*
* Class to access all kind of Settings
*/
class CxeSettingsImp : public CxeSettings
{
    Q_OBJECT

public:

    CxeSettingsImp(CxeSettingsStore *settingStore);
    virtual ~CxeSettingsImp();

    void get(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) const;
    void reset();
    CxeError::Id getVariationValue(const QString &key, QVariant &value);
    bool listenForSetting(const QString &settingKey, QObject *target, const char *slot);

public slots:
    void loadSettings(Cxe::CameraMode mode);

protected:
    void getValue(const QString &key, QVariant &value) const;
    void setValue(const QString &key, const QVariant &newValue);

private: // helper methods

    void restoreImageSettings();
    void restoreVideoSettings();

    void setImageScene(const QString &newScene);
    void setVideoScene(const QString &newScene);

    void loadVariationSettings();

    void notifyListeners(const QString &settingKey, const QVariant &newValue);

private slots:
    void handleListenerDestroyed(QObject *object);

private:

    CxeSettingsStore *mSettingStore;
    QHash<QString, QVariantList> mVariationSettings;

    CxeSceneModeStore mSceneModeStore;
    Cxe::CameraMode mCameraMode;

    QMap<QString, CxeSettingListenerList> mSettingListeners;
};

#endif // CXESETTINGSIMP_H

// end  of file
