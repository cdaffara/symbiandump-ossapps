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

#ifndef SCHEME_HANDLER_P_H
#define SCHEME_HANDLER_P_H

// INCLUDES

#include <QtCore/QObject>
#include "SchemeHandlerBr.h"

// CLASS DECLARATION

class QUrl;


namespace WRT {

// Use default scheme handler if platform isn't using QtHighway for application
// interworking.
#ifndef QTHIGHWAY
#define SCHEME_HNDLR_PRIVATE DefSchemeHandlerPrivate
#else
#define SCHEME_HNDLR_PRIVATE QtHighwaySchemeHandlerPrivate
#endif // QTHIGHWAY

/**
* Scheme Handler implementation abstract base class.
* The implementation of this class depends on whether the platform uses 
* QtHighway for application interworking.
*/
class SchemeHandlerPrivate : public QObject {

Q_OBJECT

public:
    SchemeHandlerPrivate();
    virtual ~SchemeHandlerPrivate();

    bool ReadSdConfirmDtmfValue();
    virtual SchemeHandler::SchemeHandlerError HandleUrl(const QUrl &url, bool confirmDTMF);
    
    virtual SchemeHandler::SchemeHandlerError HandleScheme(const QUrl &url) = 0;
};

} // WRT
#endif /* def SCHEME_HANDLER_P_H */
