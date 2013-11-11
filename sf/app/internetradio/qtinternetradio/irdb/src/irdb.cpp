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

#include <QtGlobal> 
#include <QString>
#include <QStringList>
#include <QSqlQuery>
#include <QFile>
#include <QMutex>
#include <QVariant>
#include <QtDebug>
#include <QByteArray>
#include "irdbwrapper.h"
#include "irsqlstr.h"
#include "irdb.h"

#define IRDB_ERR_NONE      (0x00)
#define IRDB_ERR_INSERT    (0x01)
#define IRDB_ERR_UPDATE    (0x02)
#define IRDB_ERR_SELECT    (0x03)
#define IRDB_ERR_DELETE    (0x04)
#define IRDB_ERR_OPEN      (0x05)
#define IRDB_ERR_CHANGE    (0x06)
#define IRDB_ERR_ILLEGAL   (0x07)
#define IRDB_ERR_WRAPPER   (0x08)
#define IRDB_CID_EXISTED   (0x10)
#define IRDB_CID_UNEXISTED (0x11)
#define IRDB_CID_USER_DEFINE (0xF0000000)

#define INS_CHANNELINFO_FRM_SRHVIEW (0x00) 
#define INS_IMG_FRM_SRHVIEW         (0x01)
#define INS_SEARCHRLT_FRM_SRHVIEW   (0x02)
#define UPD_CHANNELINFO_FRM_SRHVIEW (0x03) 
#define UPD_IMG_FRM_SRHVIEW         (0x04)

IRDB* IRDB::mpIRDBInstance = NULL;
QMutex IRDB::mMutex;

IRDB::IRDB(): m_RefCnt(0),iIRQuery2(NULL)
{
}

IRDB::~IRDB()
{
}

IRDB* IRDB::getInstance()
{
    mMutex.lock();
    bool existed = true;
    
    if(NULL == mpIRDBInstance)
    {
        mpIRDBInstance = new IRDB();
        existed = false;
    }
    
    ++mpIRDBInstance->m_RefCnt;
    mMutex.unlock();
    
    if(false == existed)
    {
        if(!mpIRDBInstance->createIRDB())
        {
            //remove IR database;
            QFile::remove(IRDBFile);
            mpIRDBInstance->releaseInstance();	
        }
    }
    return mpIRDBInstance;
}

void IRDB::releaseInstance()
{
    mMutex.lock();
    if ((--m_RefCnt) == 0)
    {
        delete this;
        mpIRDBInstance = NULL;
    }
    mMutex.unlock();
}

bool IRDB::createIRDB()
{
    bool ret = true;

    m_sqlDB = QSqlDatabase::addDatabase("QSQLITE");
    m_sqlDB.setDatabaseName(IRDBName);		
		
    if( !QFile::exists(IRDBFile) )			
    {
        mMutex.lock();
        if(!m_sqlDB.isOpen())
        {
            m_sqlDB.open();
            mMutex.unlock();
		      
            ret = createIRTable();
            m_sqlDB.close();    	
        }
        else
        {
            mMutex.unlock();
            ret = false;
        }
    }   

    return ret;
}

bool IRDB::destoryIRDB()  
{ 
    bool ret = true;
    mMutex.lock();
    if(!m_sqlDB.isOpen())
    {
        QFile::remove (IRDBFile);			
        mMutex.unlock();
    }
    else
    {
        mMutex.unlock();
        ret = false;
    }
    
    return ret;
}

