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
#ifndef CPPERSONALIZATIONENTRYITEMDATA_H
#define CPPERSONALIZATIONENTRYITEMDATA_H

#include <cpsettingformentryitemdata.h>
#include <xqsettingskey.h>

class CpProfileModel;
class XQSettingsManager;

class CpPersonalizationEntryItemData : public CpSettingFormEntryItemData
{
    Q_OBJECT
public:
        typedef enum
        {
            TONE_Ring = 0,
            TONE_Message,
            TONE_Email,
            TONE_Reminder
        }Profile_Tone_Types;

        explicit CpPersonalizationEntryItemData(CpItemDataHelper &itemDataHelper,
                const QString &text = QString(),
                const QString &description = QString(),
                const QString &icon = QString(),
                Profile_Tone_Types toneType = TONE_Ring,
                int profileId = -1,
                const HbDataFormModelItem *parent = 0);
        virtual ~CpPersonalizationEntryItemData();
private slots:
        void handleOk(const QString &strFname);
        void handleError( int errorCode, const QString& errorMessage );
        void settingValueChanged( const XQSettingsKey& key, const QVariant& value );
private:
        virtual CpBaseSettingView *createSettingView() const;
        QString loadStringValue() const;
        void storeStringValue( const QString &strvalue ) const;
private:
        CpProfileModel *mProfileModel;
        int m_profileID;
        Profile_Tone_Types mToneType;
        XQSettingsManager *mSettingManager;
};

#endif // CPPERSONALIZATIONENTRYITEMDATA_H
