/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMVIEWERWEBVIEW_H_
#define NMVIEWERWEBVIEW_H_

#include <QGraphicsWebView>

#include "nmcommon.h"

class NmViewerView;
class NmMessage;
class QGestureEvent;

class NmMailViewerWkContentItem
{
public:
    NmMailViewerWkContentItem() 
        : mData(0), mPartId(0), mIsFetched(false) {}
    NmMailViewerWkContentItem(QVariant data, NmId partId, bool isFetched) 
        : mData(data), mPartId(partId), mIsFetched(isFetched) {}
    QVariant mData;
    NmId     mPartId;
    bool     mIsFetched;
};

class NmMailViewerWK : public QGraphicsWebView
{
    Q_OBJECT
public:
    NmMailViewerWK();
    ~NmMailViewerWK();
    virtual QVariant loadResource (int type, const QUrl &name, NmId &partId, bool &isFetched);
    void addContent(QString key, QVariant val, NmId partId, bool isFetched);
    bool event(QEvent* event);
    
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    void gestureEvent(QGestureEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    
private:
    QMap<QString,NmMailViewerWkContentItem> mContent;
    bool mSuppressRelease;
};

#endif /* NMVIEWERWEBVIEW_H_*/
