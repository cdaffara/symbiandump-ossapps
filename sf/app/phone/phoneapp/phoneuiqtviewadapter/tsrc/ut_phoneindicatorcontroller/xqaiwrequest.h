/*!
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
* Description:  
*/

#ifndef XQAIWREQUETS_H_
#define XQAIWREQUETS_H_

 
#include <QObject>
 
/*!
 * Mock
 */
class XQAiwRequest: public QObject
{
Q_OBJECT
 
public:
 
     XQAiwRequest();
     virtual ~XQAiwRequest();
     
 
 public slots:
    void setArguments(const QList<QVariant> &/*arguments*/);
     
     bool send();
     
 
     int lastError() const;
     
    
 
};

#endif // XQAIWREQUEST_H_
