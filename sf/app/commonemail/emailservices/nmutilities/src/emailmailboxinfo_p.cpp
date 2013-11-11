/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  mailbox branding object
 *
 */

#include "nmutilitiescommonheaders.h"

// data type match the types used by the corresponding APIs
const long int NmEmailCenrep = 0x2001E277;
const unsigned long int NmWlbBrandName = 0xfffffffb;
const unsigned long int NmWlbMailboxName = 0xfffffffc;
const unsigned long int NmWlbIconPath = 0xfffffffd;

const unsigned long int NmPartialKey = 0x0;
const unsigned long int NmBitMask = 0x0F000000;

EmailMailboxInfoPrivate* EmailMailboxInfoPrivate::mSelf = NULL;
int EmailMailboxInfoPrivate::mReferenceCount = 0;

const QString NmBrandNameGmail("Gmail");
const QString NmBrandNameGoogleMail("Google Mail");

const QString NmMCCGermany("262");
const QString NmMCCUK1("234");
const QString NmMCCUK2("235");

const quint8 NmGermanyTzId = 36;
const quint8 NmUKTzId = 104;

using namespace QtMobility;

/*!
     private constructor
 */
EmailMailboxInfoPrivate::EmailMailboxInfoPrivate() :
    QObject(NULL)
{
    NM_FUNCTION;
    
    XQSettingsManager manager;

    XQCentralRepositorySettingsKey wlbKey(NmEmailCenrep, NmWlbBrandName);

    mWlbDomainName = manager.readItemValue(wlbKey, XQSettingsManager::TypeString).value<QString> ();    
}
/*!
     private destructor
 */
EmailMailboxInfoPrivate::~EmailMailboxInfoPrivate()
{
    NM_FUNCTION;
}

/*!
    gets instance of EmailMailboxInfoPrivate
    \return instance of EmailMailboxInfoPrivate
 */
EmailMailboxInfoPrivate* EmailMailboxInfoPrivate::getInstance()
{
    NM_FUNCTION;
    
    if (!mSelf) {
        mSelf = new EmailMailboxInfoPrivate();
    }
    mReferenceCount++;
    return mSelf;
}

/*!
    releases pointer to instance of EmailMailboxInfoPrivate
    \param pointer to instance of EmailMailboxInfoPrivate object
 */
void EmailMailboxInfoPrivate::releaseInstance(EmailMailboxInfoPrivate *&instance)
{
    NM_FUNCTION;
    
    if (instance) {
        mReferenceCount--;
        instance = NULL;
    }
    if (mReferenceCount < 1) {
        delete mSelf;
        mSelf = NULL;
    }
}

/*!
    returns branding name of given domain
    \param branding identifier
    \return branding name
 */
QString EmailMailboxInfoPrivate::name(const QString &identifier)
{
    NM_FUNCTION;
    
    QString returnValue = "";

    if (identifier.length() > 0) {
        processCenRepRecords(identifier);
        returnValue = mTempName;
    }

    verifyMailAccountName(returnValue);

    return returnValue;
}

/*!
    returns branding icon of given domain
    \param branding identifier
    \return branding icon
 */
QString EmailMailboxInfoPrivate::icon(const QString &identifier)
{
    NM_FUNCTION;
    
    QString returnValue = "";

    if (identifier.length() > 0) {
        processCenRepRecords(identifier);
        returnValue = mTempIcon;
    }

    return returnValue;
}

/*!
    goes through cenrep to find matching branding details
    \param brandingId (i.e. domain name)
 */
