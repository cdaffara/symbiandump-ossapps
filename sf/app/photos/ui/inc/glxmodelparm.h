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
* Description:   ?Description
*
*/



#ifndef GLXMODELPARM_H
#define GLXMODELPARM_H

#include<glxfiltergeneraldefs.h>
#include<glxuistd.h>
#include<glxmodelroles.h>

class GlxModelParm
{
public:
	GlxModelParm (qint32 collectionId = 0, qint32 depth = 0 , TGlxFilterItemType filterType = EGlxFilterItemAll , GlxContextMode contextMode = GlxContextPtGrid ,QString uri = NULL) 
	    : mCollectionId(collectionId) ,
	      mDepth(depth) , 
	      mFilterType( filterType ),
	      mContextMode( contextMode ),
          mUri(uri),
          mFetcherFiletrType(EGlxFetcherFilterNone)
	{
	
	}
	
	void setCollection (qint32 collectionId = 0) { mCollectionId = collectionId ; }
	qint32 collection() { return mCollectionId; }
	void setDepth (qint32 depth = 0) { mDepth = depth ; }
	qint32 depth() { return mDepth; }
	QString path() { return mUri; }
	void setPath(QString uri = NULL) {  mUri=uri; }
	TGlxFilterItemType filterType() { return mFilterType; }
	void setFilterType(TGlxFilterItemType filterType) { mFilterType = filterType; }
	void setContextMode( GlxContextMode contextMode ) { mContextMode = contextMode ; }
	GlxContextMode contextMode() { return mContextMode ; }
	void setFetcherFiterType(GlxFetcherFilterType filterType ){mFetcherFiletrType = filterType;}
	GlxFetcherFilterType fetcherFiterType() { return mFetcherFiletrType;}
	
private:
	qint32 mCollectionId;
	qint32 mDepth;
	TGlxFilterItemType mFilterType;
	GlxContextMode mContextMode;
	QString mUri;
	GlxFetcherFilterType mFetcherFiletrType;
};


#endif /* GLXMODELPARM_H_ */
