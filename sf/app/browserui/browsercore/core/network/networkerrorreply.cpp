/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "networkerrorreply.h"

namespace WRT {


/*!
 * \class NetworkErrorReply
 * A specialization of the abstract base class QNetworkReply. This can be returned by 
 * WebNetworkAccessManager::createRequest() to generate an error without actually
 * submitting a request to the network, thus blocking network access. 
 */

/*!
 * Constructor
 * \param[in] errorCode - The error code to set
 * \param[in] errorString - The error text to set
 */


  NetworkErrorReply::NetworkErrorReply(NetworkError errorCode, const QString& errorString, const QUrl& url)
    : QNetworkReply()
      
  {
    setError(errorCode,errorString);
    setUrl(url);
  }
  
  void NetworkErrorReply::abort()
  {
    
  }
  
    qint64 NetworkErrorReply::readData(char* /*data*/, qint64 /*size*/){
    return 0;
  }
}
