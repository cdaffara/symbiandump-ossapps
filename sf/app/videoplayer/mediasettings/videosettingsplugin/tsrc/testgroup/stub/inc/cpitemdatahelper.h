/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CpItemDataHelper stub
*
*/

#ifndef CP_ITEM_DATA_HELPER_H
#define CP_ITEM_DATA_HELPER_H

#include <QObject>
#include <qmap.h>

class HbWidget;
class QModelIndex;
class HbDataFormModelItem;

struct ConnectionHolder
{
    ConnectionHolder() {}
    
    ConnectionHolder(const char* signal, QObject* receiver, const char* method)
    {
        this->signal = signal;
        this->receiver = receiver;
        this->method = method;
    }
    
    const char* signal;
    QObject* receiver;
    const char* method;
};

class CpItemDataHelper : public QObject
{
    Q_OBJECT
public:
    CpItemDataHelper();
    virtual ~CpItemDataHelper();

    void addConnection(HbDataFormModelItem *item,
        const char *signal,
        QObject *receiver,
        const char *method);

    void connectToForm(const char *signal,
        QObject *receiver,
        const char *method);

    HbWidget *widgetFromModelIndex(const QModelIndex &index);
    
    HbDataFormModelItem *modelItemFromModelIndex(const QModelIndex &index);

public:
    
    HbWidget* mWidgetReturnValue;
    
    HbDataFormModelItem* mModelItemReturnValue;
    
    ConnectionHolder mFormConnection;
    
    QMap<HbDataFormModelItem*, ConnectionHolder> mConnections;
};

#endif
