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

#include "irqsettings.h"
#include "irqsettings_p.h"

IRQSettings * IRQSettings::mInstance = NULL;
QMutex IRQSettings::mMutex;

// ---------------------------------------------------------------------------
// IRQSettings::openInstance()
// Static function to get a singleton instance of IRQSettings
// @return IRQSettings *
// ---------------------------------------------------------------------------
//
IRQSettings* IRQSettings::openInstance()
{
    mMutex.lock();

    if (NULL == mInstance)
    {
        mInstance = new IRQSettings();
    }
    else
    {
        mInstance->mRefCount++;
    }

    mMutex.unlock();

    return mInstance;
}

// ---------------------------------------------------------------------------
// IRQSettings::closeInstance()
// Close a singleton instance of IRQSettings
// ---------------------------------------------------------------------------
//
void IRQSettings::closeInstance()
{
    mMutex.lock();
    if ((--mRefCount) == 0)
    {
        if (this == mInstance)
        {
            mInstance = NULL;
        }
        delete this;
    }
    mMutex.unlock();
}

// ---------------------------------------------------------------------------
// IRQSettings::isFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
bool IRQSettings::isFlagTermsAndConditions(bool& aFlag)
{
    return d_ptr->isFlagTermsAndConditions(aFlag);
}

// ---------------------------------------------------------------------------
// IRQSettings::setFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setFlagTermsAndConditions()
{
    d_ptr->setFlagTermsAndConditions();
}

// ---------------------------------------------------------------------------
// IRQSettings::reSetFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
void IRQSettings::reSetFlagTermsAndConditions()
{
    d_ptr->reSetFlagTermsAndConditions();
}

// ---------------------------------------------------------------------------
// IRQSettings::getIRID()
//
// ---------------------------------------------------------------------------
//
bool IRQSettings::getIRID(QString& aIRID)
{
    return d_ptr->getIRID(aIRID);
}

// ---------------------------------------------------------------------------
// IRQSettings::getVolumeSetting()
//
// ---------------------------------------------------------------------------
//
int IRQSettings::getVolumeSetting()
{
    return d_ptr->getVolumeSetting();
}

// ---------------------------------------------------------------------------
// IRQSettings::setVolumeSetting()
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setVolumeSetting(int aPlayVolume)
{
    d_ptr->setVolumeSetting(aPlayVolume);
}

// ---------------------------------------------------------------------------
// IRQSettings::setTimeOut()
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setTimeOut(int aTimeOut)
{
    d_ptr->setTimeOut(aTimeOut);
}

// ---------------------------------------------------------------------------
// IRQSettings::getTimeOut()
//
// ---------------------------------------------------------------------------
//
int IRQSettings::getTimeOut()
{
    return d_ptr->getTimeOut();
}

// ---------------------------------------------------------------------------
// IRQSettings::timeCorrection()
//
// ---------------------------------------------------------------------------
//
QString IRQSettings::privatePath()
{
    return d_ptr->privatePath();
}

void IRQSettings::setSongHistoryShow(int aShowFlag)
{
    d_ptr->setSongHistoryShow(aShowFlag);
}

int IRQSettings::getSongHistoryShow()
{
    return d_ptr->getSongHistoryShow();
}

// ---------------------------------------------------------------------------
// IRQSettings::setStartingViewId()
// Sets the starting view Id in cenrep
// ---------------------------------------------------------------------------
//
void IRQSettings::setStartingViewId(int aStartingViewId)
{
    d_ptr->setStartingViewId(aStartingViewId);
}

// ---------------------------------------------------------------------------
// IRQSettings::getStartingViewId()
// Gets the starting view Id from cenrep
// ---------------------------------------------------------------------------
//
bool IRQSettings::getStartingViewId(TIRViewId& aStartingViewId)
{
    return d_ptr->getStartingViewId(aStartingViewId);
}

// ---------------------------------------------------------------------------
// IRQSettings::setPreferredQuality()
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setPreferredQuality(IRQPreferredQuality aQualityValue)
{
    d_ptr->setPreferredQuality(aQualityValue);
}

// ---------------------------------------------------------------------------
// IRQSettings::getPreferredQuality()
//
// ---------------------------------------------------------------------------
//
IRQPreferredQuality IRQSettings::getPreferredQuality() const
{
    return d_ptr->getPreferredQuality();
}

// ---------------------------------------------------------------------------
// IRQSettings::getGlobalAdvFlag()
// Gets the global advertisement flag from cenrep
// ---------------------------------------------------------------------------
//
bool IRQSettings::getGlobalAdvFlag(bool& aFlag)
{
    return d_ptr->getGlobalAdvFlag(aFlag);
}

// ---------------------------------------------------------------------------
// IRQSettings::getManuallyInputtedStationUrl(QString& aUrl)
//
// ---------------------------------------------------------------------------
//
bool IRQSettings::getManuallyInputtedStationUrl(QString& aUrl)
{
    return d_ptr->getManuallyInputtedStationUrl(aUrl);
}

// ---------------------------------------------------------------------------
// IRQSettings::setManuallyInputtedStationUrl(const QString& aUrl)
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setManuallyInputtedStationUrl(const QString& aUrl)
{
    d_ptr->setManuallyInputtedStationUrl(aUrl);
}

// ---------------------------------------------------------------------------
// IRQSettings::getManuallyInputtedStationName(QString& aName)
//
// ---------------------------------------------------------------------------
//
bool IRQSettings::getManuallyInputtedStationName(QString& aName)
{
    return d_ptr->getManuallyInputtedStationName(aName);
}

// ---------------------------------------------------------------------------
// IRQSettings::setManuallyInputtedStationName(const QString& aName)
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setManuallyInputtedStationName(const QString& aName)
{
    d_ptr->setManuallyInputtedStationName(aName);
}

// ---------------------------------------------------------------------------
// IRQSettings::getSearchText(QString& aSearchText)
//
// ---------------------------------------------------------------------------
//
bool IRQSettings::getSearchText(QString& aSearchText)
{
    return d_ptr->getSearchText(aSearchText);
}

// ---------------------------------------------------------------------------
// IRQSettings::setSearchText(const QString& aSearchText)
//
// ---------------------------------------------------------------------------
//
void IRQSettings::setSearchText(const QString& aSearchText)
{
    d_ptr->setSearchText(aSearchText);
}

int IRQSettings::getMinDiskSpaceRequired()
{
    return d_ptr->getMinDiskSpaceRequired();
}

bool IRQSettings::getIdentifySongEnabled()
{
    return d_ptr->getIdentifySongEnabled();    
}
    
int IRQSettings::getSongRecognitionAppUid()
{
    return d_ptr->getSongRecognitionAppUid();
}

QString IRQSettings::getIsdsUrl()
{
    return d_ptr->getIsdsUrl();
}    
    
// ---------------------------------------------------------------------------
// IRQSettings::IRQSettings()
// Constructor
// ---------------------------------------------------------------------------
//
IRQSettings::IRQSettings() :
    d_ptr(new IRQSettingsPrivate()), mRefCount(1)
{
}

// ---------------------------------------------------------------------------
// IRQSettings::~IRQSettings()
// Destructor
// ---------------------------------------------------------------------------
//
IRQSettings::~IRQSettings()
{
    delete d_ptr;
}

