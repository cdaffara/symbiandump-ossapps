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
* Contributors:
*
* Description:
*
*/

#include <QSqlQuery>
#include <QVariant>
#include "irdbwrapper.h"
#include "irdb.h"


IRDBWrapper::IRDBWrapper()//: m_BuffFlag(true)
{     
    m_pIRDB = IRDB::getInstance();
}

    
IRDBWrapper::~IRDBWrapper()
{
    if(m_pIRDB)
    {  	
        m_pIRDB->releaseInstance();
        m_pIRDB = NULL;
    }
} 


/*
* If all parameters are default, all rows in IRDB will return;
*/                       
QList<QVariant*>* IRDBWrapper::getIRDB(const columnMap* const condAND,  
                                 const columnMap* const condOR)
{
    QString condSqlStr;
    QList<QVariant*> *pIRDataSet = NULL;
    
    combineCondStr(condAND, condOR, colNameView, condSqlStr);

    if( (NULL != condAND)||(NULL != condOR) )
    {
        combineCondStr(condAND, condOR, colNameView, condSqlStr);
    }
    pIRDataSet = new QList<QVariant*>();
    if( m_pIRDB->selectRowIRDB(this, condSqlStr, false, NULL, pIRDataSet) )
    {
        delete pIRDataSet;
        pIRDataSet = NULL;
    }
    
    return pIRDataSet;
}

/*
* If all parameters are default, all cid rows in IRDB will return;
*/                       
QList<uint>* IRDBWrapper::getChannelId(const columnMap* const condAND,  
                                       const columnMap* const condOR)
{
    QString condSqlStr;
    QList<uint>* pQListCIDSet = NULL;
    /*this case is wrong logic*/
    if( (NULL != condAND)&&(NULL != condOR) )
    {
        return NULL;
    }

    if( (NULL != condAND)||(NULL != condOR) )
    {
        combineCondStr(condAND, condOR, colNameView, condSqlStr);
    }
    pQListCIDSet = new QList<uint>();
    if( m_pIRDB->selectRowIRDB(this, condSqlStr, true, pQListCIDSet, NULL) )
    {
        delete pQListCIDSet;
        pQListCIDSet = NULL;
    }
    
    return pQListCIDSet;
}    


bool IRDBWrapper::loadDataOfChannelIdCB(QSqlQuery& aIRDataSet, QList<uint>* pQListCIDSet)
{
    while(aIRDataSet.next()) 
    {
        *pQListCIDSet<<aIRDataSet.value(0).toUInt(); 
    }
          
    return true;
}


bool IRDBWrapper::loadDataOfIRDBCB(QSqlQuery& aIRDataSet, QList<QVariant*>* pIRDataSet)
{
    while (aIRDataSet.next()) 
    {
         QVariant* const p = new QVariant[IRDB_CHANNELINFO_COLUMN];

        *pIRDataSet<<p;

        for(int i = 0; i < IRDB_CHANNELINFO_COLUMN; i++)
        {
            *(p+i) = aIRDataSet.value(i);
        }
    }

    return true;
}    

