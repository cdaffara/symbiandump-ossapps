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


#ifndef NMVIEWERVIEWNETMANAGER_H_
#define NMVIEWERVIEWNETMANAGER_H_

#include <QNetworkAccessManager>

class QNetworkRequest;
class NmViewerView;
class NmUiEngine;

class NmViewerViewNetManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    NmViewerViewNetManager(NmUiEngine &uiEngine);
    ~NmViewerViewNetManager();
    void setView(NmViewerView *viewerView);
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                 QIODevice *outgoingData);

private:
	NmUiEngine &mUiEngine;
	NmViewerView *mMessageView;    // Not owned

};

#endif /* NMVIEWERVIEWNETMANAGER_H_ */
