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

// INCLUDE FILES
#ifndef __WRTTELSERVICE_H
#define __WRTTELSERVICE_H

#include <QtCore/QObject>
#include <QString>

  
// FORWARD DECLARATIONS
class WrtTelServicePrivate;


// CLASS DECLARATION

/**
*  Implements telephony services for browser.
*  This class implements required telephony services for multipurpose browser.
*/
class WrtTelService : public QObject {
  
	  Q_OBJECT
	  
public:
	
	  WrtTelService();
		~WrtTelService();

		void MakeCall( QString number, bool sendDTMFAlwaysConfirm );

		void MakeVOIPCall( QString number, bool sendDTMFAlwaysConfirm );
		
private:
    WrtTelServicePrivate* d;
	};

#endif // __WRTTELSERVICE_H