//if the IR database file in disk exists, we don't need to call this function;
bool IRDB::createIRTable()
{
    bool Rlt = true;	
    
    QStringList sqlList;
    QSqlQuery sqlQuery(m_sqlDB);
 
    sqlList<<CREATE_TABEL_IRBUFF<<CREATE_TABEL_CHANNELHISTORY<<CREATE_TABEL_SEARCHRLT<<CREATE_TABEL_CHANNELINFO \
    <<CREATE_TABEL_URLINFO<<CREATE_TABEL_IMG<<CREATE_TABEL_ADVERTISEMENT<<CREATE_TABEL_SONGHISTORY<<CREATE_TABEL_FAVORITES \
\
    <<TRI_INSERT_IRBUFF<<TRI_UPDATE_IRBUFF \
    <<TRI_INSERT_SEARCHRLT<<TRI_DELETE_SEARCHRLT \
    <<TRI_UPDATE_CHANNELINFO<<TRI_DELETE_CHANNELINFO \
    <<TRI_INSERT_CHANNELHISTORY<<TRI_DELETE_CHANNELHISTORY \
    <<TRI_INSERT_ADVERTISEMENT<<TRI_UPDATE_ADVERTISEMENT \
    <<TRI_UPDATE_IMG \
\
    <<TRI_INSERT_SONGHISTORY<<TRI_UPDATE_SONGHISTORY<<TRI_DELETE_SONGHISTORY \
    <<TRI_INSERT_FAVORITES<<TRI_UPDATE_FAVORITES<<TRI_DELETE_FAVORITES \
\
    <<IRVIEW_CHANNELINFO1<<IRVIEW_CHANNELINFO<<IRVIEW_CHANNELHISTORY<<IR_VIEW_SRH_USERCID \
    <<IRVIEW_FAVORITES<<IRVIEW_SEARCHRLT<<IRVIEW_SONGHISTORY<<IR_VIEW_ALL ;


    for(int i = 0; i<sqlList.size();i++)
    {
        Rlt = sqlQuery.exec(sqlList.at(i));

        if(Rlt == false)
        {
        	  break;
        }
    }
		
    sqlQuery.clear();
		
    //remove string in sqlList;
    for(int i = 0; i<sqlList.size();i++)
    {
        sqlList.removeFirst();
    }
		
    return Rlt;
}


bool IRDB::openIRDBConnection()
{
    bool ret = true;
    mMutex.lock();
    
    if(m_sqlDB.isOpen())
    {
        ret = false;
    }
    else
    { 
        ret = m_sqlDB.open();
    }
	  
    mMutex.unlock();
    return ret;	
}


IRDBCHAR IRDB::chgRowIRDB(QString aInsSqlStr, 
                          QString aUpdSqlStr, 
                          uint* const retCid,
                          const QString& aCondSqlStr, 
                          const QString& condUserCidStr, 
                          const QList<QByteArray>* apImgList,
                          const int logoType) 
{
    IRDBCHAR ret = IRDB_ERR_NONE;
    //channelId value 0 don't be used in CIRDB;
    uint channelId = 0;                     
    //sql string for search cid according to nickname and url; 
    QString strSrhCid = IRDBSrhUserCid + condUserCidStr;
    QString strSrhDataSet = IRDBSrhCIdFrmChannelInfo + aCondSqlStr;
    
    //initialize the channelId for application level;
    if(retCid)
    {
        *retCid = 0;
    }
    
    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;	
    }

    QSqlQuery iIRQuery(m_sqlDB);
      
    if(NULL != condUserCidStr)
    {
        if(false == iIRQuery.exec(strSrhCid))
        {
            iIRQuery.clear();
            m_sqlDB.close();
            ret = IRDB_ERR_SELECT;
        } 
        else
        {
            iIRQuery.next();
            channelId = iIRQuery.value(0).toInt();
        }
    }
		
    if(ret != IRDB_ERR_NONE)
    {
        return ret;
    }

    /*for below case, because it is a channel inputted by user.
    * Although the channelId can be gotten according to the condstr, 
    * if the channelId is defined by iSDS, still new user-defined channelId will be allocated.  
    */
    if(channelId < IRDB_CID_USER_DEFINE)
    {
    	channelId = 0;
    }
 
    if(0 == channelId )
    {
        //new channelId should be allocated.
        if( false == iIRQuery.exec(IRDBSrhCIdMaxFrmChannelInfo) )
        {	
            iIRQuery.clear();
            m_sqlDB.close();
            ret = IRDB_ERR_SELECT;        	
        }   
        
        if(ret != IRDB_ERR_NONE)
        {
            return ret;
        }
		      
        iIRQuery.next();
      
        if(iIRQuery.value(0).toInt()< IRDB_CID_USER_DEFINE )
        {
            channelId = (uint)IRDB_CID_USER_DEFINE;
        }
        else
        {
            channelId = (uint)iIRQuery.value(0).toInt() + 1;
        }
        
        //if a userdefined channelid is created, the value will be returned to application level;
        if(retCid)
        {
            *retCid = channelId;
        }
	}
    else
	{    
	    /*
	    * for case: although channelId can be gotten, but the updSqlStr is invalid, 
	    * operation should be stopped
	    */
        if(false == iIRQuery.exec(strSrhDataSet))
        {
            iIRQuery.clear();
            m_sqlDB.close();
            ret = IRDB_ERR_SELECT;
        } 
        else
        {
            iIRQuery.next();
            if(0 == iIRQuery.value(0).toInt())
            {
                iIRQuery.clear();
                m_sqlDB.close();
                ret = IRDB_ERR_UPDATE;
            }
        }
    }
  
    if(IRDB_ERR_NONE == ret )
    {
        iIRQuery.clear();

        aInsSqlStr.insert(aInsSqlStr.indexOf(')'), QString(" ,channelId"));
        aInsSqlStr.insert(aInsSqlStr.lastIndexOf(')'),QString(" ,")+QString::number(channelId));
    
        /* 
        * Since process will go ahead to call 
        * EIRDB IRDB::chgRowIRDB(const QString& aInsSqlStr, const QString& aUpdSqlStr, uint channelId, bool bPreHandle),
        * for the synchoronization concern, the m_sqlDB can't be closed until all the process is compeleted;    
        * m_sqlDB.close();
        */
         ret = chgRowIRDB(aInsSqlStr, aUpdSqlStr, channelId, aCondSqlStr, apImgList, logoType, true);
    }
