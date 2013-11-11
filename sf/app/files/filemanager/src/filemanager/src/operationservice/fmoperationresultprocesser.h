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

#ifndef FMOPERATIONRESULTPROCESSER_H
#define FMOPERATIONRESULTPROCESSER_H
#include <QObject>

class FmOperationBase;
class FmOperationService;
class HbProgressDialog;

class FmOperationResultProcesser :
    public QObject
{
Q_OBJECT
public:
    explicit FmOperationResultProcesser( FmOperationService* );
    ~FmOperationResultProcesser(void);

    void onAskForRename( FmOperationBase* operationBase, const QString &srcFile, QString *destFile );
    void onAskForReplace( FmOperationBase* operationBase, const QString &srcFile, const QString &destFile, bool *isAccepted );
    void onShowNote( FmOperationBase* operationBase, const char *noteString );
    void onNotifyWaiting( FmOperationBase* operationBase, bool cancelable );
    void onNotifyPreparing( FmOperationBase* operationBase, bool cancelable );
    void onNotifyStart( FmOperationBase* operationBase, bool cancelable, int maxSteps );
    void onNotifyProgress( FmOperationBase* operationBase, int currentStep );


    void onNotifyFinish( FmOperationBase* operationBase ); 
    void onNotifyError( FmOperationBase* operationBase, int error, const QString &errString );
    
private slots:
    void onProgressCancelled();
private:
    void showWaiting(  QString title, bool cancelable );
    void showPreparing( QString title, bool cancelable );
    void showProgress(  QString title, bool cancelable, int maxValue );
    void setProgress( int value );
    void finishProgress();
    void cancelProgress();
    void failAndCloseProgress();
private:
    FmOperationService *mOperationService;
    HbProgressDialog *mNote;
};

#endif
