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
* Description: NMail application service interface used for viewing a mail
*              according to given id. The service utilizes the Qt highway framework.
*
*/

#ifndef NMVIEWERSERVICEINTERFACE_H_
#define NMVIEWERSERVICEINTERFACE_H_

//  INCLUDES
#include <QObject>
#include <xqserviceprovider.h>

//  FORWARD DECLARATIONS
class NmApplication;
class NmUiEngine;
class NmId;


class NmViewerServiceInterface : public XQServiceProvider
{
    Q_OBJECT

public:

    NmViewerServiceInterface(QObject *parent,
                             NmApplication *application,
                             NmUiEngine &uiEngine);
    
    ~NmViewerServiceInterface();


public slots:

    void viewMessage(QVariant idList, QVariant flags);


private:

    NmApplication *mApplication; // Not owned
    NmUiEngine &mUiEngine;
    int mAsyncReqId;
};


#endif /* NMVIEWERSERVICEINTERFACE_H_ */