/*    else
    {
        iIRQuery.clear();
        m_sqlDB.close();
    }
*/    
    return ret;
}
    


// if the action invoked from 'go to station view', don't use this function
IRDBCHAR IRDB::chgRowIRDB(const QString& aInsSqlStr, 
                          const QString& aUpdSqlStr, 
                          const uint& channelId, 
                          const QString& aCondSqlStr, 
                          const QList<QByteArray>* apImgList,
                          const int logoType,
                          bool bPreHandle)
{
    IRDBCHAR ret = IRDB_ERR_NONE;
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelInfo + QString::number(channelId) +";";
    QString updSqlStr;
    
    /*
    * if the function call isn't invoked from 
    * EIRDB IRDB::chgRowIRDB(QString aInsSqlStr, QString aUpdSqlStr, const QString& aCondStr),
    * db connection should be opened again;
    */  
    if(false == bPreHandle)
    {
        if(false == openIRDBConnection())
        {
            return IRDB_ERR_OPEN;	
        }
    }
    
    QSqlQuery iIRQuery(m_sqlDB);  

    /*If channelId isn't 0, then two cases should be checked.
	  * First, if channelId  exists in the channelInfo, update operation will be excuted.
	  * Second, if channelId doesn't exists in channelInfo table, insert operation will be excuted.
    */
    if(false == iIRQuery.exec(strIsCIDExist))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        return IRDB_ERR_SELECT;
    } 
		  
    iIRQuery.next();    
    
    /*if channelID existed, we need to get the preset data*/
    if (0 != iIRQuery.value(0).toInt())
    {
        /*trigger the action, 
        * All the data recorded in CiRDB, its channelID equal to here ,
        * will be composed to one row in IRBuff table; 
        */ 
        if(false == iIRQuery.exec(IRDBInsertIRBuff + QString::number(channelId)+",1, 2)"))
        {
            ret = IRDB_ERR_INSERT;            
        } 
        else
        {
            if(aCondSqlStr.isEmpty())
            {
                updSqlStr = aUpdSqlStr  + " where channelId = " + QString::number(channelId);
            }
            else
            {
                updSqlStr += aCondSqlStr;
            }
            if(NULL == apImgList)
            {
                if(false == iIRQuery.exec(aUpdSqlStr))
		        {
		            ret = IRDB_ERR_UPDATE;        	
		        }
            }
            else
            {            
                iIRQuery.prepare(aUpdSqlStr);
                //when there are both big and small logo, first insert blogo and next insert slogo is required;
                if(IRDB_BOTH_LOGO == logoType)
                {
                    iIRQuery.bindValue(strBLogo, apImgList->at(bLogo)); 
                    iIRQuery.bindValue(strSLogo, apImgList->at(sLogo));                
                }
                else
                {
                    if( IRDB_BIG_LOGO == logoType&IRDB_BOTH_LOGO )
                    {
                        iIRQuery.bindValue(strBLogo, apImgList->at(0));                
                    }
                    if(IRDB_SMALL_LOGO == logoType&IRDB_BOTH_LOGO)
                    {
                        iIRQuery.bindValue(strSLogo, apImgList->at(0));
                    }
                
                }
                
                if(false == iIRQuery.exec())
                {
                    ret = IRDB_ERR_UPDATE;
                }
            }
		    
		}	
    }
    else /*since there is not uid, we insert directly*/
    {   
        if(NULL == apImgList)
        {
            if(false == iIRQuery.exec(aInsSqlStr))
            {
                ret = IRDB_ERR_INSERT;
            }
        }
        else
        {
            iIRQuery.prepare(aInsSqlStr);
            //when there are both big and small logo, first insert blogo and next insert slogo is required;
            if(IRDB_BOTH_LOGO == logoType)
            {
                iIRQuery.bindValue(strBLogo, apImgList->at(bLogo)); 
                iIRQuery.bindValue(strSLogo, apImgList->at(sLogo));                
            }
            else
            {
                if( IRDB_BIG_LOGO == logoType&IRDB_BOTH_LOGO )
                {
                    iIRQuery.bindValue(strBLogo, apImgList->at(0));                
                }
                if(IRDB_SMALL_LOGO == logoType&IRDB_BOTH_LOGO)
                {
                    iIRQuery.bindValue(strSLogo, apImgList->at(0));
                }
            
            }
            
            if(false == iIRQuery.exec())
            {
                ret = IRDB_ERR_UPDATE;
            }        
        }
    }
 
    iIRQuery.clear();
    m_sqlDB.close();

    return ret;
}

