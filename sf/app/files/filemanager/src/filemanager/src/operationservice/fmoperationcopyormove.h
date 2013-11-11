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

#ifndef FMOPERATIONCOPYORMOVE_H
#define FMOPERATIONCOPYORMOVE_H

#include <QObject>
#include <QString>

#include "fmoperationbase.h"

class FmOperationCopyOrMove : public FmOperationBase
{
    Q_OBJECT
    
public:
    explicit FmOperationCopyOrMove( FmOperationService *operationService, FmOperationService::TOperationType type, const QStringList &sourceList, const QString &targetPath );
    virtual ~FmOperationCopyOrMove();
    virtual int prepare();
    virtual void start( volatile bool *isStopped );
    QStringList sourceList();
    QString     targetPath();

signals:
    void askForReplace( const QString &srcFile, const QString &destFile, bool *isAccepted );
    void askForRename( const QString &srcFile, QString *destFile );
    void driveSpaceChanged();
        
private:
    int copyOrMove( const QString &source, const QString &targetPath, const QString &newTargetName = QString() );
    int copyOrMoveDirInsideContent( const QString &srcPath, const QString &destPath );
    void increaseProgress( quint64 size );
    void queryForRename( const QString &srcFile, QString *destFile );
    int copyOneFile( const QString &srcFile, const QString &desFile );
    bool targetHasEnoughSpace();

private:
    FmOperationService *mOperationService;
    QStringList         mSourceList;
    QString             mTargetPath;    
    volatile bool      *mStop;    
    QString             mErrString;
    
    quint64             mTotalSize;
    quint64             mCopiedOrMovedSize;
    int                 mTotalSteps;
    int                 mCurrentStep;
};
#endif
