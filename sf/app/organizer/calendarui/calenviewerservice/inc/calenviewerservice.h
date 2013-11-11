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

#ifndef __CALEN_VIEWER_SERVICE_H__
#define __CALEN_VIEWER_SERVICE_H__

// System includes
#include <xqserviceprovider.h>
#include <xqsharablefile.h>

// Forward declarations
class AgendaUtil;
class AgendaEventViewer;

class CalenViewerService : public XQServiceProvider
{
    Q_OBJECT
public:
	CalenViewerService(QObject *parent = 0);
	~CalenViewerService();

public slots:
	bool view(QString file);
	bool view(XQSharableFile file);
	
private slots:
    void entryViewReady(int error);
    void handleClientDisconnected();
    
private:
    void viewEntry();
    void complete(bool ok);
    
private:
    QString mFileName;
    AgendaUtil *mAgendaUtil;
    AgendaEventViewer *mViewer;
    bool mEntryViewReady;
    bool mWaitingForEntryViewReady;
    int mAsyncReqIndex;
};

#endif		// __CALEN_VIEWER_SERVICE_H__
