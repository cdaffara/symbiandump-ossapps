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
#ifndef IRQSETTINGS_H_
#define IRQSETTINGS_H_

#include "irqsettingsexport.h"
#include "irqenums.h"
#include "irviewdefinitions.h"
#include <QString>
#include <QMutex>

class IRQSettingsPrivate;

/**
 * This class provides the interface to IR settings component
 *
 * IRQSettings follows a singleton pattern
 * hence the destructor is private and to destroy it we need to use an API
 *
 */

class IRQSETTINGS_DLL_EXPORT IRQSettings
{
public:

    /**
     *  Get the instance of IRQSettings
     *  @return IRQSettings*
     */
    static IRQSettings* openInstance();
    void closeInstance();

    bool isFlagTermsAndConditions(bool& aFlag);
    void setFlagTermsAndConditions();
    void reSetFlagTermsAndConditions();

    bool getIRID(QString& aIRID);
    int getVolumeSetting();
    void setVolumeSetting(int aPlayVolume);
    void setTimeOut(int aTimeOut);
    int getTimeOut();
    QString privatePath();
    int getSongHistoryShow();
    void setSongHistoryShow(int aShowFlag);

    /**
     * setStartingViewId()
     * Sets the starting view Id in cenrep
     */
    void setStartingViewId(int aStartingViewId);

    /**
     * getStartingViewId()
     * Gets the starting view Id from cenrep
     */
    bool getStartingViewId(TIRViewId& aStartingViewId);

    void setPreferredQuality(IRQPreferredQuality aQualityValue);

    /**
     * getPreferredQuality()
     * Gets the Preferred Quality from cenrep
     */
    IRQPreferredQuality getPreferredQuality() const;

    /**
     * getGlobalAdvFlag()
     * Gets the global adv flag from cenrep. True: enable NAE adv
     */
    bool getGlobalAdvFlag(bool& aFlag);

    bool getManuallyInputtedStationUrl(QString& aUrl);
    void setManuallyInputtedStationUrl(const QString& aUrl);
    bool getManuallyInputtedStationName(QString& aName);
    void setManuallyInputtedStationName(const QString& aName);
    bool getSearchText(QString& aSearchText);
    void setSearchText(const QString& aSearchText);
    
    int getMinDiskSpaceRequired();    
    bool getIdentifySongEnabled();
    int getSongRecognitionAppUid();
    QString getIsdsUrl();
    
private:

    /**
     *  Default constructor must be private
     */
    IRQSettings();

    ~IRQSettings();

private:

    /**
     *  IRQSettingsPrivate instance
     */
    IRQSettingsPrivate* const d_ptr;

    /**
     * Number of objects currently referring to the singleton object IRQSettings
     */
    int mRefCount;

    /**
     * The instance of IRQSettings singleton
     */
    static IRQSettings *mInstance;

    /**
     * Mutex for the thread-safe of openInstance()
     */
    static QMutex mMutex;
};

#endif /* IRQSETTINGS_H_ */
