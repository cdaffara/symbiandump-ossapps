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
* Description: Manipulate Interface of Internet Radio Database 
*
*/

#ifndef _IRDB_WRAPPER_H_
#define _IRDB_WRAPPER_H_

#include <QList>
#include "irdbexport.h"
#include "irdbviewstruct.h"

class QVariant;
class QSqlQuery;
class IRDB;


class IRDB_DLL_EXPORT IRDBWrapper
{
public:

    IRDBWrapper();

    virtual ~IRDBWrapper();
    
public:
    /*
    * If all parameters are default, all cid rows in IRDB will return;
    * condAND: condition for search IRDB, it is AND relationship in every pair;
    * condOR: condition for search IRDB, it is OR  relationship in every pair;
    * here below enum can be input as condition(condAND or condOR):
    * channelId,        channelName,   channelNickName,  
      genreName,        genreId,       languageName, languageCode,  
      countryName,      countryCode,   description,  shortDesc,    
      lastModified,     channelType,   musicStoreStatus, 
      imgUrl,           
      advertisementUrl, advertisementInUse, expiration,
      channelUrl, bitRate;
    */                       
    QList<uint>* getChannelId(const columnMap* const condAND = NULL,  
                              const columnMap* const condOR  = NULL);


    /*
    * If all parameters are default, all rows in IRDB will return;
    * condAND: condition for search IRDB, it is AND relationship in every pair;
    * condOR: condition for search IRDB, it is OR  relationship in every pair;
    */                       
    QList<QVariant*>* getIRDB(const columnMap* const condAND = NULL,  
                              const columnMap* const condOR  = NULL);

    friend class IRDB;

protected:
    /*
    * create sql condition(where) string according to the input condAND and condAND;
    * condAND:      [direction:in] it is AND relationship in every pair;
    * condOR:       [direction:in] it is OR  relationship in every pair;
    * pArrayColName:[direction:in] array pointer to colName<table>[];
    *               for example:colNameView[],colNameChannelHistory[];
    * condSqlStr:   [direction:out] return the sqlString;  
    */
    void combineCondStr(const columnMap* const condAND,  
                        const columnMap* const condOR,   
                        const QString* pArryColName,
                        QString& condSqlStr);       

    /*
    * create sql insert/update string according to RowData;
    * RowData:      [direction:in] row value of table;
    * pArrayColName:[direction:in] array pointer to colName<table>[];
    *               for example:colNameView[],colNameChannelHistory[];
    * insSqlStr:   [direction:out] return the sqlString; 
    * updSqlStr:   [direction:out] return the sqlString;  
    */                              
    void combinePutStr(const columnMap* const RowData,
                       const QString* pArryColName,
                       QString& insSqlStr, 
                       QString& updSqlStr,
                       const logoMap* const logoData = NULL,
                       QList<QByteArray>* logoArrayList = NULL,
                       int* logoType = NULL);
    
    /*
    * create sql select string according to the input condAND and condOR;
    * condAND:      [direction:in] it is AND relationship in every pair;
    * condOR:       [direction:in] it is OR  relationship in every pair;
    * pArrayColName:[direction:in] array pointer to colName<table>[];
    *               for example:colNameView[],colNameChannelHistory[];
    * sltSqlStr:    [direction:out] return the sqlString;  
    */   
    void combineGetStr(const columnMap* const condAND,
                       const columnMap* const condOR,
                       const QString* pArryColName,
                       QString& sltSqlStr);    

    /*
    * create sql delete string according to the input condAND and condOR;
    * condAND:      [direction:in] it is AND relationship in every pair;
    * condOR:       [direction:in] it is OR  relationship in every pair;
    * pArrayColName:[direction:in] array pointer to colName<table>[];
    *               for example:colNameView[],colNameChannelHistory[];
    * dltSqlStr:    [direction:out] return the sqlString;  
    */  
    void combineDeleteStr(const columnMap* const condAND,
                          const columnMap* const condOR,
                          const QString* pArryColName,
                          QString& dltSqlStr);     

private:

    /*
    * It is called by IRDB instance;
    * IRDB instance delivers rows set(channelId) to IRDBWrapper via this function;
    * the rows set is storaged in m_IRCIDSet; 
    */
    bool loadDataOfChannelIdCB(QSqlQuery& aIRDataSet, QList<uint>* pCIDDataSet);

    /*
    * It is called by IRDB instance;
    * IRDB instance delivers rows set to IRDBWrapper via this function;
    * the rows set is storaged in m_IRDataSet;
    */
    bool loadDataOfIRDBCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pViewDataSet);
    
    /*
    * it is for the single table;  
    */
    virtual bool getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pTableDataSet); 

protected:
    //pointer for IRDB;
    IRDB*              m_pIRDB;    
};


#endif



