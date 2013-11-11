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
#include "searchrltwrapper.h"
#include "irdb.h"

searchRltWrapper::searchRltWrapper()
{     
}

searchRltWrapper::~searchRltWrapper()
{
} 

void searchRltWrapper::combineSqlStr(const uint &cid,
                                     const columnMap* const RowData, 
                                     QStringList &sqlList)
{
    columnMap::const_iterator it;
    QString insSqlstrCnlInfo = "insert into channelInfo( ";
    QString insSqlstrImg     = "insert into img( ";
    QString insSqlstrSrhRlt  = "insert into searchrlt( "; 
    QString escStr;
    QString condStr = " where channelId ='" + QString::number(cid) + "' ";

    bool FstOfStatementCnl = true;
    bool FstOfStatementImg = true;

    it = RowData->begin();
    while(it != RowData->end())
    {
        if(channelId == it.key()) 
        {
            if(false == FstOfStatementCnl)
            {
                insSqlstrCnlInfo += ", ";                
            }
        
            if(false == FstOfStatementImg)
            {
                insSqlstrImg += ", ";                
            } 
            
            insSqlstrCnlInfo += colNameView[it.key()];
            insSqlstrImg += colNameView[it.key()];
            insSqlstrSrhRlt += colNameView[it.key()];
            FstOfStatementCnl = false;
            FstOfStatementImg = false;
        }
        
        if( (channelName == it.key()) ||(shortDesc == it.key()) )
        {
            if(false == FstOfStatementCnl)
            {
                insSqlstrCnlInfo += ", ";                
            }
            
            insSqlstrCnlInfo += colNameView[it.key()];
            FstOfStatementCnl = false;
        }
        
        if(imgUrl == it.key())
        {
            if(false == FstOfStatementImg)
            {
                insSqlstrImg += ", ";                
            }
            
            insSqlstrImg += colNameView[it.key()];
            FstOfStatementImg = false;
        }

        ++it;
    }
    
    insSqlstrCnlInfo += ") values(";
    insSqlstrImg += ") values(";
    insSqlstrSrhRlt += ") values(";
    FstOfStatementCnl = true;
    FstOfStatementImg = true;
        
    it = RowData->begin();
    while(it != RowData->end())
    {
        //here for escape char,
        escStr = it.value();
        escStr.replace('\'', "\'\'");

        if(channelId == it.key()) 
        {
            if(false == FstOfStatementCnl)
            {
                insSqlstrCnlInfo += ", ";                
            }
            
            if(false == FstOfStatementImg)
            {
                insSqlstrImg += ", ";                
            }
            
            insSqlstrCnlInfo  = insSqlstrCnlInfo + "'"+ escStr+ "'";
            insSqlstrImg      = insSqlstrImg + "'"+ escStr+ "'";
            insSqlstrSrhRlt   = insSqlstrSrhRlt + "'"+ escStr+ "'";
            FstOfStatementCnl = false;
            FstOfStatementImg = false;
        }
        
        if( (channelName == it.key()) ||(shortDesc == it.key()) )
        {
            if(false == FstOfStatementCnl)
            {
                insSqlstrCnlInfo += ", ";                
            }
        
            insSqlstrCnlInfo = insSqlstrCnlInfo + "'"+ escStr+ "'";
            FstOfStatementCnl = false;                
        }

        if(imgUrl == it.key())
        {
            if(false == FstOfStatementImg)
            {
                insSqlstrImg += ", ";                
            }
        
            insSqlstrImg = insSqlstrImg + "'"+ escStr+ "'";
            FstOfStatementImg = false;                  
        }            
          
        ++it;
    
    }    
    
    insSqlstrImg += ") ";
    insSqlstrCnlInfo += ") ";
    insSqlstrSrhRlt += ") ";
    
    QString updSqlStrCnlInfo = "update channelInfo set ";
    QString updSqlStrCnlImg = "update img set ";
    //part two, create update string;
    FstOfStatementCnl = true;
    FstOfStatementImg = true;
    
    it = RowData->begin();
     
    while( it != RowData->end())
    {
            //here for escape char,
        escStr = it.value();
        escStr.replace('\'', "\'\'");
        if( (channelName == it.key()) ||(shortDesc == it.key()) )
        {
            if(false == FstOfStatementCnl)
            {
                updSqlStrCnlInfo += ", ";
            }
            updSqlStrCnlInfo = updSqlStrCnlInfo + colNameView[it.key()] + "=" + "'" + escStr + "'";
            FstOfStatementCnl = false;
        }
        if(imgUrl == it.key())
        {
            updSqlStrCnlImg = updSqlStrCnlImg + colNameView[it.key()] + "=" + "'" + escStr + "'";
        }  

        ++it;
    }
    updSqlStrCnlInfo += condStr;
    updSqlStrCnlImg  += condStr;
    sqlList<<insSqlstrCnlInfo<<insSqlstrImg<<insSqlstrSrhRlt<<updSqlStrCnlInfo<<updSqlStrCnlImg ;
    
    return;
}


