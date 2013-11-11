/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <QMap>
#include <xqserviceprovider.h>


class FileService : public XQServiceProvider
{
    Q_OBJECT
    public:
        FileService(QObject *parent = 0);
        ~FileService();
        bool asyncAnswer() {return mAsyncReqIds.count() > 0;}
    
    public slots:
        bool view(QString file);
    
    private slots:
        void handleClientDisconnect();
        void handleAnswerDelivered();
        void completeAsyncRequest(bool ok);
    
    private:
        void RemoveAsyncReqId();
        
        QMap<quint32,int> mAsyncReqIds;
};


#endif
