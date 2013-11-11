/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 *
 */
#ifndef NMAPIMESSAGEBODY_P_H_
#define NMAPIMESSAGEBODY_P_H_

#include <QString>

namespace EmailClientApi
{

class NmApiMessageBodyPrivate : public QSharedData
{
public:
    NmApiMessageBodyPrivate(){};
    virtual ~NmApiMessageBodyPrivate(){};
	
    quint64 totalSize;
    quint64 fetchedSize;
    QString content;
};

}
#endif /* NMAPIMESSAGEBODY_P_H_ */
