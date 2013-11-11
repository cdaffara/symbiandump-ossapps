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
 * 
 * Description:
 *     The header file of the back up config loader of file manager
 */

#ifndef FMBACKUPCONFIGLOADER_H
#define FMBACKUPCONFIGLOADER_H

#include <QXmlStreamReader>
#include <QString>

#include "fmcommon.h"

class FmBkupDrivesAndOperation;
class FmBkupBackupCategory;
class FmBackupSettings;

class FmBackupConfigLoader : public QObject
{
Q_OBJECT

public:
    FmBackupConfigLoader(void);
    ~FmBackupConfigLoader(void);

public:
    void loadData();
    QList<FmBkupDrivesAndOperation* > driversAndOperationList();
    QList<FmBkupBackupCategory*>      backupCategoryList();

private:
    void parseXml();
    void parseDrivesAndOperation();
    void parseBackupCategory();

private:
    
    QXmlStreamReader xml;

    QString currentTag;

    QString driveString;
    QString typeString;

    QString categoryString;
    QString archive_nameString;
    QString special_flagsString;
    QString exclude_special_flagsString;
    QString uidsString;
    QString exclude_uidsString;

    QList<FmBkupDrivesAndOperation* > mDrivesAndOperationList;
    QList<FmBkupBackupCategory*>      mBackupCategoryList;
};

#endif


