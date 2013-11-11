/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <xqconversions.h>
#include <hbinputdialog.h>
#include <bautils.h>
#include <shareuidialog.h>
#include <eikenv.h>
#include <hbprogressdialog.h>
#include <hbaction.h>
#include <QDebug>

#include "launchermainwindow.h"
#include "notifications.h"
#include "enginewrapper.h"

// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper(LauncherMainWindow* mainWindow)
: mEngine(0), 
  mMainWindow(mainWindow),
  mProgressDialog(0),
  mWaitDialog(0)
{
}

// ---------------------------------------------------------------------------

EngineWrapper::~EngineWrapper()
{
    if (mEngine != NULL) {
        delete mEngine;
        mEngine = NULL;
    }
    if (mProgressDialog != NULL) {
        delete mProgressDialog;
        mProgressDialog = NULL;
    }
    if (mWaitDialog != NULL) {
        delete mWaitDialog;
        mWaitDialog = NULL;
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::init()
{
    
    // construct engine
    TRAPD(error, mEngine = CLauncherEngine::NewL(this));
    if (error != KErrNone) {
        return false;
    }
    
    return true;
   
}

// ---------------------------------------------------------------------------

bool EngineWrapper::listOfAllApps(QStringList &allAppsQStringList){
    
    // Get application CDesCArray list from engine
    CDesCArray* allAppsCDescArray = 0;
    TRAPD(error, allAppsCDescArray = mEngine->ListOfAllAppsL());
    if(error != KErrNone) {
        return false;
    }
        
    //convert CDesCArray to QStringList
    for (int i = 0; i < allAppsCDescArray->Count(); i++) {
        allAppsQStringList.append(QString::fromUtf16(
                allAppsCDescArray->MdcaPoint(i).Ptr(),
                allAppsCDescArray->MdcaPoint(i).Length()));
    }
    return true;
}

// ---------------------------------------------------------------------------
bool EngineWrapper::startAppLaunching(const QModelIndexList &qList, bool autoClose) 
{
    
    // check if used has not selected any files
    if (qList.size() == 0) {
        Notifications::showErrorNote("Nothing selected");
        return false;
    }

    bool retval(true);
    CArrayFix<TInt>* pArrayFix = NULL;
    try{
        // Create CArrayFix for application numbers that are launched and Qlist into CArrayFix
        QT_TRAP_THROWING( pArrayFix = QModelIndexListToCArrayFixL(qList) );
        
        // Start launching applications
        QT_TRAP_THROWING(mEngine->StartAppLaunchingL(pArrayFix, autoClose ? ETrue : EFalse));
    }
    catch (const std::exception &e){
        qDebug() << "EngineWrapper::startAppLaunching - exception: " << e.what();
        retval = false;
    }
    
    if(pArrayFix)
        delete pArrayFix;
    
    return retval;
}

// ---------------------------------------------------------------------------

bool EngineWrapper::stopLaunching()
{
    TRAPD(error, mEngine->StopLaunchingL());
    if (error != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::sendLog()
{
    ShareUi dialog;
    QList<QVariant> fileList;

    if( mEngine->LogFileExists() )
        {
        const TDesC& logFilePath = mEngine->LogFilePath();
        fileList.append( QString::fromUtf16( logFilePath.Ptr(), logFilePath.Length() ) );
        }
    if( mEngine->BCLogFileExists() )
        {
        const TDesC& BCLogFilePath = mEngine->BCLogFilePath();
        fileList.append( QString::fromUtf16( BCLogFilePath.Ptr(), BCLogFilePath.Length() ) );
        }
    
    if( !fileList.empty()){
        dialog.init(fileList,true);   
        return true;
    }
    else{
        return false;
    }
    
}

// ---------------------------------------------------------------------------

bool EngineWrapper::deleteLog()
{
    // if log file exists delete it.
    if (mEngine->LogFileExists()) {
        if (mEngine->DeleteLogFile() != KErrNone) {
            return false;
        }
    }
    
    //if BC log file exists, delete it.
    if (mEngine->BCLogFileExists()) {
        if (mEngine->DeleteBCLogFile() != KErrNone) {
            return false;
        }
    }
    
    return true;

}

// ---------------------------------------------------------------------------

bool EngineWrapper::sendListOfDlls()
{
    const TDesC& systemDllsFilePath = mEngine->SystemDllsFilePath();
    
    if ( BaflUtils::FileExists( CEikonEnv::Static()->FsSession(), systemDllsFilePath) ){
        ShareUi dialog;
        QList<QVariant> fileList;
        fileList.append( QString::fromUtf16( systemDllsFilePath.Ptr(), systemDllsFilePath.Length() ) );
        dialog.init(fileList,true); 
        return true;
    }
    else{
        Notifications::showErrorNote("DLL list does not exist");
        return false;
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::doCompareDlls(HbAction* action)
    {
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if( action && !action->text().compare("ok", Qt::CaseInsensitive) )
        {
        TFileName fileName( dlg->value().toString().utf16() );
        TRAPD(error,  mEngine->AnalyseDLLsL( fileName ) );
        if( error != KErrNone )
            { 
            HideWaitDialog();
            HideProgressBar();
            Notifications::showErrorNote( QString("Error in analyse: %1").arg( error ) );
            }
        }
    }

// ---------------------------------------------------------------------------

void EngineWrapper::compareDlls()
{
    const TDesC& requiredDllsFileName = mEngine->RequiredDllsFilePath();
    QString fileName = QString::fromUtf16( requiredDllsFileName.Ptr(), requiredDllsFileName.Length() );
    HbInputDialog::getText( "Select input file for BC analysis:", this, SLOT(doCompareDlls(HbAction*)), fileName );
}

// ---------------------------------------------------------------------------
void EngineWrapper::ShowProgressBar(const TDesC& descText, TInt minimum, TInt maximum)
{
    QString qStringText = XQConversions::s60DescToQString(descText);
   
    if(mProgressDialog){
        delete mProgressDialog;
        mProgressDialog = NULL;
    }
    mProgressDialog = new HbProgressDialog( HbProgressDialog::ProgressDialog );
    
    if( maximum > 0 )
        mProgressDialog->setRange(minimum, maximum);
    //iProgressDialog->setAutoClose(true);
    mProgressDialog->setText(qStringText);
    connect(mProgressDialog, SIGNAL(cancelled()), this, SLOT(CancelDllBCanalysis()));
    mProgressDialog->show();
}

// ---------------------------------------------------------------------------
void EngineWrapper::SetProgressBarValue(TInt value)
{
    if (mProgressDialog && value > 0 ){
        mProgressDialog->setProgressValue(value);
    }
}

// ---------------------------------------------------------------------------
void EngineWrapper::HideProgressBar()
{
    if (mProgressDialog != NULL) {
        disconnect(mProgressDialog, SIGNAL(cancelled), this, SLOT(CancelDllBCanalysis()));
        delete mProgressDialog;
        mProgressDialog = NULL;
    }
}

// ---------------------------------------------------------------------------
void EngineWrapper::CancelDllBCanalysis()
{
    mEngine->CancelBCAnalysis();
}

// ---------------------------------------------------------------------------
void EngineWrapper::ShowWaitDialog(const TDesC& descText)
{
    QString qStringText = XQConversions::s60DescToQString(descText);
   
    if(mWaitDialog){
        delete mWaitDialog;
        mWaitDialog = NULL;
    }
    mWaitDialog = Notifications::showWaitDialog(qStringText);
}

// ---------------------------------------------------------------------------
void EngineWrapper::HideWaitDialog()
{
    if (mWaitDialog != NULL) {
        delete mWaitDialog;
        mWaitDialog = NULL;
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::setSkipHiddenAndEmbedOnly(bool skip)
{
    if(skip) {
        mEngine->SetSkipHiddenAndEmbedOnly(ETrue);
    }
    else {
        mEngine->SetSkipHiddenAndEmbedOnly(EFalse);
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::ChangeFocusToOutputView()
{
    mMainWindow->openOutputView();
}

// ---------------------------------------------------------------------------

void EngineWrapper::PrintText(const TDesC& descText)
{
    
    // Convert the descriptor to QString
    QString qStringText = XQConversions::s60DescToQString(descText);
    mMainWindow->printText(qStringText);
}

// ---------------------------------------------------------------------------

CArrayFix<TInt>* EngineWrapper::QModelIndexListToCArrayFixL(const QModelIndexList& qlist)
{
    CArrayFix<TInt>* pArrayFix = new (ELeave) CArrayFixFlat<TInt>( qlist.size() );
    CleanupStack::PushL(pArrayFix);
    for (int j = 0; j < qlist.size(); j++) {
        pArrayFix->AppendL(qlist.at(j).row());
    }
    CleanupStack::Pop(pArrayFix);
    return pArrayFix;
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowErrorMessage(const TDesC& descText)
{
    QString qStringText = XQConversions::s60DescToQString(descText);
    Notifications::showErrorNote(qStringText);
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowInfoMessage(const TDesC& descText)
{
    QString qStringText = XQConversions::s60DescToQString(descText);
    Notifications::showInformationNote(qStringText);
}

// End of File
