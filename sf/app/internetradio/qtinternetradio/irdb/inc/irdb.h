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

#ifndef IRDB_H_
#define IRDB_H_

#include <QSqlDatabase>

typedef  unsigned char IRDBCHAR; 

class QSqlQuery;
class QMutex;
class IRDBWrapper;

class IRDB
{
  
public:
    /*
    * create singleton instance or increase reference count;
    */
    static IRDB* getInstance();  
    
    /*
    * release instance or decrease reference count;
    */
    void releaseInstance();       
    
public:
    /*
    * destory Internet Radio database,
    * It will remove the IR database file from disk.
    * this interface is reserved for future use.
    */
    bool destoryIRDB();
    
    /* 
    * create internet Radio database
    * It will create the IR database file in disk.
    * If createIRDB return false, the instance point of IRDB will be released and set to null;
    */
    bool createIRDB();		    

public:
    /*
    * action is invoked from view of playHistory or search; 
    * INSERT/UPDATE row;
    * manipulation for multiple tables; 
    * for row has a channelid value;
    * First, this function will insert a row in IRBuff table;
    * then INSERT/UPDATE this row to searchRlt and channelHistory table, 
    * next above operations will trigger other actions for manipulation other tables in IRDB. 
    * channelId is mandatory paramter.
    */
    IRDBCHAR chgRowIRDB(const QString& aInsSqlStr, 
                        const QString& aUpdSqlStr, 
                        const uint& channelId, 
                        const QString& aCondSqlStr, 
                        const QList<QByteArray>* apImgList = NULL,
                        const int logoType = 0,
                        bool bPreHandle = false);
    /*
    * action is invoked from view of go to station;     
    * INSERT/UPDATE row;
    * manipulation for multiple tables; 
    * for no channelid 
    * new channelId will be allocated for this row if condStr == NULL;
    * or 
    * A exist channelId will be used for this row, this exist channelId is gotten according to condStr 
    * AND this channelId isNOT iSDS's channelID;
    */ 
    IRDBCHAR chgRowIRDB(QString aInsSqlStr, 
                        QString aUpdSqlStr, 
                        uint* const cidRet = NULL,
                        const QString& condStr = NULL, 
                        const QString& condUserCidStr = NULL,
                        const QList<QByteArray>* apImgList = NULL,
                        const int logoType = 0);
   
    /*
    * this fuction is used while synchronization data with iSDS side;  
    * INSERT/UPDATE row;
    * manipulation for multiple tables; 
    * for row has a channelid value,    
    * update rows that are storged in channelInfo, advertisement, and img tables;
    * about urls for channelId, 
    * we need to call <EIRDB insertUrlInfo(const QStringList&) const> after this function call if there is urls updated. 
    * TBD
    */
    IRDBCHAR updateChannelDetailsIRDB(const QString& aInsSqlStr, 
                                      const QString& aUpdSqlStr, 
                                      const uint& channelId)
    {
        Q_UNUSED(aInsSqlStr); 
        Q_UNUSED(aUpdSqlStr);
        Q_UNUSED(channelId);
        return (IRDBCHAR)0;
    }; 
   
public:
    /*
    * INSERT/UPDATE row;
    * manipulation for single table, songHistory table;     
    * the song name is used as the criterion.
    */	
    IRDBCHAR chgRowSongHistory(const QString& aInsSqlStr, 
                               const QString& aUpdSqlStr, 
                               const QString& aSongName, 
                               const QString& aCondSqlStr = NULL, 
                               const uint& channelId = 0);
      
   /*
   * INSERT/UPDATE row;
   * for change(update/insert) data in favorites,
   * channelId is used as criterion
   */
    IRDBCHAR chgRowFavorites(const QString& aInsSqlStr, 
                             const QString& aUpdSqlStr, 
                             const uint& channelId, 
                             bool& bNewRow);    

    /*
    * TBD
	*/
    IRDBCHAR chgAdvertisement(const QString& aInsSqlStr, 
                              const QString& aUpdSqlStr, 
                              const uint& channelId)
    {
        Q_UNUSED(aInsSqlStr); 
        Q_UNUSED(aUpdSqlStr);
        Q_UNUSED(channelId);	
	    
        return (IRDBCHAR)0;
    }     
    
public:
    /*
    * INSERT/DELETE row, in urlInfo table;
    * step:
    * 1/ first delete all the rows whose channelID equals to input channelId,
    * 2/ insert all the rows belonged to one channelId to urlinfo
    */
    IRDBCHAR resetUrlInfo(const QStringList& aInsSqlList, 
                           const uint& channelId);

    /*
    * only update channelLatestplaytime in channelHistory table for recently play 
    */
    IRDBCHAR updRowChannelHistory(const uint& channelId);

    /*
    * only update imgUrl, imgLocalFile in img table; 
    */
    IRDBCHAR updRowImg(const uint& channelId,
                       const QString& updSqlStr,
                       QList<QByteArray>* apImgList = NULL,
                       const int logotype = 0);    

public:
    /*
    *  handle request from search view;
    *  aSqlList, sql string list, this list is provided by wrapper level;
    *  bBegin,   if bBeging is true,it will invoke the operation of open db's connection.
    */
    IRDBCHAR handleRstFromSrhView(const uint& channelId, const QStringList& aSqlList, bool bBegin=true);
   
    /*
    * call this function after call handleRstFromSrhView;
    */
    void closeIRDBConnection();
    /*
    * DELETE rows in
    * channelHistory, searchRlt, songHistory, favorites, advertisement;
    * rows in img, channelInfo and urlInfo can't be removed directly.
    */
    IRDBCHAR deleteRow(const QString& aDltSqlStr);
    
    /*
    * select rows from IRDB
    * aBegin, aBegin is 0, will invoke the action of open db connection;
    * aBegin == aEnd, will invoke the action of close db connection;
    */
    IRDBCHAR selectRow(IRDBWrapper* const apWrapper, 
                       const QString& aSltSqlStr,
                       QList<QVariant*>* pDataSet,
                       int aBegin = 0,
                       int aEnd = 0);    
 
public:
    /*
    * SELECT * from IR_VIEW_ALL; 
    * param is used as condition string;
    */
    IRDBCHAR selectRowIRDB(IRDBWrapper* const apWrapper, 
                           const QString& aCondStr, 
                           bool bIsSrhCid,
                           QList<uint>* pQListCIDSet, 
                           QList<QVariant*>* pQLIstIRDataSet);
private:
    /*
    * default constructor
    */
    IRDB();

    /*
    * forbid copy constructor
    */
    IRDB(IRDB&);         					

    /*
    * destructor
    */
    ~IRDB();

private:        
    /*
    * here "=" is forbidded
    */  
    const IRDB& operator=(const IRDB&);

private:
    /*
    * create tables and relations in IRDB; 
    */	
    bool createIRTable();
    
    /*
    * open IRDB connection; 
    */
    bool openIRDBConnection();
    
    /*
    * search channelid from channelInfo table; 
    */
    IRDBCHAR isChannelIdExisted(uint channelId);    
    
private:
    //IRDB Instance
    QSqlDatabase m_sqlDB;
	
    // reference count	
    int m_RefCnt;  
    
    // Mutex locker
    static QMutex mMutex;
    
    // The static instance  
    static IRDB* mpIRDBInstance; 
    
    //for data batch handle  
    QSqlQuery* iIRQuery2;
   
};    


#endif /* IRDB_H_ */








