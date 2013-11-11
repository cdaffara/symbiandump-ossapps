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
#include "favoriteswrapper.h"
#include "irdb.h"

favoritesWrapper::favoritesWrapper()
{     
}

favoritesWrapper::~favoritesWrapper()
{
} 

bool favoritesWrapper::putFavorites(columnMap* const RowData,
                                    bool& bNewRow)
{
    uint uCid = 0;
    QString insSqlStr;
    QString updSqlStr;


    if( NULL == RowData )
    {
        return false;
    }

    if(RowData->isEmpty())
    {
        return false;
    }
  
    uCid = (RowData->value(channelId)).toUInt();


    if(!uCid)
    {
        return false;	
    }
    
    insSqlStr = "insert into favorites ";   
    updSqlStr = "update favorites set ";    
    
    //create insSqlstr and updSqlStr in advance;
    combinePutStr(RowData, colNameView, insSqlStr, updSqlStr); 

    return m_pIRDB->chgRowFavorites(insSqlStr, updSqlStr, uCid, bNewRow)? false:true;  
}

bool favoritesWrapper::deleteFavorites(const columnMap* const condAND,
                                       const columnMap* const condOR)
{
    QString deleteSqlStr = "delete from Favorites ";
    
    if( (NULL != condAND)&&(NULL != condOR) )
    {
        return false;
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

    combineDeleteStr(condAND, condOR, colNameView, deleteSqlStr);
   
    return m_pIRDB->deleteRow(deleteSqlStr)? false:true;
}

QList<QVariant*>* favoritesWrapper::getFavorites(const columnMap* const condAND,  
                                                 const columnMap* const condOR)
{
    QString sltSqlStr = "select * from IRVIEW_favorites ";
    QList<QVariant*>* pDataSet = NULL;
    
    if( (NULL != condAND)&&(NULL != condOR) )
    {
        return NULL;
    }

    if(condAND)
    {
        if(condAND->isEmpty())
        {
            return NULL;
        }
    }

    if(condOR)
    {
        if(condOR->isEmpty())
        {
            return NULL;
        }
    }

    combineGetStr(condAND, condOR, colNameView, sltSqlStr);
//    sltSqlStr = sltSqlStr + " order by FavSeq desc ";
    pDataSet = new QList<QVariant*>();
    if( m_pIRDB->selectRow(this, sltSqlStr, pDataSet) )
    {
        delete pDataSet;
        pDataSet = NULL;
    }
    
    return pDataSet;
}

bool favoritesWrapper::getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pDataSet)
{
    while (aIRDataSet.next()) 
    {
        QVariant* const p = new QVariant[IRDB_FAVORITES_COLUMN];
        *pDataSet<<p;

        for(int i = 0; i < IRDB_FAVORITES_COLUMN; i++)
        {
            *(p+i) = aIRDataSet.value(i);
        }
    }

    return true;
}


