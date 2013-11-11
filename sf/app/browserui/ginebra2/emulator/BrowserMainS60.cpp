/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <coemain.h>
#include "BrowserMainS60.h"

/*
 * Handles calls from other apps to QDesktopServices.openUrl
 */

// This function handles when an app calls QDesktopServices.openUrl when the browser is already started
MCoeMessageObserver::TMessageResponse BrowserMainAppUiS60::HandleMessageL(TUint32 /*aClientHandleOfTargetWindowGroup*/, TUid /*aMessageUid*/, const TDesC8 &aMessageParameters)
{
    QString url = QString::fromUtf8((const char *)aMessageParameters.Ptr(), aMessageParameters.Length());
    //qDebug() << "BrowserMainAppUiS60::HandleMessageL " <<
    //        QString::fromUtf16(aMessageUid.Name().Ptr(), aMessageUid.Name().Length()) << ", " << url;
    // I have no idea if the browser is thread-safe and what will happen when it receives this signal when it's in the middle of something else - mm
    GinebraBrowser *urlHandler = ((BrowserMainApplicationS60 *)Application())->UrlHandler();
    if (urlHandler != 0) {
        urlHandler->queueOpenUrl(url);
    }
    return(EMessageHandled);
}

//    void HandleApplicationSpecificEventL(TInt aType, const TWsEvent &aEvent)
//    {
//        qDebug() << "BrowserMainAppUiS60::HandleApplicationSpecificEvent " << aType << ", " << aEvent.EventData();
//    }

//    void OpenFileL(const TDesC& aFilename)
//    {
//        qDebug() << "BrowserMainAppUiS60::OpenFileL fn - " << QString::fromUtf16(aFilename.Ptr(), aFilename.Length());
//    }

//    void HandleCommandL(TInt aCommand)
//    {
//        qDebug() << "BrowserMainAppUiS60::HandleCommandL - " << aCommand;
//    }

// These functions handle when an app calls QDesktopServices.openUrl when the browser isn't already started
TBool BrowserMainAppUiS60::ProcessCommandParametersL(TApaCommand /*aCommand*/,TFileName& aFilename)
{
    //qDebug() << "BrowserMainAppUiS60:: ProcessCmdParms2 - " << aCommand << ", " << QString::fromUtf16(aFilename.Ptr(), aFilename.Length());
//        ((RealBrowserApp *)QApplication::instance())->setInitialUrl(QString::fromUtf16(aFilename.Ptr(), aFilename.Length()));
    ((BrowserMainApplicationS60 *)Application())->SetInitialUrl(QString::fromUtf16(aFilename.Ptr(), aFilename.Length()));
    return ETrue;
}

TBool BrowserMainAppUiS60::ProcessCommandParametersL(TApaCommand /*aCommand*/,TFileName& aFilename, const TDesC8& /*aTail*/)
{
    //qDebug()
    //       << "BrowserMainAppUiS60:: ProcessCmdParms3 - " << aCommand << ", "
    //       << QString::fromUtf16(aFilename.Ptr(), aFilename.Length()) << ", "
    //       << QString::fromUtf8((const char*)aTail.Ptr(), aTail.Length());
    ((BrowserMainApplicationS60 *)Application())->SetInitialUrl(QString::fromUtf16(aFilename.Ptr(), aFilename.Length()));
//        ((RealBrowserApp *)QApplication::instance())->setInitialUrl(QString::fromUtf16(aFilename.Ptr(), aFilename.Length()));
    return ETrue;
}

CEikAppUi *BrowserMainDocumentS60::CreateAppUiL()
{
//    qDebug() << "BrowserMainDocumentS60::CreateAppUiL";
    return new (ELeave) BrowserMainAppUiS60;
}
//    CFileStore* OpenFileL(TBool aDoOpen, const TDesC &aFilename, RFs &aFs)
//    {
//        qDebug() << "BrowserMainDocumentS60::OpenFileL fn - " << QString::fromUtf16(aFilename.Ptr(), aFilename.Length());
//        return 0;
//    }
//    void OpenFileL(CFileStore*& aFileStore, RFile& aFile)
//    {
//        aFileStore = NULL; //So the other OpenFileL version is not called
//        TFileName name;
//        aFile.FullName(name);
//        qDebug() << "BrowserMainDocumentS60::OpenFileL fs - " << QString::fromUtf16(name.Ptr(), name.Length());
//    }

CApaDocument *BrowserMainApplicationS60::CreateDocumentL()
{
//    qDebug() << "BrowserMainApplicationS60::CreateDocumentL";
    return new (ELeave) BrowserMainDocumentS60(*this);
}

BrowserMainApplicationS60::BrowserMainApplicationS60() : QS60MainApplication(), urlHandler(0)
{
//        CApaCommandLine* commandLine = 0;
//        TInt err = CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine);
//        qDebug() << "BrowserMainApplicationS60 - " << QString::fromUtf16(commandLine->DocumentName().Ptr(), commandLine->DocumentName().Length());
//        qDebug() << "BrowserMainApplicationS60";
//    initialUrl = 0;
}

void BrowserMainApplicationS60::setUrlHandler(GinebraBrowser *uh)
{
    urlHandler = uh;
}
