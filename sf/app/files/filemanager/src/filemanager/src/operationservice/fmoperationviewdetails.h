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
*     The header file of the operation param of file manager
*
*/

#ifndef FMOPERATIONVIEWDETAILS_H
#define FMOPERATIONVIEWDETAILS_H

#include "fmoperationservice.h"
#include "fmoperationbase.h"

#include <QObject>
#include <QString>

class FmOperationDriveDetails : public FmOperationBase
{
public:
    explicit FmOperationDriveDetails( QObject *parent, const QString &driverName );
    
    virtual ~FmOperationDriveDetails();
    QString driverName();
    virtual void start( volatile bool *isStopped );
    QList<FmDriveDetailsSize*> &detailsSizeList();
    
private:
    QString mDriverName;
    QList<FmDriveDetailsSize*> mDetailsSizeList;
    volatile bool *mStop;
};

class FmOperationFolderDetails : public FmOperationBase
{
public:
    explicit FmOperationFolderDetails( QObject *parent, const QString &folderPath );
    
    virtual ~FmOperationFolderDetails();
    virtual void start( volatile bool *isStopped );
    
    QString folderPath();
    int &numofSubFolders();
    int &numofFiles();
    quint64 &sizeofFolder();
    
private:
    QString mFolderPath;
    int mNumofSubFolders;
    int mNumofFiles;
    quint64 mSizeofFolder;
    volatile bool *mStop;
};
#endif
