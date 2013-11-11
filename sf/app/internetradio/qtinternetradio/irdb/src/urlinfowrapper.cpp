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

#include <QSqlQuery>
#include <QVariant>
#include <QStringList>

#include "urlinfowrapper.h"
#include "irdb.h"

urlInfoWrapper::urlInfoWrapper()
{     
}

urlInfoWrapper::~urlInfoWrapper()
{
} 

bool urlInfoWrapper::resetUrlInfo(const columnUrlInfoInsertMap* const RowData,
                                  const uint& channelId)
{
    QStringList insSqlStrList;

    if(!channelId)
    {
        return false;	
    }

    if( NULL == RowData )
    {
        return false;
    }

    if(RowData->isEmpty())
    {
        return false;
    }

    combineInsertStr(RowData, channelId, insSqlStrList);
    return m_pIRDB->resetUrlInfo(insSqlStrList, channelId)? false:true;  
}

void urlInfoWrapper::combineInsertStr(const columnUrlInfoInsertMap* const RowData,
                                      const uint& channelId,	    
                                      QStringList& insSqlStrList)
{
    columnUrlInfoInsertMap::const_iterator it;
    QString insSqlStr;  
    it = RowData->begin();
    while(it != RowData->end())
    {
        insSqlStr = "Insert into urlinfo(channelUrl, channelId, bitrate) values(";
        insSqlStr += "'" +it.key()+ "'" +", ";
        insSqlStr += QString::number(channelId);
        insSqlStr += ", ";
        insSqlStr += QString::number(it.value());  
        insSqlStr += " )" ;
        
        insSqlStrList<<insSqlStr;
        
        ++it;
    }
    
    return;        
}

QList<QVariant*>* urlInfoWrapper::getUrlInfo(const columnMap* const condAND,  
                                             const columnMap* const condOR,
                                             int aBegin,
                                             int aEnd)
{
    QString sltSqlStr = "select channelUrl, channelId, bitRate from urlInfo ";
    QList<QVariant*>* pDataSet = NULL;
    
    if( (NULL != condAND)&&(NULL != condOR) )
    {
        return NULL;
    }

    if(condAND)
    {
        if(condAND->isEmpty())
        {
            return false;
        }
    }

    if(condOR)
    {
        if(condOR->isEmpty())
        {
            return false;
        }
    }

    combineGetStr(condAND, condOR, colNameView, sltSqlStr);
    pDataSet = new QList<QVariant*>(); 
    if( m_pIRDB->selectRow(this, sltSqlStr, pDataSet, aBegin, aEnd) )
    {
        delete pDataSet;
        pDataSet = NULL;
    }
    
    return pDataSet;
}

bool urlInfoWrapper::getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pDataSet)
{
    while (aIRDataSet.next()) 
    {
        QVariant* const p = new QVariant[IRDB_URLINFO_COLUMN];
        *pDataSet<<p;

        for(int i = 0; i < IRDB_URLINFO_COLUMN; i++)
        {
            *(p+i) = aIRDataSet.value(i);
        }
    }

    return true;
}


