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
#ifndef  CXESETTINGSCENREPSTORE_H_
#define  CXESETTINGSCENREPSTORE_H_

//  Include Files
#include <QHash>
#include <QVariant>
#include <QMetaType>

#include "cxeerror.h"
#include "cxenamespace.h"
#include "xqsettingsmanager.h"
#include "cxesettingsstore.h"

// forward declarations
class XQSettingsManager;
class XQSettingsKey;

/*!
* CxeSettingsCenRepStore class implements CxeSettingsStore.

* This class uses CenRep key mechanism for storing and retrieving settings information.
*/
class CxeSettingsCenRepStore : public QObject,
                               public CxeSettingsStore
{

    Q_OBJECT

public:

    CxeSettingsCenRepStore();
	~CxeSettingsCenRepStore();

public: // from base class

    void reset();
	CxeError::Id get(const QString &key, QVariant &value);
    void startMonitoring(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value);
	CxeError::Id set(const QString &key, const QVariant newValue);
	QHash<QString, QVariantList> loadVariationSettings(QList<QString> &settingKeys);


signals:
    void settingValueChanged(long int uid, unsigned long int key, QVariant value);

private slots:
    void handleValueChanged(XQSettingsKey key, QVariant value);

private:

    class CxeCenRepDefinition
    {
    public:
        long int mRepositoryUid;            //! UID of the all the setting keys
        unsigned long int mKeyId;           //! key cenrep id
        bool mReadOnly;                     //! if the key holds data that is read-only
        XQSettingsManager::Type mDataType;  //! type of data that cenrep key holds
        
    };

private:

    void addKeyMapping(QString key,
                       unsigned long int keyid,
                       XQSettingsManager::Type type,
                       bool readOnly = false);
    
    XQSettingsKey generateXQSettingsKey(const QString& key,CxeError::Id& error);
	void mapKeys();
	
protected:
    const QHash<QString, CxeCenRepDefinition>& keyMapping() const;

private: // data

    XQSettingsManager* mSettingsManager;
	QHash<QString, CxeCenRepDefinition> mKeyMapping;
};


/*!
 * \class CxeSettingsLocalStore
 * \brief Settings store that reads key values from cenrep and keeps
 * cached copies of them in memory. Doesn't write anything back
 * to cenrep.
 */
class CxeSettingsLocalStore : public CxeSettingsCenRepStore
{
  Q_OBJECT

public:
     CxeSettingsLocalStore();
     ~CxeSettingsLocalStore();

     CxeError::Id get(const QString& key, QVariant &value);
     CxeError::Id set(const QString& key, const QVariant newValue);

private:

     bool useValueFromCenrep(const QString &key) const;

     QHash<QString, QVariant> mSettings;
};

#endif  // CXESETTINGSCENREPSTORE_H_

