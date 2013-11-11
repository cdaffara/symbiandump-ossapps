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

#ifndef _SEARCH_RLT_WRAPPER_H_
#define _SEARCH_RLT_WRAPPER_H_

#include "irdbexport.h"
#include "irdbwrapper.h"

class IRDB_DLL_EXPORT searchRltWrapper: public IRDBWrapper
{
public:
    searchRltWrapper();
    ~searchRltWrapper();

public:
    /*
    * RowData: [direction: in] table row value
    * bOpt:    [direction: in] true = the db connection should be open;
    */
    bool addSearchRlt(const columnMap* const RowData, 
                      bool bOpt = true);    
    
    void addSearchRltFinished();
    /*
    * RowData: [direction: in]table row value;
    * condAND: [direction: in]condition for search searchRlt, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search searchRlt, it is OR  relationship in every pair;
    */     
    bool putSearchRlt(const columnMap* const RowData,
                      const logoMap*   const logoData = NULL,
                      const columnMap* const condAND = NULL,
                      const columnMap* const condOR = NULL);  
    
    /*
    * If all parameters are default,all rows in searchRlt will be deleted if not violating constraint;
    * condAND: [direction: in]condition for search searchRlt, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search searchRlt, it is OR  relationship in every pair;
    */   
    bool deleteSearchRlt(const columnMap* const condAND = NULL,
                         const columnMap* const condOR = NULL);

    /*
    * If all parameters are default, all rows in searchRlt will return;
    * cond1: condition for search searchRlt, it is AND relationship in every pair;
    * cond2: condition for search searchRlt, it is OR  relationship in every pair;
    */                       
    QList<QVariant*>* getSearchRlt(const columnMap* const condAND = NULL,  
                                   const columnMap* const condOR = NULL);

private:
    /*
    * It is called by IRDB instance;
    * IRDB instance delivers rows set to IRDBWrapper via this function;
    * the rows set is storaged in m_IRDataSet;
    */
    bool getIRTableCB(QSqlQuery& aIRDataSet,
                      QList<QVariant*>* pDataSet);

private:
    /*
    * search channelId according to parameter;
    * condUserCidStr:[direction: in]specific condition(where) string for user-define uid;
    * condAND:       [direction: in]condition for search channelHistory, it is AND relationship in every pair;
    * condOR:        [direction: in]condition for search channelHistory, it is OR  relationship in every pair;
    * uint:          return channelId;
    */    
    uint srhChannelId(const columnMap* const condAND = NULL,
                      const columnMap* const condOR = NULL);
    /*
    * create QString List for handle(insert/update) the data come from search view
    * channelId: [direction: in] channel id;
    * RowData:   [direction: in] input data;
    * sqlList:   [direction: out] return sql string; 
    */
    void combineSqlStr(const uint& channelId,
                       const columnMap* const RowData, 
                       QStringList& sqlList);
};

#endif
