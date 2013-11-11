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
#include "songhistorywrapper.h"
#include "irdb.h"

songHistoryWrapper::songHistoryWrapper()
{     
}

songHistoryWrapper::~songHistoryWrapper()
{
} 

bool songHistoryWrapper::putSongHistory(const columnMap* const RowData,
                                        const columnMap* const condAND,
                                        const columnMap* const condOR)
{
    QString sSongName;
    QString sArtistName;
    uint uCid = 0;
    QString insSqlStr;
    QString updSqlStr;
    QString condSqlStr;       


    if( NULL == RowData )
    {
        return false;
    }

    if(RowData->isEmpty())
    {
        return false;
    }

    if(NULL != condOR)
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

    insSqlStr = "insert into songHistory ";
    updSqlStr = "update songHistory set ";        
    combinePutStr(RowData,colNameView, insSqlStr, updSqlStr); 
    
    if(condAND == NULL) //for update or insert;
    {
        /*
        * insert string, there must be have songName and ChannelId;
        * update string, there must have songName, channelId and artistname. 
        */  
        sSongName = RowData->value(songName);
        uCid = (RowData->value(channelId)).toUInt();
        sArtistName = (RowData->value(artistName));
        
        if( (sSongName.isEmpty()) || (0 == uCid) )
        {
            return false;
        }
        if(sArtistName.isEmpty())
        {
            /*
            * In songhistorywrapper.cpp, 
            * if (songname not change && artistname not change && channeled not change) then update, 
            * otherwise insert a new one.
            */
            updSqlStr.clear();
        }
        else
        {
            //combine cond str;
            //combineCondStr(RowData, NULL, colNameSongHistory, condSqlStr); 
            condSqlStr = "where songName = '" + sSongName +"'";
            condSqlStr += " AND ";
            condSqlStr = condSqlStr + "channelId = " + QString::number(uCid);
            condSqlStr += " AND ";
            condSqlStr = condSqlStr + "artistName = '" + sArtistName+"'";
        }
        
    }    
    else //if condtion isn't null, it is update. for update, songName must be in condAND or condOR; 
    {
        /*
        * if cond != NULL,
        * there must have songname and channelId and artistName in cond AND 
        * there must have not channelId in Rowdata
        */
        if(!(RowData->value(channelId)).isEmpty())
        {
            return false;
        }
        sSongName = condAND->value(songName);
        uCid = (condAND->value(channelId)).toUInt();
        sArtistName = (condAND->value(artistName));
        
        if( ( sSongName.isEmpty()) || (0 == uCid) || (sArtistName.isEmpty()) )
        {
            return false;
        }
        
        //combine update string;
        combineCondStr(condAND, NULL, colNameView, condSqlStr);    
        insSqlStr.clear();
        //updSqlStr += condSqlStr;
    }
    
    return m_pIRDB->chgRowSongHistory(insSqlStr, updSqlStr, condSqlStr, sSongName, uCid)? false:true;     
}

bool songHistoryWrapper::deleteSongHistory(const columnMap* const condAND,
                                           const columnMap* const condOR)
{
    QString deleteSqlStr = "delete from SongHistory ";
    
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


QList<QVariant*>* songHistoryWrapper::getSongHistory(const columnMap* const condAND,  
                                                     const columnMap* const condOR)
{
    QString sltSqlStr = "select * from IRVIEW_SONGHISTORY ";
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
            return false;
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

bool songHistoryWrapper::getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pDataSet)
{
    while (aIRDataSet.next()) 
    {
        QVariant* const p = new QVariant[IRDB_SONGHISTORY_COLUMN];
        *pDataSet<<p;

        for(int i = 0; i < IRDB_SONGHISTORY_COLUMN; i++)
        {
            *(p+i) = aIRDataSet.value(i);
        }
    }

    return true;

}