IRDBCHAR IRDB::chgRowSongHistory(const QString& aInsSqlStr, 
                                 const QString& aUpdSqlStr, 
                                 const QString& aCondSqlStr, 
                                 const QString& aSongName, 
                                 const uint& channelId)
{
    QString strSltRecordFromSongHistory = IRDBSrhRecordCntFrmSongHistory + aCondSqlStr;
    QString updStr;
    bool bDone = false;
    IRDBCHAR ret = IRDB_ERR_NONE;
    
    
    if( (aSongName.isEmpty()) || (0 == channelId) )
    {
        return IRDB_ERR_ILLEGAL;
    }

    if(IRDB_CID_EXISTED != isChannelIdExisted(channelId))
    {
        return IRDB_ERR_ILLEGAL;
    }    
    
    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;   
    }
    
    QSqlQuery iIRQuery(m_sqlDB);  
    
    //if update string is true, first excute update string;
    if(false == aCondSqlStr.isEmpty())
    {
        if(false == iIRQuery.exec(strSltRecordFromSongHistory))
        {
            iIRQuery.clear();
            m_sqlDB.close();
            return IRDB_ERR_SELECT;
        } 
  
        iIRQuery.next();  
        if (0 != iIRQuery.value(0).toInt())
        {
            /*
            * if row can be selected from songHistory Table,
            * it is a stirng which is to be updated. 
            */
            bDone = true;
            updStr = aUpdSqlStr + aCondSqlStr;
            if( false == iIRQuery.exec(updStr) )
            {
                ret = IRDB_ERR_INSERT;
            }          
        }
    }
    
    if(false == bDone)
    {
        if(false == iIRQuery.exec(aInsSqlStr))
        {
            ret = IRDB_ERR_UPDATE;
        }     
    }

    iIRQuery.clear();
    m_sqlDB.close();
    
    return ret;
    
}

