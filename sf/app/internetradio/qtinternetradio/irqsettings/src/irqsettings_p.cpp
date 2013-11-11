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
 * Description:
 *
 */

#include "irqsettings_p.h"
#include "irinternalcrkeys.h"
#include <XQSettingsKey>
#include <XQSettingsManager>
#include <QDesktopServices>
#include <QTextCodec>

const int KDefaultVolume            = 6;
const int KDefaultHttpTimeOut       = 60000000; // unit: us, 6s
const int KIRDefaultMinDiskSpace    = 3145728;  // byte, 3MB
const QString KIRDefaultIsdsUrl     = "http://idirectory.xgns.net/isds";
#ifdef USE_TEST_ISDS_SERVER
const QString KIRTestIsdsUrl        = "http://88.114.146.238/isds";
#endif

IRQSettingsPrivate::IRQSettingsPrivate() :
    mSettingsManager(new XQSettingsManager())
{
    mPrivatePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/";
}

IRQSettingsPrivate::~IRQSettingsPrivate()
{
    if (mSettingsManager)
    {
        delete mSettingsManager;
    }
}

QString IRQSettingsPrivate::privatePath()
{
    return mPrivatePath;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::isFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::isFlagTermsAndConditions(bool& aFlag)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRTermsAndConditionsFlag);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        int nFlag = value.toInt();
        aFlag = ( nFlag == 1 );         
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setFlagTermsAndConditions()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRTermsAndConditionsFlag);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, 1);
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::reSetFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::reSetFlagTermsAndConditions()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRTermsAndConditionsFlag);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, 0);
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getIRID()
//
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::getIRID(QString& aIRID)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRIrId);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        aIRID = QTextCodec::codecForName("UTF-16")->toUnicode(value.toByteArray());
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getVolumeSetting()
//
// ---------------------------------------------------------------------------
//
int IRQSettingsPrivate::getVolumeSetting()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRPlayerVolume);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    int ir = KDefaultVolume;

    if (!value.isNull())
    {
        ir = value.toInt();
    }

    return ir;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setVolumeSetting()
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setVolumeSetting(int aPlayVolume)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRPlayerVolume);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, aPlayVolume);
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setTimeOut()
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setTimeOut(int aTimeOut)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRHttpTimeout);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, aTimeOut);
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getTimeOut()
//
// ---------------------------------------------------------------------------
//
int IRQSettingsPrivate::getTimeOut()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRHttpTimeout);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    int ir = KDefaultHttpTimeOut;

    if (!value.isNull())
    {
        ir = value.toInt();
    }

    return ir;
}

void IRQSettingsPrivate::setSongHistoryShow(int aShowFlag)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRSongHistoryFTU);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, aShowFlag);
}

int IRQSettingsPrivate::getSongHistoryShow()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRSongHistoryFTU);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    int ir = 0;

    if (!value.isNull())
    {
        ir = value.toInt();
    }

    return ir;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setStartingViewId()
// Sets the starting view Id in cenrep
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setStartingViewId(int aStartingViewId)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRStartingViewId);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, aStartingViewId);
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getStartingViewId()
// Gets the starting view Id from cenrep
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::getStartingViewId(TIRViewId& aStartingViewId)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRStartingViewId);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        aStartingViewId = (TIRViewId) value.toInt();
    }
    else
    {
        aStartingViewId = EIRView_CategoryView;
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setPreferredQuality()
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setPreferredQuality(IRQPreferredQuality aQualityValue)
{
    TInt value = 0;
    switch (aQualityValue)
    {
    case EIRQHighQuality:
        value = 1;
        break;
    default:
        break;
    }

    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRPreferredQuality);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, value);
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getPreferredQuality()
//
// ---------------------------------------------------------------------------
//
IRQPreferredQuality IRQSettingsPrivate::getPreferredQuality() const
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRPreferredQuality);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    int ir = 0;

    if (!value.isNull())
    {
        ir = value.toInt();
    }

    switch (ir)
    {
    case 1:
        return EIRQHighQuality;
    default:
        return EIRQStandardQuality;
    }
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getGlobalAdvFlag()
// Gets the global advertisement flag from cenrep
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::getGlobalAdvFlag(bool& aFlag)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRGlobalAdvEnabled);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        aFlag = value.toBool();
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getManuallyInputtedStationUrl(QString& aUrl)
//
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::getManuallyInputtedStationUrl(QString& aUrl)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRManuallyInputStationUrl);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        aUrl = QTextCodec::codecForName("UTF-16")->toUnicode(value.toByteArray());
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setManuallyInputtedStationUrl(const QString& aUrl)
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setManuallyInputtedStationUrl(const QString& aUrl)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRManuallyInputStationUrl);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, QString::fromUtf16(aUrl.utf16()));
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getManuallyInputtedStationName(QString& aName)
//
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::getManuallyInputtedStationName(QString& aName)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRManuallyInputStationName);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        aName = QTextCodec::codecForName("UTF-16")->toUnicode(value.toByteArray());
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setManuallyInputtedStationName(const QString& aName)
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setManuallyInputtedStationName(const QString& aName)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRManuallyInputStationName);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, QString::fromUtf16(aName.utf16()));
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::getSearchText(QString& aSearchText)
//
// ---------------------------------------------------------------------------
//
bool IRQSettingsPrivate::getSearchText(QString& aSearchText)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRSearchText);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool br = !value.isNull();

    if (br)
    {
        aSearchText = value.toString();
    }

    return br;
}

// ---------------------------------------------------------------------------
// IRQSettingsPrivate::setSearchText(const QString& aSearchText)
//
// ---------------------------------------------------------------------------
//
void IRQSettingsPrivate::setSearchText(const QString& aSearchText)
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRSearchText);

    Q_ASSERT(mSettingsManager);
    mSettingsManager->writeItemValue(profileKey, aSearchText.toUtf8());    
}

int IRQSettingsPrivate::getMinDiskSpaceRequired()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRMinDiskSpaceRequired);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    int retVal = KIRDefaultMinDiskSpace;
        
    if (!value.isNull())
    {
        retVal = value.toInt();
    }

    return retVal;
}

bool IRQSettingsPrivate::getIdentifySongEnabled()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRIdentifySongEnabled);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    bool retVal = false;
        
    if (!value.isNull())
    {
        retVal = value.toBool();
    }

    return retVal;
}
   
int IRQSettingsPrivate::getSongRecognitionAppUid()
{
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRSongRecognitionAppUid);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    int retVal = 0;
        
    if (!value.isNull())
    {
        retVal = value.toInt();
    }

    return retVal;
}

QString IRQSettingsPrivate::getIsdsUrl()
{
#ifdef USE_TEST_ISDS_SERVER
    return KIRTestIsdsUrl;
#else    
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
            KCRUidInternetRadio.iUid, KIRIsdsUrl);

    Q_ASSERT(mSettingsManager);
    QVariant value = mSettingsManager->readItemValue(profileKey);

    QString retVal = KIRDefaultIsdsUrl;
        
    if (!value.isNull())
    {
        retVal = QTextCodec::codecForName("UTF-16")->toUnicode(value.toByteArray());;
    }

    return retVal;    
#endif    
}    
