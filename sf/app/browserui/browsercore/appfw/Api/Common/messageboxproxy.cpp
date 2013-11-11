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

#include "messageboxproxy.h"

namespace WRT {

 /*!
 * \class MessageBoxProxy
 *
 * \brief Proxy for display QMessageBox
 *
 * This class is responsible for displaying QmessageBox in Chrome.
 * All model level classes which need to display a QMessageBox should inherit from this class,
 * and implement virtual function onMessageBoxResponse(int).
 *
 */

/*!
  Basic MessageBoxProxy constructor, which does nothing
*/
MessageBoxProxy::MessageBoxProxy()
{
}
/*!
  Basic MessageBoxProxy destructor, which does nothing
*/
MessageBoxProxy::~MessageBoxProxy()
{
}
}
