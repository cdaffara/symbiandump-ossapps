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

#ifndef LOGSCOMMONDATA_H
#define LOGSCOMMONDATA_H

#include <qmobilityglobal.h>
#include "logsevent.h"
#include "logsconfigurationparams.h"

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class XQSettingsManager;

/**
 * Commonly shared data.
 */
class LogsCommonData 
{ 
    private: 
        explicit LogsCommonData();
        virtual ~LogsCommonData();
        
    public: 
        static LogsCommonData& getInstance();
        static void freeCommonData();
        
        QContactManager& contactManager();
        
        /**
         * Configure maximum number of events. It is possible to
         * configure that reading is stopped once defined amount
         * of events of certain direction has been read.
         */
        void configureReadSize(
                int maxSize, 
                LogsEvent::LogsDirection dir = LogsEvent::DirUndefined );
        /**
         * Get max read size
         * @return max number of events, -1 if max num is not configured 
         */
        int maxReadSize() const;
        LogsEvent::LogsDirection maxReadSizeDirection() const;
        
        int updateConfiguration(const LogsConfigurationParams& params);
        LogsConfigurationParams& currentConfiguration();
        
        int telNumMatchLen() const;
        
        /**
         * Returns cenrep key status of predictive search feature. 
         * @return 0 - feature is permanently off and can't be turned on,
         *         1 - feature is on
         *         2 - feature is temporarily off and can be turned on 
         *         negative value indicates some error in fetching the key
         */
        int predictiveSearchStatus();
        
        /**
         * Allows to modify cenrep key value of predictive search features. 
         * However, this function can't be used if feature is set permanently off 
         * (see predictiveSearchStatus())
         * @param enabled, specify whether cenrep key will be set to 1 or 2
         * @ return 0 if cenrep key value modified succesfully,
         *          -1 in case of some error
         */
        int setPredictiveSearch(bool enabled);
        
        /**
         * Clear missed calls counter.
         * @return 0 if clearing was success
         */
        int clearMissedCallsCounter();
        
        bool getTelNumMatchLen(int& matchLen);
        
        
    private:
        
        int getPredictiveSearchStatus();
        
    private:
        
        QContactManager* mContactManager;
        int mMaxReadSize;
        LogsEvent::LogsDirection mMaxReadSizeDir;
        LogsConfigurationParams mConfiguration;
        XQSettingsManager* mSettingsManager;
        int mMatchLen;
        int mPredictiveSearchStatus;
        
    private:
        friend class UT_LogsCommonData;
        friend class UT_LogsModel;
        
};


#endif // LOGSCOMMONDATA_H
