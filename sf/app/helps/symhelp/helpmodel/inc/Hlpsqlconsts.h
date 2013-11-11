// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __HLPSQLCONSTS_H__
#define __HLPSQLCONSTS_H__

#include <e32std.h>


_LIT(KSQLTopicIdColumn,					"TopicId");
_LIT(KSQLIndexIdColumn,					"IndexId");
_LIT(KSQLTopicTitleColumn,				"TopicTitle");
_LIT(KSQLTopicTextColumn,				"TopicText");
_LIT(KSQLTopicMarkupColumn,				"TopicMarkup");
_LIT(KSQLCategoryColumn,				"Category");
_LIT(KSQLSynonymColumn,					"Synonym");
_LIT(KSQLIndexColumn,					"Index");
_LIT(KSQLContextColumn,					"Context");
_LIT(KSQLCategoryUidColumn,				"CategoryUID");
_LIT(KSQLImageCountColumn,				"ImageCount");
_LIT(KSQLImage0Column,					"Image0");
_LIT(KSQLImage1Column,					"Image1");
_LIT(KSQLImage2Column,					"Image2");
/**
* This next literal, corresponds to the name of the 
* image column in an old-format help file (2 columns). 
*/
_LIT(KSQLImageColumn,					"Image");

_LIT(KSQLTopicByCategoryUIDProlog,		"SELECT TOPICTITLE, TOPICID, CATEGORYUID FROM TOPIC WHERE CATEGORYUID=");
_LIT(KSQLTopicByCategoryProlog,			"SELECT TOPICTITLE, TOPICID, CATEGORYUID FROM TOPIC WHERE CATEGORY=");
_LIT(KSQLCategoryList,					"SELECT TOPICTITLE, TOPICID, CATEGORYUID FROM TOPIC");
_LIT(KSQLTopicSearchProlog,				"SELECT TOPICTITLE, TOPICID, CATEGORY, TOPICTEXT, TOPICMARKUP FROM TOPIC WHERE ");
_LIT(KSQLTopicIdSearchProlog,			"SELECT TOPICTITLE, TOPICID, CATEGORYUID FROM TOPIC WHERE ");
_LIT(KSQLIndexSearch,					"SELECT INDEXID FROM INDEX WHERE INDEX ='");
_LIT(KSQLIndexIdSearch,					"SELECT TOPICID, CATEGORYUID, TOPICTITLE FROM TOPICINDEX WHERE INDEXID=");
_LIT(KSQLContextSearch,					"SELECT CONTEXT, TOPICID, CATEGORYUID FROM CONTEXT WHERE CONTEXT='");
_LIT(KSQLIndexList,						"SELECT INDEX, INDEXID FROM INDEX");

_LIT(KSQLTopicOrderBy,					" ORDER BY TOPICTITLE ASC");

_LIT(KSQLTxtSrchCat,					"*' OR Category LIKE '*");
_LIT(KSQLTxtSrchTopicText,				"*' OR TopicText LIKE '*");
_LIT(KSQLTxtSrchTopicTitle,				"TopicTitle LIKE '*");
_LIT(KSQLTxtSrchSynonym,				"*' OR Synonym LIKE '*");

_LIT(KSQLSingleInvComma,				"'");
_LIT(KSQLLikeOpeningInvComma,			"'*");
_LIT(KSQLLikeClosingInvComma,			"*'");

_LIT(KSQLOrOperator,					" OR ");
_LIT(KSQLEqualOperator,					" = ");
_LIT(KSQLLikeOperator,					" LIKE ");
_LIT(KSQLAndOperator,					" AND ");


#endif
