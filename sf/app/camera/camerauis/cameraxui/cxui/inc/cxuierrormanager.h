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
#ifndef CXUIERRORMANAGER_H
#define CXUIERRORMANAGER_H

#include <QObject>
#include "cxeerror.h"

class HbAction;
class HbDialog;
class CxuiDocumentLoader;


/*
* class which handles errors and reports
*/
class CxuiErrorManager : public QObject
{
    Q_OBJECT

public:

    CxuiErrorManager(CxuiDocumentLoader *documentLoader);
    ~CxuiErrorManager();

signals:

    void errorPopupShown();
    void errorPopupClosed();

public slots:

    void check(CxeError::Id error);
    void clear();

private slots:
    void popupClosed(HbAction *action);
    void closeApp();

private:

    // error severity
    enum ErrorSeverity
    {
        None = 0,
        Warning,
        Error
    };

    void launchPopup(const QString &errorText, const QString &buttonText);
    void showErrorPopup(const QString &errorText, const QString &buttonText);
    void showWarningPopup(const QString &errorText);
    void getErrorDetails(QString &errorText, QString &buttonText);

private:

    //data
    CxuiDocumentLoader *mDocumentLoader; // not own
    HbDialog* mErrorMsgPopup;
    CxeError::Id mErrorId;
    CxuiErrorManager::ErrorSeverity mErrorSeverity;
};

#endif // CXUIVIEWMANAGER_H
