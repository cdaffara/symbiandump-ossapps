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

#ifndef FEEDVIEW_H
#define FEEDVIEW_H

#include <QObject>
#include <hbview.h>
#include <qtcontacts.h>
#include <hbdocumentloader.h>

class PrcPresenceWriter;
class PrcPresenceReader;
class HbView;
class QTimer;

QTM_USE_NAMESPACE

class FeedView : public QObject
{
    Q_OBJECT
public:
    FeedView(QObject *parent = 0);
    ~FeedView();

    HbView* view() const { return mView; }

private slots:
    void startPressed();
    void stopPressed();
    void createPressed();

    void updateRandomPresence();
	
private: //data
    HbView*          mView;
    PrcPresenceWriter*  iWriter;
    PrcPresenceReader*  iReader;
    QContactManager* mManager;
    QTimer*          mUpdateTimer;
    HbDocumentLoader mDocumentLoader;

    QStringList      mBuddyList;

    bool             mStarted;
};

#endif /* FEEDVIEW_H */
