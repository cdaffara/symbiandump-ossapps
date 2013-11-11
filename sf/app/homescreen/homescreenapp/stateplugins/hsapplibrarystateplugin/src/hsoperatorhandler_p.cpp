/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: hsoperatorhandler_p.cpp
 *
 */
#ifdef QT_EXTENSIONS

#include <QDebug>
#include <XQSettingsKey>
#include "hsoperatorhandler_p.h"
#include "hsmenuservice.h"
#include "caentry.h"
#include "caservice.h"

const int AppLibUid                 = 0x20022F97;
const int IncludeOperatorStore      = 0x00000001;
const int OperatorStoreType         = 0x00000002;
const int OperatorStoreTitle        = 0x00000003;
const int OperatorStoreURL          = 0x00000004;
const int OperatorStoreIconSkinID   = 0x00000005;
const int OperatorStoreApplication  = 0x00000006;
const int OperatorStore1stPopUp     = 0x00000007;
const int RemoveOviStore            = 0x00000008;
const char *operatorStoreIconPath
        = "z:/private/20022F35/customsvg/operatorstore.svg";


// ======== MEMBER FUNCTIONS ========

/*!
 Constructor
 \param parent parent object
 */
HsOperatorHandlerPrivate::HsOperatorHandlerPrivate(QObject *parent):
    QObject(parent),
    mOviStorePresent(false),
    mOperatorStorePresent(false),
    mStoreType(NoneType),
    mOperatorEntry(0),
    mIcon(),
    mOperatorStoreTitle(),
    mOperatorStoreFirst(false)
{
    readCentralRepository();
}

/*!
 Destructor
 */
HsOperatorHandlerPrivate::~HsOperatorHandlerPrivate()
{
    delete mOperatorEntry;
}

/*!
 Executes action
 */
void HsOperatorHandlerPrivate::action()
{
    qDebug("HsOperatorHandlerPrivate::action");
    CaService::instance()->executeCommand(*mOperatorEntry);
}

/*!
 Returns Operator icon predefined in Central Repositoryicon
 \retval icon
 */
HbIcon HsOperatorHandlerPrivate::icon()
{
    return mIcon;
}

/*!
 Returns Operator title text predefined in Central Repositoryicon
 \retval text
 */
QString HsOperatorHandlerPrivate::text()
{
    return mOperatorStoreTitle;
}

/*!
 Returns Ovi Store presence value predefined in Central Repositoryicon
 \retval true if Ovi Store should be present in toolbar
 */
bool HsOperatorHandlerPrivate::oviStorePresent()
{
    return mOviStorePresent;
}

/*!
 Returns Operator Store presence value predefined in Central Repositoryicon
 \retval true if Operator Store should be present in toolbar
 */
bool HsOperatorHandlerPrivate::operatorStorePresent()
{
    return mOperatorStorePresent;
}

/*!
 Returns if Operator Store should first in toolbar
 \retval true if Operator Store should first in toolbar
 */
bool HsOperatorHandlerPrivate::operatorStoreFirst()
{
    return mOperatorStoreFirst;
}

/*!
 Creates icon based on Central Repository
 \param crManager settings manager
 \retval icon
 */
HbIcon HsOperatorHandlerPrivate::createIcon(XQSettingsManager *crManager)
{
    QVariant variant = getVariantFromKey(
            crManager,
            OperatorStoreIconSkinID,
            XQSettingsManager::TypeString);
    if (!variant.isNull() && !variant.toString().isEmpty()) {
        return HbIcon(variant.toString());
    } else {
        return HbIcon(operatorStoreIconPath);
    }
}

/*!
 Creates application entry based on Central Repository
 \param crManager settings manager
 \retval entry
 */
CaEntry *HsOperatorHandlerPrivate::createAppEntry(
    XQSettingsManager *crManager)
{
    QVariant variant = getVariantFromKey(
        crManager, OperatorStoreApplication, XQSettingsManager::TypeString);
    if (!variant.isNull()) {
        CaEntry *operatorEntry = new CaEntry;
        operatorEntry->setEntryTypeName(Hs::applicationTypeName);
        // Convert from a hexadecimal integer into a decimal one (as a string).
        operatorEntry->setAttribute(
            Hs::applicationUidEntryKey,
            QString::number(variant.toString().toInt(0, 16)));
        return operatorEntry;
    }
    return NULL;
}

