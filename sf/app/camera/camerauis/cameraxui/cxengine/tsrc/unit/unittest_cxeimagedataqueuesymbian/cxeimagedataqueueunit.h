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
#ifndef CXEIMAGEDATAQUEUEUNIT_H
#define CXEIMAGEDATAQUEUEUNIT_H

//  Include Files

#include "cxeimagedataqueuesymbian.h"

//  Constants

//  Class Definitions

class CxeImageDataQueueUnit : public CxeImageDataQueueSymbian
{
    Q_OBJECT

public:
    CxeImageDataQueueUnit(); 
    virtual ~CxeImageDataQueueUnit();

protected: // helper methods
    virtual CxeImageDataItem *createDataItem( int index, const QByteArray& data, const QString& path ) const;

};



#endif  // CXEIMAGEDATAQUEUEUNIT_H
