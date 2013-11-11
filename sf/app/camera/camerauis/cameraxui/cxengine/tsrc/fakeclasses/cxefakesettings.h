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
#ifndef CXEFAKESETTINGS_H
#define CXEFAKESETTINGS_H

#include "cxesettings.h"
#include "cxeerror.h"

class CxeFakeSettings : public CxeSettings
{

    Q_OBJECT

    public:
        CxeFakeSettings();
        ~CxeFakeSettings();

    public: // from CxeSettings

        void get(long int uid,
                 unsigned long int key,
                 Cxe::SettingKeyType type,
                 QVariant &value) const;
        void reset();
        CxeError::Id getVariationValue(const QString &key, QVariant &value);
        bool listenForSetting(const QString &settingKey, QObject *target, const char *slot);

    protected:
        void getValue(const QString &key, QVariant &value) const;
        void setValue(const QString &key, const QVariant &newValue);

    public: // methods for unit testing
        void emulate(long int uid, unsigned long int key, QVariant value);
        
    private:
        QHash<QString, QVariantList> loadVariationSettings();

    private: // data
        QHash<QString, QVariant> mSettingKeyHash;
        QHash<QString, unsigned long int> mVariationKeyHash;
        QHash<QString, QVariantList> mVariationSettings;

    private:
        Q_DISABLE_COPY(CxeFakeSettings)
};

#endif /*CXEFAKESETTINGS_H*/

// end  of file
