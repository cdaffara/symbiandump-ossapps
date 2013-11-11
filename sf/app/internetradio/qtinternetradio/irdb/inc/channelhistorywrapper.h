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

#ifndef _CHANNEL_HISTORY_WRAPPER_H_
#define _CHANNEL_HISTORY_WRAPPER_H_

#include "irdbexport.h"
#include "irdbwrapper.h"

class IRDB_DLL_EXPORT channelHistoryWrapper: public IRDBWrapper
{
public:
    channelHistoryWrapper();
    ~channelHistoryWrapper();

public:
    /*
    * RowData:        [direction: in]table row value;
    *                 reminder, channelUrl and bitrate can't be input from here;
    *                 if the enum of channelUrl or bitrate is used here, return false;
    * cidUserDefined: [direction: out] return the channelid allocated by IRDB, 
    *                                  the channelId is a user-defined channelId;
    * logoMap: [direction: in]the logo rowData will be inserted into irdb via this structure;
    *                         the logo rowData should be filled into logoMap as below:
    *                         logoMap* pLogo = new logoMap(); 
    *                         QByteArray bLog("XXXXXXXXXXXXXXXX");
    *                         QByteArray sLog("ssssssssssssssss"); 
    *                         pLogo->insert(bLogoInsert, bLog); 
    *                         pLogo->insert(sLogoInsert, sLog); 
    *                         
    * condAND: [direction: in]condition for search channelHistory, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search channelHistory, it is OR  relationship in every pair;
    */ 
    bool putChannelHistory(const columnMap* const RowData,
                           uint* const cidUserDefined = NULL,
                           const logoMap*   const logoData = NULL,
                           const columnMap* const condAND = NULL,
                           const columnMap* const condOR = NULL);  
    
    /*
    * If all parameters are default,all rows in channelHistory will be deleted if not violating constraint;
    * condAND: [direction: in]condition for search channelHistory, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search channelHistory, it is OR  relationship in every pair;
    */     
    bool deleteChannelHistory(const columnMap* const condAND = NULL,
                              const columnMap* const condOR = NULL);

    /*
    * If all parameters are default, all rows in channelHistory will return;
    * condAND: [direction: in]condition for search channelHistory, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search channelHistory, it is OR  relationship in every pair;
    */                       
    QList<QVariant*>* getChannelHistory(const columnMap* const condAND = NULL,  
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
    * condAND:       [direction: in]condition for search channelHistory, it is AND relationship in every pair;
    * condOR:        [direction: in]condition for search channelHistory, it is OR  relationship in every pair;
    * uint:          return channelId;
    */    
    uint srhChannelId(const columnMap* const condAND = NULL,
                      const columnMap* const condOR = NULL);

};

#endif

