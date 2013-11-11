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
#ifndef  CXESETTINGSSTORE_H_
#define  CXESETTINGSSTORE_H_

//  Include Files
#include <QHash>
#include <QVariant>
#include <QMetaType>
#include "cxeerror.h"
#include "cxenamespace.h"

/*!
* Settings store intrerface.
*/
class CxeSettingsStore
{
public:

    /*!
     * Destructor. Needs to be virtual.
    */
    virtual ~CxeSettingsStore() {};

    /*!
    * resets the cenrep store
    */
    virtual void reset() = 0;
    
    /*!
    * Reads a value from cenrep
    * \param "key"   - setting key
    * \param "value" - setting value read from cenrep
    */
	virtual CxeError::Id get(const QString &key, QVariant &value) = 0;

    /*!
    * Reads a value from the settings store and starts monitoring it.
    * \param "uid"   - UID of the component that own setting key
    * \param "key"   - setting key id
    * \param "type"  - type of setting key
    * \param "value" - setting value read from cenrep
    */
    virtual void startMonitoring(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) = 0;

   /*!
    * Sets a new value to cenrep
    * \param "key"   - setting key
    * \param "newValue" - new value set to the key in cenrep
    */
	virtual CxeError::Id set(const QString &key,const QVariant newValue) = 0;
	    
	/*!
    * Reads/loads all run-time variation settings values from cenrep
    * @param QList<QString> contains list of all runtime variation key ids which we use to load values from cenrep.
    * returns: QHash container, "contains" values associated with each key that are read from cenrep
    * NOTE: loading runtime variation settings should be done only ONCE at start-up. Its an overhead to read variation keys
    *       unnecessarily multiple times as the values of the runtime keys are not changed.
    *       Variation keys are only used to configure camerax application.
    */
	virtual QHash<QString, QVariantList> loadVariationSettings(QList<QString> &settingKeys) = 0;
};


#endif  // CXESETTINGSSTORE_H_