IRDBCHAR IRDB::handleRstFromSrhView(const uint& channelId, const QStringList& aSqlList, bool bBegin)
{
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelInfo + QString::number(channelId);
    IRDBCHAR ret = IRDB_ERR_NONE;
    
    if(0 == channelId)
    {
        return IRDB_CID_UNEXISTED;
    }    
    if(true == bBegin)
    {
        if(false == openIRDBConnection())
        {
            return IRDB_ERR_OPEN;   
        }
        iIRQuery2 = new QSqlQuery(m_sqlDB);
    }
    
  
    
    //iIRQuery.clear();
    if(false == iIRQuery2->exec(strIsCIDExist))
    {
            ret = IRDB_ERR_SELECT; 
    }
    else
    {
        iIRQuery2->next();  
        if(0 != iIRQuery2->value(0).toInt())
        {
            ret = IRDB_CID_EXISTED;
        }
    } 
    
    if(IRDB_CID_EXISTED == ret)
    {
        ret = IRDB_ERR_NONE;
        if( false == iIRQuery2->exec(aSqlList.value(INS_SEARCHRLT_FRM_SRHVIEW)) )
        {
            ret = IRDB_ERR_INSERT;
        }
        //the update string should be excute;
        if(false == iIRQuery2->exec(aSqlList.value(UPD_CHANNELINFO_FRM_SRHVIEW)))
        {
            ret = IRDB_ERR_UPDATE;
        }
        if(false == iIRQuery2->exec(aSqlList.value(UPD_IMG_FRM_SRHVIEW)))
        {
            ret = IRDB_ERR_UPDATE;
        }
    }
    else if( IRDB_ERR_NONE == ret)
    {
        if( false == iIRQuery2->exec(aSqlList.value(INS_CHANNELINFO_FRM_SRHVIEW)) )
        {
            ret = IRDB_ERR_INSERT;
        }
        else
        {
            if(false == iIRQuery2->exec(aSqlList.value(INS_IMG_FRM_SRHVIEW)))
            {
                ret = IRDB_ERR_INSERT;
            }else
            {
                if ( false == iIRQuery2->exec(aSqlList.value(INS_SEARCHRLT_FRM_SRHVIEW)) )
                {
                    ret = IRDB_ERR_INSERT;
                }
            }
            
        }
    }

    return ret ;      
    
}


void IRDB::closeIRDBConnection()
{
    if( NULL != iIRQuery2)
    {
        iIRQuery2->clear();
        delete iIRQuery2;
        iIRQuery2 = NULL;	
    }
    m_sqlDB.close();
}

IRDBCHAR IRDB::isChannelIdExisted(uint channelId)
{
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelInfo + QString::number(channelId);
    IRDBCHAR ret = IRDB_ERR_NONE;
    
    if(0 == channelId)
    {
        return IRDB_CID_UNEXISTED;
    }    
  
    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;   
    }

    QSqlQuery iIRQuery(m_sqlDB);  
    
    //iIRQuery.clear();
    if(false == iIRQuery.exec(strIsCIDExist))
    {
            ret = IRDB_ERR_SELECT; 
    }
    else
    {
        iIRQuery.next();  
        if(0 != iIRQuery.value(0).toInt())
        {
            ret = IRDB_CID_EXISTED;
        }
    }

    iIRQuery.clear();
    m_sqlDB.close();

    return ret ;    
}

IRDBCHAR IRDB::chgRowFavorites(const QString& aInsSqlStr, const QString& aUpdSqlStr, const uint& channelId, bool& bNewRow)
{
    IRDBCHAR ret = IRDB_ERR_NONE;
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelInfo + QString::number(channelId) +";";
    QString strIsCIDFavorites = IRDBSrhCIdCntFrmFavorites + QString::number(channelId) +";";
    QString updSqlStr = aUpdSqlStr + "where channelid = " + QString::number(channelId);
    
    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;	
    }

    QSqlQuery iIRQuery(m_sqlDB);  
    
    if(false == iIRQuery.exec(strIsCIDExist))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        ret = IRDB_ERR_SELECT;
    } 
    
    if(ret != IRDB_ERR_NONE)
    {
        return ret;
    }
		  
    iIRQuery.next();  
   
    /*if channelID existed in channelInfo*/
    if (0 != iIRQuery.value(0).toInt())
    {
        if(false != iIRQuery.exec(strIsCIDFavorites))
        {
        	//if this channelId is in Favorties, this is a row need to update;
            iIRQuery.next();  
            if(0 != iIRQuery.value(0).toInt())
            {
        	      bNewRow = false;	
        	    
                if( false == iIRQuery.exec(updSqlStr) )
                {
                    ret = IRDB_ERR_UPDATE;
                }        		
            }
            else //this is a new row which need to be insert favorites;
            {
                bNewRow = true;	
                
                if( false == iIRQuery.exec(aInsSqlStr) )
                {
                    ret = IRDB_ERR_INSERT;
                }            		
            }
        }
        else
        {
            ret = IRDB_ERR_SELECT;
        }	
    }
    else /*channelId don't be recorded in channelinfo, illegal data*/
    {
        ret = IRDB_ERR_ILLEGAL;
    }
     
    iIRQuery.clear();
    m_sqlDB.close();
   
    return ret ;
}


