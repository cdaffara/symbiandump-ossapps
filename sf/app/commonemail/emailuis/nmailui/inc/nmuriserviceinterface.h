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
* Description: NMail Application service interface used for interfacing between
*              QT highway and other applications
*
*/

#ifndef NMURISERVICEINTERFACE_H_
#define NMURISERVICEINTERFACE_H_

//  INCLUDES
#include <QObject>
#include <QVariant>
#include <xqserviceprovider.h>

//  FORWARD DECLARATIONS
class NmUiEngine;
class NmApplication;
class NmUiStartParam;
class NmMailboxSelectionDialog;
class NmId;
class HbView;

class NmUriServiceInterface : public XQServiceProvider
{
    Q_OBJECT
public:
    NmUriServiceInterface(QObject *parent,
                          NmUiEngine &uiEngine,
                          NmApplication *application);
    virtual ~NmUriServiceInterface();

public slots:
    bool view(const QString& uri);
    void selectionDialogClosed(NmId &mailboxId);

private:
    void launchEditorView(NmId mailboxId);
    void cancelService();

private: // Data
    NmApplication *mApplication;                // Not owned
    NmUiEngine &mUiEngine;
    int mAsyncReqId;
    NmUiStartParam *mStartParam;                // Owned
    NmMailboxSelectionDialog *mSelectionDialog; //Owned
    HbView *mCurrentView;
};

#endif /* NMURISERVICEINTERFACE_H_ */

// End of file.
