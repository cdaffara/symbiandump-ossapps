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
* Description:  hbdocumentloader stub
*
*/

#ifndef HBDOCUMENTLOADER_H
#define HBDOCUMENTLOADER_H

#include "hbwidget.h"
#include <qlist.h>
#include <qpointer.h>

class HbMainWindow;
class VideoCollectionUiLoader;

class ObjectData : public QObject
{
    Q_OBJECT
public:
    ObjectData(QObject *obj, QString name)
    {
        mObject = obj;
        mName = name;
    }
    ~ObjectData()
    {
        delete mObject;
    }
public:
    QPointer<QObject> mObject;
    QString mName;
};

class HbDocumentLoader
{

public:
    static void cleanup();
    
    HbDocumentLoader();
    HbDocumentLoader(const HbMainWindow *window);
    virtual ~HbDocumentLoader();

    QObjectList load( const QString &fileName, const QString &section , bool *ok = 0 );
    QObjectList load( const QString &fileName, bool *ok = 0 );
        
    QGraphicsWidget *findWidget(const QString &name);
    QObject *findObject(const QString &name);

    void reset();
    
protected:
    virtual QObject *createObject(const QString& type, const QString &name);
    
private:
    
    QObject *doCreateObject(const QString &name);
    
    QList<ObjectData *> mObjects;
    
    bool mCreatingObject;
    
public:
    static bool mFindWidgetFails;
    static bool mFindObjectFails;
    static bool mCreateObjectFails;

    static bool mVideoListWidgetFailure;
    static bool mCollectionWidgetFailure;
    static bool mCollectionContentWidgetFailure;
    static bool mLoadFails;
    
    static QObjectList mLoadReturns;
};

#endif // HBDOCUMENTLOADER_H