bool IRDBWrapper::getIRTableCB(QSqlQuery& aIRDataSet, QList<QVariant*>* p) 
{
    Q_UNUSED(aIRDataSet);
    Q_UNUSED(p);
    return true;
}

 
void IRDBWrapper::combinePutStr(const columnMap* const RowData,
                                const QString* pArryColName,
                                QString& insSqlStr, 
                                QString& updSqlStr,
                                const logoMap* const logoData,
                                QList<QByteArray>* logoArrayList,
                                int* logoType)
{
    bool bContinue = false;
    columnMap::const_iterator it;
    logoMap::const_iterator itLogoMap;
    QString escStr;
    insSqlStr += "(";
    
    
    if(NULL != RowData)
    {
        bContinue = true;
        it = RowData->begin();
        while(it != RowData->end())
        {
            if(it != RowData->begin())
            {
                insSqlStr += ",";  
            }

            insSqlStr += pArryColName[it.key()];

            ++it;
        }
    }
    
    if(NULL != logoData)
    {
        if(logoType)
        {
            *logoType = 0x00; //
        }
        else
        {
            return;
        }
        itLogoMap = logoData->begin();
        while(itLogoMap != logoData->end())
        {
            if( (false != bContinue)||(itLogoMap != logoData->begin()) )
            {
                     insSqlStr += ",";  
            }
            if(bLogo ==itLogoMap.key())
            {
                *logoType += IRDB_BIG_LOGO;
            }
            if(sLogo ==itLogoMap.key())
            {
                *logoType += IRDB_SMALL_LOGO;
            }
             
            insSqlStr += colNameLogo[itLogoMap.key()];
            ++itLogoMap;        
        }
    }
    
    insSqlStr += ") values(";
    if(NULL != RowData)
    {
        it = RowData->begin();
        while(it != RowData->end())
        {
            if(it != RowData->begin())
            {
                insSqlStr += ",";  
            }
            //here for escape char,
            escStr = it.value();
            escStr.replace('\'', "\'\'");
            
            insSqlStr = insSqlStr + "'"+ escStr+ "'";
       
            ++it;
        }    
    }    


    if(NULL != logoData)
    {
      
        itLogoMap = logoData->begin();
        while(itLogoMap != logoData->end())
        {
            if( (false != bContinue)||(itLogoMap != logoData->begin()) )
            {
                 insSqlStr += ",";  
            }
            
            insSqlStr = insSqlStr + " :"+colNameLogo[itLogoMap.key()];
            *logoArrayList<<itLogoMap.value();

            ++itLogoMap;        
        }
    }    
    
    insSqlStr += " )";
   
    //part two, create update string;
    if(NULL != RowData)
    {    
        it = RowData->begin();
     
        while( it != RowData->end())
        {
            if(it != RowData->begin())
            {
                updSqlStr += ", ";  
            }
            if(pArryColName[it.key()] == "opt")
            {
                updSqlStr = updSqlStr + pArryColName[it.key()] + "=" + "'" + QString::number(IRDB_OPT_TRIGGER) + "'";        
            }
            else
            {
                //here for escape char,
                escStr = it.value();
                escStr.replace('\'', "\'\'");
                updSqlStr = updSqlStr + pArryColName[it.key()] + "=" + "'" + escStr + "'";
            }
            ++it;
        }
    }
    
    if(NULL != logoData)
    {

        itLogoMap = logoData->begin();
        while(itLogoMap != logoData->end())
        {
            if( (false != bContinue)||(itLogoMap != logoData->begin()) )
            {
                 updSqlStr += ",";  
            }

            updSqlStr = updSqlStr + colNameLogo[itLogoMap.key()] + "=:"+colNameLogo[itLogoMap.key()] + " ";
            ++itLogoMap;        
        }
    }    
    
    return;
}



void IRDBWrapper::combineCondStr(const columnMap* const condAND,  
                                 const columnMap* const condOR,   
                                 const QString* pArryColName,
                                 QString& condSqlStr)
{
    bool bDone = false;
    columnMap::const_iterator it;
    QString escStr;
    
    if(NULL != condAND)
    {
        if( true != condAND->empty() )
        {
            it = condAND->begin();
            
            condSqlStr = " where "; 
            
            while( it != condAND->end())
            {
                if(it != condAND->begin())
                {
                    condSqlStr += " AND ";
                }
                escStr = it.value();
                escStr.replace('\'', "\'\'");
                condSqlStr = condSqlStr + pArryColName[it.key()] + "=" + "'" + escStr + "'" ;
                    
                ++it;
            }
            
            bDone = true;               
        }
    }
    
    if(true == bDone)
    {
        return;   
    }

    if(NULL != condOR )
    {
        if( true != condOR->empty() )
        {
            it = condOR->begin();
            condSqlStr = " where ";
            
            while( it != condOR->end())
            {
                if(it != condOR->begin())
                {
                    condSqlStr += " OR ";
                }
                escStr = it.value();
                escStr.replace('\'', "\'\'");
                condSqlStr = condSqlStr + pArryColName[it.key()] + "=" + "'" + it.value()+ "'" ;
                ++it;
             }               
        }       
    }

    return;
}

 
void IRDBWrapper::combineGetStr(const columnMap* const condAND,
                                const columnMap* const condOR,
                                const QString* pArryColName,
                                QString& sltSqlStr)
{
    QString condStr;

    combineCondStr(condAND, condOR, pArryColName, condStr);
    sltSqlStr += condStr;
    
    return;
} 

void IRDBWrapper::combineDeleteStr(const columnMap* const condAND,
                                   const columnMap* const condOR,
                                   const QString* pArryColName,
                                   QString& dltSqlStr)
{
    QString condStr;

    combineCondStr(condAND, condOR, pArryColName, condStr);
    dltSqlStr += condStr;
    
    return;    
}

