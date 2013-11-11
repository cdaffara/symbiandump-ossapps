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

#ifndef BROWSERMAINS60_H_
#define BROWSERMAINS60_H_

#include <QtGui/QS60MainApplication>
#include <QtGui/QS60MainDocument>
#include <QtGui/QS60MainAppUi>
#include <QDebug>
#include <QtGui>

#include "browser.h"
#include "singleton.h"

/*
 * Handles calls from other apps to QDesktopServices.openUrl
 */

class BrowserMainAppUiS60 : public QS60MainAppUi
{
  public:
    BrowserMainAppUiS60() : QS60MainAppUi() {};

    // This function handles when an app calls QDesktopServices.openUrl when the browser is already started
    MCoeMessageObserver::TMessageResponse HandleMessageL(TUint32 aClientHandleOfTargetWindowGroup, TUid aMessageUid, const TDesC8 &aMessageParameters);

    // These functions handle when an app calls QDesktopServices.openUrl when the browser isn't already started
    TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aFilename);
    TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aFilename, const TDesC8& aTail);
};

class BrowserMainDocumentS60 : public QS60MainDocument
{
  public:
    BrowserMainDocumentS60(CEikApplication &eikApp) : QS60MainDocument(eikApp) {};
    CEikAppUi *CreateAppUiL();
};

class BrowserMainApplicationS60 : public  QS60MainApplication
{
  
  public:
    BrowserMainApplicationS60();
    void SetInitialUrl(QString url) { initialUrl = url; }
    QString InitialUrl() { return initialUrl; }
    void setUrlHandler(GinebraBrowser *uh);
    GinebraBrowser *UrlHandler() { return urlHandler; }

  protected:
    CApaDocument *CreateDocumentL();

  private:
    QString initialUrl;
    GinebraBrowser *urlHandler;
};

// Define the Meyer's singleton for BrowserMainApplicationS60
typedef Singleton<BrowserMainApplicationS60, CreateGamma> CBrowserMainAppSingleton;

#endif /* BROWSERMAINS60_H_ */
