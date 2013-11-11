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
#ifndef LOGSCONFIGURATIONPARAMS_H
#define LOGSCONFIGURATIONPARAMS_H

#include <QObject>
#include <logsexport.h>

/**
 *
 */
class LogsConfigurationParams : public QObject
{
    Q_OBJECT

public: // The exported API

    LOGSENGINE_EXPORT explicit LogsConfigurationParams(QObject* parent = 0);
    LOGSENGINE_EXPORT ~LogsConfigurationParams();
    
    LOGSENGINE_EXPORT void setListItemTextWidth(int width);
    LOGSENGINE_EXPORT int listItemTextWidth() const;  
    
    LOGSENGINE_EXPORT void setLocaleChanged(bool changed);
    LOGSENGINE_EXPORT bool localeChanged() const;  

    LOGSENGINE_EXPORT LogsConfigurationParams& operator=(const LogsConfigurationParams& params);

private:
    int mListItemTextWidth;
    bool mLocaleChanged;

};
                  
#endif // LOGSCONFIGURATIONPARAMS_H