/*!
 Creates url entry based on Central Repository
 \param crManager settings manager
 \retval entry
 */
CaEntry *HsOperatorHandlerPrivate::createUrlEntry(
    XQSettingsManager *crManager)
{
    QVariant variant = getVariantFromKey(
            crManager, OperatorStoreURL, XQSettingsManager::TypeString);
    if (!variant.isNull()) {
        CaEntry *operatorEntry = new CaEntry;
        operatorEntry->setEntryTypeName(Hs::urlTypeName);
        operatorEntry->setAttribute(Hs::urlEntryKey, variant.toString());

        return operatorEntry;
    }
    return NULL;
}


/*!
 Generic function to get stores presence values
 \param crManager settings manager
 \param storePresenceKey store key(Ovi or Operator)
 \retval entry
 */
bool HsOperatorHandlerPrivate::storeValue(
        XQSettingsManager *crManager, int storePresenceKey)
{
    QVariant variant = getVariantFromKey(
            crManager, storePresenceKey, XQSettingsManager::TypeInt);
    if (!variant.isNull()) {
        return bool(variant.toInt());
    }
    return false;
}

/*!
 Generic function to get QVariant for CR key
 \param crManager settings manager
 \param crKey CR key
 \param type type of QVariant to be returned
 \retval QVariant
 */
QVariant HsOperatorHandlerPrivate::getVariantFromKey(
        XQSettingsManager *crManager, int crKey, XQSettingsManager::Type type)
{
    XQCentralRepositorySettingsKey presenceKey(AppLibUid, crKey);
    QVariant presenceVariant = crManager->readItemValue(
            presenceKey, type);

    if (presenceVariant.isValid()) {
        qDebug() << "HsOperatorHandlerPrivate::storePresent"
                 << presenceVariant;
        return presenceVariant;
    }
    return QVariant();
}

/*!
 Generic function to get store type(Url/App)
 \param crManager settings manager
 \retval HsStoreType store type
 */
HsOperatorHandlerPrivate::HsStoreType
        HsOperatorHandlerPrivate::operatorStoreType(
                XQSettingsManager *crManager)
{
    QVariant variant = getVariantFromKey(
            crManager, OperatorStoreType, XQSettingsManager::TypeInt);
    if (!variant.isNull()) {
        return HsOperatorHandlerPrivate::HsStoreType(variant.toInt());
    }
    return NoneType;
}

/*!
 Creates text based on Central Repository
 \param crManager settings manager
 \retval icon
 */
QString HsOperatorHandlerPrivate::operatorStoreTitle(XQSettingsManager *crManager)
{
    QVariant variant = getVariantFromKey(
            crManager, OperatorStoreTitle, XQSettingsManager::TypeString);
    if (!variant.isNull()) {
        return variant.toString();
    }
    return QString();
}

/*!
 Retrives information if Operator Store should first in toolbarbased
 (from Central Repository)
 \param crManager settings manager
 \retval true if Operator Store should be first
 */
bool HsOperatorHandlerPrivate::operatorStoreFirst(XQSettingsManager *crManager)
{
    QVariant variant = getVariantFromKey(
            crManager, OperatorStore1stPopUp, XQSettingsManager::TypeInt);
    if (!variant.isNull()) {
        return bool(variant.toInt());
    }
    return false;
}

/*!
 Reads Central Repository
 */
void HsOperatorHandlerPrivate::readCentralRepository()
{
    XQSettingsManager *crManager = new XQSettingsManager;
    //this key indicates that ovi store should be removed
    mOviStorePresent = !storeValue(crManager, RemoveOviStore);
    //this key indicates that operator store should be added
    mOperatorStorePresent = storeValue(crManager, IncludeOperatorStore);

    if (mOperatorStorePresent) {
        mStoreType = operatorStoreType(crManager);
        if (mStoreType == ApplicationType) {
            mOperatorEntry = createAppEntry(crManager);
        } else {
            mOperatorEntry = createUrlEntry(crManager);
        }
        mOperatorStoreTitle = operatorStoreTitle(crManager);
        mOperatorStoreFirst = operatorStoreFirst(crManager);
        mIcon = createIcon(crManager);
    }
    delete crManager;
}
#endif
