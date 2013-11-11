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

#ifndef _URLINFO_WRAPPER_H_
#define _URLINFO_WRAPPER_H_

#include "irdbexport.h"
#include "irdbwrapper.h"

class IRDB_DLL_EXPORT urlInfoWrapper: public IRDBWrapper
{
public:
    urlInfoWrapper();
    ~urlInfoWrapper();

public:
    /*
    * RowData:   [direction: in] column value for channelUrl and  bitrate;
    *                            first oprand in QMap is channelUrl,
    *                            sencod oprand in QMap is bitrate;
    * channelId: [direction: in] channelid for update row;
    * return true if operation success; 
    */     
    bool resetUrlInfo(const columnUrlInfoInsertMap* const RowData,
                      const uint& channelId);    
    /*
    * aBegin, aBegin is 0, will invoke the action of open db connection;
    * aBegin == aEnd, will invoke the action of close db connection;
    */
    QList<QVariant*>* getUrlInfo(const columnMap* const condAND = NULL,  
                                 const columnMap* const condOR = NULL,
                                 int aBegin = 0,
                                 int aEnd = 0);

private:
    /*
    * It is called by IRDB instance;
    * IRDB instance delivers rows set to IRDBWrapper via this function;
    * the rows set is storaged in m_IRDataSet;
    */
    bool getIRTableCB(QSqlQuery& aIRDataSet,
                      QList<QVariant*>* pDataSet);

    /*
    * create sql insert string according to the input RowData;
    * RowData:       [direction:in] refer to line 29;
    * channelId:     [direction:in] it is row channelId;
    * insSqlStrList: [direction:out] return the sql QStringList;  
    */    
    void combineInsertStr(const columnUrlInfoInsertMap* const RowData,
                          const uint& channelId,	    
                          QStringList& insSqlStrList);    
};

#endif