void EmailMailboxInfoPrivate::processCenRepRecords(const QString &brandingId)
{
    NM_FUNCTION;
    
    bool found = false;
    QString name;
    QString icon;

    XQSettingsManager cenRepManager;

    if (brandingId == mWlbDomainName) {
        found = true;
        XQCentralRepositorySettingsKey wlbNameKey(NmEmailCenrep, NmWlbMailboxName);
        XQCentralRepositorySettingsKey wlbIconKey(NmEmailCenrep, NmWlbIconPath);

        icon = cenRepManager.readItemValue(
            wlbIconKey, XQSettingsManager::TypeString).value<QString> ();
        
        name = cenRepManager.readItemValue(
            wlbNameKey, XQSettingsManager::TypeString).value<QString> ();
    }
    else {
        XQCentralRepositorySearchCriteria sCriteria(NmEmailCenrep, NmPartialKey, NmBitMask);
        XQCentralRepositoryUtils cenrepUtils(cenRepManager);
        QList<XQCentralRepositorySettingsKey> foundKeys = cenrepUtils.findKeys(sCriteria);

        int count = foundKeys.count();
        for (int i = 0; i < count && !found; i++) {
            XQCentralRepositorySettingsKey key = foundKeys[i];

            QString dataRow = "";
            if (mBrandingDataMap.contains(key.key())) {
                dataRow = mBrandingDataMap.value(key.key());
            }
            //first we put every spotted data row to map
            else {
                QVariant brandingDataRaw = cenRepManager.readItemValue(
                    key, XQSettingsManager::TypeString);

                dataRow = brandingDataRaw.toString(); // empty if cannot be converted
                mBrandingDataMap.insert(key.key(), dataRow);
            }

            // then we check if this row contains matching data
            QStringList cenRepRecord = dataRow.split(";");

            if (cenRepRecord.size() >= 3) {

                QRegExp regExp(cenRepRecord.at(0));
                regExp.setCaseSensitivity(Qt::CaseInsensitive);
    
                if (regExp.exactMatch(brandingId)) { //match
                    found = true;
                    icon = "z:/resource/apps/" + cenRepRecord.at(2) + ".svg";
                    name = cenRepRecord.at(1);
                }
            }
        }
    }
    if (!found ) { 
        //get default icon and name
        icon = "qtg_large_email";
        // If "." does not match anywhere in the string, split() returns a single-element list 
        // containing brandingId string => no safety check needed
        QStringList domain = brandingId.split(".");
        name = domain.at(0);
    }
    mTempIcon = icon;
    mTempName = name;
}

/*!
    gets current country code
    \return current country id
 */
int EmailMailboxInfoPrivate::getCurrentCountryL() const
{
    NM_FUNCTION;
    
    int countryId = NmNotFoundError;
    
    CTzLocalizer* localizer = CTzLocalizer::NewLC();

    CTzLocalizedCity* city = localizer->GetFrequentlyUsedZoneCityL(
        CTzLocalizedTimeZone::ECurrentZone);
    
    if (city) {
        CleanupStack::PushL(city);

        CTzLocalizedCityGroup* cityGroup = localizer->GetCityGroupL(city->GroupId());
        
        if (cityGroup) {
            countryId = cityGroup->Id();

            delete cityGroup;
            cityGroup = NULL;
            
        }
        CleanupStack::PopAndDestroy(city);
    }

    CleanupStack::PopAndDestroy(localizer);

    return countryId;
}

/*!
    verifies if timezone is set for Germany or UK
    \return true if timezone is UK or Germany, false otherwise
 */
bool EmailMailboxInfoPrivate::verifyTimeZone() const
{
    NM_FUNCTION;
    
    quint8 timeZone = 0;
    bool retVal = false;
    TRAPD(err, timeZone = getCurrentCountryL());

    if (err == KErrNone && (timeZone == NmGermanyTzId || timeZone == NmUKTzId)) {
        retVal = true;
    }
    return retVal;
}

/*!
    modifies branding name "Gmail" according to UK and Germany law rules
    \param brandingName name of branding
 */
void EmailMailboxInfoPrivate::verifyMailAccountName(QString &brandingName) const
{
    NM_FUNCTION;
    
    if (brandingName == NmBrandNameGmail) {
        QSystemNetworkInfo *networkInfo = new QSystemNetworkInfo();
        QString currentMCC = networkInfo->currentMobileCountryCode();

        if ((currentMCC == NmMCCGermany) || (currentMCC == NmMCCUK1) || (currentMCC == NmMCCUK2)) {
            brandingName = NmBrandNameGoogleMail;
        }
        else if (verifyTimeZone()) { //if there is information (no sim)
            brandingName = NmBrandNameGoogleMail;
        }
    }
}