IRDBCHAR IRDB::deleteRow(const QString& aDltSqlStr)
{
    IRDBCHAR ret = IRDB_ERR_NONE;
    
    /*rows in channelInfo and img can't be removed directly.*/    
    if( 0 == QString::compare("channelInfo", aDltSqlStr, Qt::CaseInsensitive) )
    {
    	return IRDB_ERR_DELETE;
    }
    
    if( 0 == QString::compare("img", aDltSqlStr, Qt::CaseInsensitive) )
    {
    	return IRDB_ERR_DELETE; 
    }
    
    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;	
    }

    QSqlQuery iIRQuery(m_sqlDB);  
    if(false == iIRQuery.exec(aDltSqlStr))
    {
        ret = IRDB_ERR_DELETE;
    }
    
    iIRQuery.clear();
    m_sqlDB.close();
    
    return ret;
}

IRDBCHAR IRDB::selectRowIRDB(IRDBWrapper* const apWrapper, const QString& aCondStr, bool bIsSrhCid, 
                             QList<uint>* pQListCIDSet, QList<QVariant*>* pIRDataSet)
{
    QString strSrhCid;
    IRDBCHAR ret = IRDB_ERR_NONE;   

    if(true == bIsSrhCid)
    {
        strSrhCid = IRDBSrhCIdFrmView + aCondStr;  
    }
    else
    {
        strSrhCid = IRDBSrhAllFrmView + aCondStr;
    } 
    
    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;   
    }
 
    QSqlQuery iIRQuery(m_sqlDB);  


    if(false == iIRQuery.exec(strSrhCid))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        ret = IRDB_ERR_SELECT;      
    }

    if(ret != IRDB_ERR_NONE)
    {
        return ret;
    }  
        
    if(!apWrapper)
    {
        iIRQuery.clear();
        m_sqlDB.close();
        ret = IRDB_ERR_SELECT;      
    }

    if(true == bIsSrhCid)
    {
        if(false == apWrapper->loadDataOfChannelIdCB(iIRQuery, pQListCIDSet))
        {
            ret = IRDB_ERR_WRAPPER;
        } 
    }
    else
    {
        if(false == apWrapper->loadDataOfIRDBCB(iIRQuery, pIRDataSet))
        {
            ret = IRDB_ERR_WRAPPER;
        } 
    } 

    iIRQuery.clear();
    m_sqlDB.close();
 
    return ret; 
}

IRDBCHAR IRDB::selectRow(IRDBWrapper* const apWrapper, 
                         const QString& aCondStr, 
                         QList<QVariant*>* pDataSet,
                         int aBegin,
                         int aEnd)
{
    IRDBCHAR ret = IRDB_ERR_NONE;   
    
    if(!apWrapper)
    {
        ret = IRDB_ERR_SELECT;      
    }
    
    if(0 == aBegin)
    {    
        if(false == openIRDBConnection())
        {
            return IRDB_ERR_OPEN;   
        }
    }
 
    QSqlQuery iIRQuery(m_sqlDB);  


    if(false == iIRQuery.exec(aCondStr))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        ret = IRDB_ERR_SELECT;      
    }

    if(ret != IRDB_ERR_NONE)
    {
        return ret;
    }  
        
    if(false == apWrapper->getIRTableCB(iIRQuery, pDataSet))
    {
            ret = IRDB_ERR_WRAPPER;
    } 
        
    

    if(aBegin == aEnd)
    {
        iIRQuery.clear();
        m_sqlDB.close();
    }
 
    return ret; 
}

