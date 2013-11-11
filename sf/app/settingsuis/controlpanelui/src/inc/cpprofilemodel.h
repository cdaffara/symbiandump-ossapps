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

#ifndef CPPROFILEMODEL_H
#define CPPROFILEMODEL_H

#include "cpprofilewrappermacro.h"
#include <QtCore/qglobal.h>
#include <QString>
#include <QtCore/QObject>
class CpProfileModelPrivate;

/*!
     Profile id is defined in profile engine
 */
enum ProfileWrapperProfileId
{
    EProfileWapperStart = -1,
    EProfileWrapperGeneralId = 0, 
    EProfileWrapperMeetingId = 2,
    EPRofileWrapperEnd    
};
class CpProfileSettings
{
public:
    QString mRingTone;
    QString mMessageTone;
    QString mEmailTone;
    QString mReminderTone;
    bool mNotificationTone;
    int mKeyTouchScreenTone; // 0-5
    
    int mKeyTouchScreenVibra; //0-5    
};
class PROFILE_WRAPPER_EXPORT CpProfileModel : QObject
{
    Q_OBJECT
public:
    CpProfileModel(QObject *parent = 0);
    ~CpProfileModel();

public:
    QString profileName(int profileId)const;
    void setProfileName(int profileId, const QString &name);
    QStringList profileNames()const;
    int activateProfile(int profileId);
    int activeProfileId() const;
    
    void profileSettings(int profileId, CpProfileSettings& profileSettings);
    void setProfileSettings(int profileId, CpProfileSettings& profileSettings );
    
    QString ringTone() const;
    void setRingTone(const QString& filePath);
    
    int masterVolume() const;
    void setMasterVolume(int volume);
    
    
    bool masterVibra() const;
    void setMasterVibra(bool isVibra);
        
    bool silenceMode() const;
    void setSilenceMode(bool isSlience);
    
    /*!
     *  For profile settings 
     */
    
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
    CpProfileModelPrivate *const d_ptr;

private:    
    Q_DISABLE_COPY(CpProfileModel)
    Q_DECLARE_PRIVATE_D(d_ptr,CpProfileModel)
};

#endif
