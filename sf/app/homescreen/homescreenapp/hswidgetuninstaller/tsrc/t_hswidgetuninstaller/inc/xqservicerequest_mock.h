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
* Description:  Mock class for XQServiceRequest
*
*/

#ifndef XQSERVICEREQUEST_H
#define XQSERVICEREQUEST_H
#include <QObject>
#include <QVariant>
#include "xqrequestinfo_mock.h"

class XQServiceRequest : public QObject
{
    Q_OBJECT

public:
    
    XQServiceRequest(const QString& /*service*/, const QString& /*message*/, const bool& synchronous = true){
        Q_UNUSED(synchronous);};
    ~XQServiceRequest(){};

    bool send(){return true;};
    
    void setInfo(const XQRequestInfo &/*info*/){};
    XQRequestInfo info() const{return XQRequestInfo();};
    int latestError()  {return 0;};
    
    template<typename T>
    inline XQServiceRequest &operator<< (const T &/*var*/)
    {
        return *this;
    }

private:
};

#endif