/*
* INSERT/DELETE row, in urlInfo table;
* step:
* 1/ first delete all the rows whose channelID equals to input channelId,
* 2/ insert all the rows belonged to one channelId to urlinfo
*/
IRDBCHAR IRDB::resetUrlInfo(const QStringList& aInsSqlList, const uint& channelId)
{
    QString strDltRow = IRDBDltRowFrmUrlInfoByCId + QString::number(channelId) +";";
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelInfo + QString::number(channelId) +";";
    	
    IRDBCHAR ret = IRDB_ERR_NONE;	

    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;	
    }
 
    QSqlQuery iIRQuery(m_sqlDB);  

    if(false == iIRQuery.exec(strIsCIDExist))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        return IRDB_ERR_SELECT;    	
    }
    
    iIRQuery.next();  
   
    /*if channelID existed in channelInfo*/
    if (0 != iIRQuery.value(0).toInt())
    {
        if(false == iIRQuery.exec(strDltRow))
        {
            ret = IRDB_ERR_DELETE;    	
        }
        else
        {
            for(int i = 0; i<aInsSqlList.size();i++)
            {
                if(false == iIRQuery.exec(aInsSqlList.at(i)) )
                {
                    ret = IRDB_ERR_INSERT;
                    break;	
                }
            }  		    	
         }    	
    }
    else
    {
        ret = IRDB_ERR_ILLEGAL;	
    }
    
    iIRQuery.clear();
    m_sqlDB.close();
           
    return ret;	
}

/*
* only update imgUrl, imgLocalFile in img table; 
*/
IRDBCHAR IRDB::updRowImg(const uint& channelId, 
                         const QString& updSqlStr, 
                         QList<QByteArray>* apImgList, 
                         const int logoType)
{
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelInfo + QString::number(channelId) +";";    
    IRDBCHAR ret = IRDB_ERR_NONE;   

    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;   
    }
 
    QSqlQuery iIRQuery(m_sqlDB);  

    if(false == iIRQuery.exec(strIsCIDExist))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        return IRDB_ERR_SELECT;     
    }
    
    iIRQuery.next();  

    /*if channelID existed in channelInfo*/
    if (0 != iIRQuery.value(0).toInt())
    {
        if(NULL == apImgList)
        {
            if(false == iIRQuery.exec(updSqlStr))
            {
                ret = IRDB_ERR_UPDATE;          
            }
        }
        else
        {            
            iIRQuery.prepare(updSqlStr);
            //when there are both big and small logo, first insert blogo and next insert slogo is required;
            if(IRDB_BOTH_LOGO == logoType)
            {
                iIRQuery.bindValue(strBLogo, apImgList->at(bLogo)); 
                iIRQuery.bindValue(strSLogo, apImgList->at(sLogo));                
            }
            else
            {
                if( IRDB_BIG_LOGO == logoType&IRDB_BOTH_LOGO )
                {
                    iIRQuery.bindValue(strBLogo, apImgList->at(0));                
                }
                if(IRDB_SMALL_LOGO == logoType&IRDB_BOTH_LOGO)
                {
                    iIRQuery.bindValue(strSLogo, apImgList->at(0));
                }
            
            }
            if(false == iIRQuery.exec())
            {
                ret = IRDB_ERR_UPDATE;
            }
        }    
    }
    else
    {
        ret = IRDB_ERR_ILLEGAL; 
    }
    
    iIRQuery.clear();
    m_sqlDB.close();
           
    return ret; 
    
}

IRDBCHAR IRDB::updRowChannelHistory(const uint& aChannelId)
{
    QString strIsCIDExist = IRDBSrhCIdCntFrmChannelHistory + QString::number(aChannelId) + ";";
    QString updStr = "update channelHistory set channelId = " + QString::number(aChannelId) + "where channelId = " + QString::number(aChannelId) +";";
    IRDBCHAR ret = IRDB_ERR_NONE;	

    if(false == openIRDBConnection())
    {
        return IRDB_ERR_OPEN;	
    }
 
    //if channelId isn't in channelHistory, return;
    QSqlQuery iIRQuery(m_sqlDB);  

    if(false == iIRQuery.exec(strIsCIDExist))
    {
        iIRQuery.clear();
        m_sqlDB.close();
        return IRDB_ERR_SELECT;    	
    }
    
    iIRQuery.next();  
   
    /*if channelID existed in channelHistory*/
    if (0 != iIRQuery.value(0).toInt())
    {
        if(false == iIRQuery.exec(updStr))
        {
            ret = IRDB_ERR_DELETE;    	
        }    	
    }
    else
    {
        ret = IRDB_ERR_ILLEGAL;
    }

    return ret;
}