bool searchRltWrapper::addSearchRlt(const columnMap* const RowData, bool bOpt)
{
    uint uCid = 0;
    QStringList sqlStrList;
    
    if( NULL == RowData )
    {
        return false;
    }

    if(RowData->isEmpty())
    {
        return false;
    }
    uCid = srhChannelId(RowData);
    
    if(0 == uCid)
    {
        return false;
    }
    
    combineSqlStr(uCid, RowData, sqlStrList);
    
    return m_pIRDB->handleRstFromSrhView(uCid, sqlStrList, bOpt)? false:true;
          
}

void searchRltWrapper::addSearchRltFinished()
{
    m_pIRDB->closeIRDBConnection();
    return;
}

/*
* this fuction don't suport "update" operation for mutliple records ;
* if condAND != NULL AND condOR != NULL, illegal operation;
* if condAND != NULL or condOR != NULL, it must be "update" operation.
* if condAND == NULL AND condOR == NULL, it is "insert" or "update" operation;
* 
*/
bool searchRltWrapper::putSearchRlt(const columnMap* const RowData,
                                    const logoMap*   const logoData,
                                    const columnMap* const condAND,
                                    const columnMap* const condOR)
{
    uint uCid = 0;
    QString insSqlStr;
    QString updSqlStr;
    QString condSqlStr;       
    QString condUserCidStr;
    columnMap* const RowDataAppend = const_cast<columnMap*>(RowData);
    QList<QByteArray>* pImgList = NULL;
    bool ret = true;
    int logoType = 0;
    
    if( NULL == RowData )
    {
        return false;
    }

    if(RowData->isEmpty())
    {
        return false;
    }


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
    
    insSqlStr = "insert into IRBuff ";   
    updSqlStr = "update IRBuff set ";
    RowDataAppend->insert(dataSource, QString::number(DATA_OF_SEARCHRLT));
    RowDataAppend->insert(opt, QString::number(IRDB_OPT_INSERT));    
 
    if(NULL != logoData)
    {
        pImgList = new QList<QByteArray>();
        combinePutStr(RowDataAppend, colNameView, insSqlStr, updSqlStr, logoData, pImgList, &logoType); 
    
    }
    else
    {
        combinePutStr(RowDataAppend, colNameView, insSqlStr, updSqlStr);
    }      
    
    if( (NULL == condAND)&&(NULL == condOR) )
    {
        uCid = srhChannelId(RowData);
    }
    else //here it must be "update" operation; condition string 
    {
        uCid = srhChannelId(condAND, condOR);
        combineCondStr(condAND, condOR, colNameView,condSqlStr);
    }

    if(uCid)
    {
        //updSqlStr += condSqlStr;
        m_pIRDB->chgRowIRDB(insSqlStr, updSqlStr, uCid, condSqlStr, pImgList, logoType)? ret = false:true;
    }
    else //here deliver the updSqlstr and condSqlStr seperately.
    {
        m_pIRDB->chgRowIRDB(insSqlStr, updSqlStr, NULL, condSqlStr, condUserCidStr, pImgList, logoType)?ret = false:true;
    }    

    if(pImgList)
    {
        delete pImgList;
        pImgList = NULL;
    }
    
    return ret;
}    

QList<QVariant*>*  searchRltWrapper::getSearchRlt(const columnMap* const condAND,  
                                                  const columnMap* const condOR)
{
    QString sltSqlStr = "select * from IRVIEW_searchRlt ";
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
    pDataSet = new QList<QVariant*>();
    if( m_pIRDB->selectRow(this, sltSqlStr, pDataSet) )
    {
        delete pDataSet;
        pDataSet = NULL;
    }
    
    return pDataSet;
}


bool searchRltWrapper::deleteSearchRlt(const columnMap* const condAND,
                                       const columnMap* const condOR)
{
    QString deleteSqlStr = "delete from searchRlt ";
    
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


bool searchRltWrapper::getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pDataSet)
{

    while (aIRDataSet.next()) 
    {
        QVariant* const p = new QVariant[IRDB_SEARCHRLT_COLUMN];

        *pDataSet<<p;

        for(int i = 0; i < IRDB_SEARCHRLT_COLUMN; i++)
        {
            *(p+i) = aIRDataSet.value(i);
        }
    }

    return true;
}

uint searchRltWrapper::srhChannelId(const columnMap* const condAND,
                                    const columnMap* const condOR)
{
    uint srhCID = 0;

    //if condAND or condOR has channeld,
    if(condAND != NULL)
    {
        srhCID = (condAND->value(channelId)).toUInt();         
    } 
    else if(condOR != NULL)
    {
        srhCID = (condOR->value(channelId)).toUInt();         
    }
#if 0
    if(srhCID)
    {
        return srhCID ;
    }

    //channelId column is false
    //only one case, go to station view, channelUrl and channelNickName
    //search channelId according to urlinof and channelNickName;
    QString srhStr;
    if(condAND != NULL)
    {
        if( ( !(condAND->value(channelUrl).isEmpty())  )&&( !(condAND->value(channelNickName).isEmpty()) ) )
        {
            srhStr = "where "+ colNameView[channelUrl] + " = ";
            srhStr = srhStr + "'" + condAND->value(channelUrl) + "' AND " ;
            srhStr = srhStr + colNameView[channelNickName] + " = ";
            srhStr = srhStr + "'" + condAND->value(channelNickName) + "'";
        }
        condUserCidStr = srhStr;
    }
#endif          
    return srhCID;
}

