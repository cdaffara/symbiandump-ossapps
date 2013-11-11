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
#include "imgwrapper.h"
#include "irdb.h"

imgWrapper::imgWrapper()
{     
}

imgWrapper::~imgWrapper()
{
} 


bool imgWrapper::updateImg(const columnMap* const RowData,
                           const uint& channelId,
                           const logoMap* const logoData)
{
    QString insSqlStr;
    QString updSqlStr;
    QList<QByteArray>* pImgList = NULL;
    bool ret = true;
    int logoType;
    
    if(!channelId)
    {
        return false;	
    }

    if( ( NULL == RowData )&&(NULL ==logoData) )
    {
        return false;
    }
    if(RowData)
    {
        if(RowData->isEmpty())
        {
            return false;
        }

        if( true != (RowData->value(channelId)).isEmpty() )
        {
            return false;
        }
    }    

    updSqlStr = "update img set ";
    if(NULL != logoData)
     {
         pImgList = new QList<QByteArray>();
         combinePutStr(RowData, colNameView, insSqlStr, updSqlStr, logoData, pImgList, &logoType); 
     
     }
     else
     {
         combinePutStr(RowData, colNameView, insSqlStr, updSqlStr);
     }  
    
    //create insSqlstr and updSqlStr in advance;
    updSqlStr = updSqlStr + "where channelId = " + QString::number(channelId);
    
    m_pIRDB->updRowImg(channelId, updSqlStr, pImgList, logoType)? ret = false:true;
    if(pImgList)
    {
        delete pImgList;
        pImgList = NULL;
    }
    
    return ret;     
}

QList<QVariant*>* imgWrapper::getImg(const columnMap* const condAND,  
                                   const columnMap* const condOR)
{
    QString sltSqlStr = "select * from img ";
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
    pDataSet = new  QList<QVariant*>();
    
    if( m_pIRDB->selectRow(this, sltSqlStr, pDataSet) )
    {
        delete pDataSet;
        pDataSet = NULL;
    }
    
    return pDataSet;
}

bool imgWrapper::getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pDataSet)
{

    while (aIRDataSet.next()) 
    {
        QVariant* const p = new QVariant[IRDB_CHANNELINFO_COLUMN];
        *pDataSet<<p;

        for(int i = 0; i < IRDB_CHANNELINFO_COLUMN; i++)
        {
            *(p+i) = aIRDataSet.value(i);
        }
    }

    return true;
}


