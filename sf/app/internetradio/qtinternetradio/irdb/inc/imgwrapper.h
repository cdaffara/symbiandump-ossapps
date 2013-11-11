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
* Description: 
*
*/

#ifndef _IMG_WRAPPER_H_
#define _IMG_WRAPPER_H_

#include "irdbexport.h"
#include "irdbwrapper.h"

class IRDB_DLL_EXPORT imgWrapper: public IRDBWrapper
{
public:
    imgWrapper();
    ~imgWrapper();

public:
    /*
    * RowData:   [direction: in] table row value, only accept imgUrl and imgLocalFile column;
    * channelId: [direction: in] channelid for update row;
    * return true if operation success; 
    */     
    bool updateImg(const columnMap* const RowData,
                   const uint& channelId,
                   const logoMap* const logoData = NULL);    

    QList<QVariant*>* getImg(const columnMap* const condAND = NULL,  
                             const columnMap* const condOR = NULL);

private:
    /*
    * It is called by IRDB instance;
    * IRDB instance delivers rows set to IRDBWrapper via this function;
    * the rows set is storaged in m_IRDataSet;
    */
    bool getIRTableCB(QSqlQuery& aIRDataSet, 
                      QList<QVariant*>* pDataSet);
 
};

#endif

