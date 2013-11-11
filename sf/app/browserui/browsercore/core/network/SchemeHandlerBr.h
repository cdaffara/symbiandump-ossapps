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

#ifndef SCHEME_HANDLER_H
#define SCHEME_HANDLER_H

// INCLUDES

#include <QtCore/QObject>

// CLASS DECLARATION

class QUrl;

namespace WRT {

class SchemeHandlerPrivate;

/**
* Scheme Handler IF definition class
*/
class SchemeHandler : public QObject {

Q_OBJECT

public:
    enum SchemeHandlerError {
        NoError,                // Success
        SchemeNotHandled,       // http(s) and file schemes to be handled elsewhere
        SchemeUnsupported,      // scheme not supported
        LaunchFailed            // attempt to launch handling application failed
    };
    
    SchemeHandler();
    ~SchemeHandler();
    
    SchemeHandlerError HandleSpecialScheme(const QUrl &url);

private:
    SchemeHandlerPrivate *d;
};

}
#endif /* def SCHEME_HANDLER_H */
