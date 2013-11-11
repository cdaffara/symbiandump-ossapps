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
#ifndef IRQISDSDATASTRUCTURE_H
#define IRQISDSDATASTRUCTURE_H

#include "irqcommonexport.h"
#include <QList>
#include <QString>

#define DEBUG_ISDS_DATA 0

class QDataStream;

class IRQBrowseCategoryItem
{
public:  
  QString catName;
  int     size;
};


class IRQChannelItem
{
public:
   QString channelName;
   QString shortDescription;
   QString imageURL;
   //added for cache search
   uint     channelID;
};

class IRQChannelServerURL
{
public:
    void internalize(QDataStream &aStream);
    void externalize(QDataStream &aStream) const;
    
public:
   QString serverName;
   QString url;
   int     bitrate;
};

class IRQCOMMMON_DLL_EXPORT IRQPreset
{
public:
  enum IRQPresetType  
    {   
      EUser,
      EIsds
    };
  
   
   IRQPreset();
   ~IRQPreset();  
   IRQPreset& operator = (const IRQPreset& aOther); 
   
   void internalize(QDataStream &aStream);
   void externalize(QDataStream &aStream) const;
   
   /* get the bitrates of the preset.
    */
   void getAvailableBitrates(QList<int>& aBitrates) const;
   /* get the url for the specify bitrate
    * the caller should delete the QList */
   QList<QString> * getURLsForBitrate(int aBitrate) const;
   /* the following functions may be changed in future */
   int getChannelUrlAt(int aIndex, QString& aURL) const;   
   int getChannelBitrate(unsigned int aIndex, unsigned int &aBitrate) const;    
   
   /* sort the urlArray with bitrates by ascending. for 
    * there are not many items, here we use bubble sorting.
    */
   void sortURLArray();
   /* check wether the internal list is sorted 
    */
   bool isSorted() const;
   /* append a channel server into a internal list 
    */
   void insertChannelServer(const IRQChannelServerURL& aURL);
   /* clear the internal list 
    */
   void clearChannelServerList();   
   
   int getChannelURLCount() const;
   
   bool setUrlBitrate(const unsigned int aUrlIndex, const unsigned int aBitrate);
  
   int uniqID; //unique id generated for the preset saved at favourite
   
   int type; //0 for user defined 1 for isds type.
   uint presetId;//isds generated preset id
   QString name; //preset name   
   QString nickName;     
   QString languageCode;//preset language code
   QString languageName; // preset language
   QString countryCode;//preset country of origin code     
   QString countryName;//preset country of origin      
   QString lastModified;//preset last modified     
   QString musicStoreStatus;//musicStoreEnabled value
   QByteArray logoData;//Logo Data
   QString description;//channel description       
   QString shortDesc;//channel short desription    
   QString genreName;//channel genre name 
   QString genreId;//channel genre id     

   //in our spec but not in current code 
   QString advertisementUrl; 
   QString advertisementInUse; 
   QString imgUrl;        
   
 
private:
  /* for one preset, there may be many servers*/
  QList<IRQChannelServerURL> urlArray;
  bool iIsSorted;
  int  iChannelUrlCount; //no of channel urls
  
private:
    friend class IRQUtility;
 
};

bool bitrateLessThan(const IRQChannelServerURL &url1, const IRQChannelServerURL &url2);

#endif /* IRQISDSDATASTRUCTURE_H */
