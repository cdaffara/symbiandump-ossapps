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

#ifndef SCHEME_HANDLER_QTHIGHWAY_P_H
#define SCHEME_HANDLER_QTHIGHWAY_P_H

// INCLUDES

#include <QtCore/QObject>
#include <xqappmgr.h>
#include "SchemeHandlerBr_p.h"

// CLASS DECLARATION

class QUrl;


namespace WRT {

/**
* Scheme Handler implementation for platform that are using QtHighway.
*/
class QtHighwaySchemeHandlerPrivate : public SchemeHandlerPrivate {

Q_OBJECT

public:
    QtHighwaySchemeHandlerPrivate();
    ~QtHighwaySchemeHandlerPrivate();
    
    virtual SchemeHandler::SchemeHandlerError HandleScheme(const QUrl &url);

private:
    SchemeHandler::SchemeHandlerError SendStandardUriViewRequest(const QUrl& url);

    // application interworking manager from QtHighway
    XQApplicationManager mAiwMgr;
};

} // WRT
#endif /* def SCHEME_HANDLER_QTHIGHWAY_P_H */
