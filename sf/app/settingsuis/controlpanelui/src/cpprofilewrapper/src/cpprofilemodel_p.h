/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef CPPROFILEMODEL_P_H
#define CPPROFILEMODEL_P_H

#include <qglobal.h>
#include <QList>
class CpProfileModel;
class CpProfileSettings;
class MProfileEngineExtended2;
class MProfileExtended2;
class MProfileFeedbackSettings;
class MProfileSetFeedbackSettings;
class CRepository;
class QStringList;
class MProfilesNamesArray;
struct TProfileToneSettings; 
class CpProfileModelPrivate
{
    Q_DECLARE_PUBLIC(CpProfileModel)
public:
    CpProfileModelPrivate();
    ~CpProfileModelPrivate();
    void initialize(CpProfileModel *parent);
public:
    QString profileName(int profileId) const;
    QStringList profileNames() const;
    void setProfileName(int profileId, const QString &name);
    int activateProfile(int profileId);
    int activeProfileId() const;
    void profileSettings(int profileId, CpProfileSettings& profileSettings);
    void setProfileSettings(int profileId, CpProfileSettings& profileSettings);

    QString ringTone() const;
    void setRingTone(const QString& filePath);

    int masterVolume() const;
    void setMasterVolume(int volume);

    bool masterVibra() const;
    void setMasterVibra(bool isVibra);

    bool silenceMode() const;
    void setSilenceMode(bool isSlience);

    QString ringTone(int profileId)const;
    void setRingTone(int profileId, const QString& filePath);
    QString messageTone(int profileId) const;
    void setMessageTone(int profileId, const QString& filePath);
    QString emailTone(int profileId) const;
    void setEmailTone(int profileId, const QString& filePath);
    QString reminderTone(int profileId) const;
    void setReminderTone(int profileId, const QString& filePath);
    
    bool notificationTone(int profileId) const;
    void setNotificationTone(int profileId, bool isActive);
    
    int keyTouchScreenTone(int profileId) const;
    void setKeyTouchScreenTone(int profileId, int level);
    
    int keyTouchScreenVibra(int profileId)const;
    void setKeyTouchScreenVibra(int profileId, int level);

private:
    bool isValidProfile(int profileId) const;
    
private:
#ifdef Q_OS_SYMBIAN
    MProfileEngineExtended2 *mEngine;
    // Valid profile id list
    QList<int> mProfileList;
    MProfilesNamesArray *mProfileNames;

#endif // Q_OS_SYMBIAN
    CpProfileModel *q_ptr;
};

#endif
