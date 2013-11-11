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

#ifndef MOBCNTMODELPRIVATE_H
#define MOBCNTMODELPRIVATE_H

#include <QSharedData>
#include <QMap>

#include <qcontactmanager.h>
#include <qcontactfilter.h>
#include <qcontactdetailfilter.h>
#include <qcontactsortorder.h>
#include <cntuids.h>
#include "cntcache.h"
#include <cntdebug.h>
#include "cntdisplaytextformatter.h"
#include <xqsettingsmanager.h> 
#include <xqsettingskey.h>

QTM_USE_NAMESPACE

class CntListModelData : public QSharedData
{
public:
    CntListModelData( const QContactFilter& contactFilter = QContactFilter(),
                      bool showMyCard = true) :
                          m_contactManager(NULL),
                          m_ownedContactManager(false),
                          m_currentRow(-1),
                          m_showMyCard(showMyCard),
                          m_Settings(NULL),
                          m_NameListRowSettingkey(NULL),
                          m_currentRowSetting(0),
                          m_groupId(-1),
                          m_Format( new CntDummyDisplayTextFormatter )
        { 
            setFilter(contactFilter);
        }

    ~CntListModelData()
        {
            if (m_ownedContactManager) 
                delete m_contactManager;
            delete m_Settings;
            delete m_NameListRowSettingkey;
            delete m_Format;
        }

    void setFilter(const QContactFilter& contactFilter)
        {
            CNT_LOG_ARGS(contactFilter.type())

            m_filter = contactFilter;
            m_currentRow = -1;
            if (contactFilter.type() == QContactFilter::RelationshipFilter) {
                    QContactRelationshipFilter* relationshipFilter = static_cast<QContactRelationshipFilter*>(&m_filter);
                    CNT_LOG_ARGS("type:" << relationshipFilter->relationshipType() << "role:" << relationshipFilter->relatedContactRole())
                    if (relationshipFilter->relationshipType() == QContactRelationship::HasMember &&
                        relationshipFilter->relatedContactRole() == QContactRelationship::First)
                        m_groupId = relationshipFilter->relatedContactId().localId();
            }
            else {
                m_groupId = -1;
                
                // set proper text formatter for the display name. 
                if ( contactFilter.type() == QContactFilter::ContactDetailFilter )
                {
                    delete m_Format;
                    m_Format = NULL;
                    
                    QContactDetailFilter* detailFilter = static_cast<QContactDetailFilter*>( &m_filter );
                    QStringList filter = detailFilter->value().toStringList();
                    
                    if ( detailFilter->detailDefinitionName() == QContactDisplayLabel::DefinitionName && 
                         detailFilter->matchFlags() & QContactFilter::MatchStartsWith &&
                         !filter.isEmpty() )
                    {
                        m_Format = new CntHTMLDisplayTextFormatter();
                    }
                    else
                    {
                        m_Format = new CntDummyDisplayTextFormatter();    
                    }
                }
                    
            }

            CNT_LOG_ARGS(m_groupId)
        }

public:
    QContactManager* m_contactManager;
    CntCache* m_cache;
    bool m_ownedContactManager;
    mutable CntContactInfo m_currentContact;
    mutable int m_currentRow;
	
    QList<QContactLocalId> m_contactIds;
    QContactFilter m_filter;
    QList<QContactSortOrder> m_sortOrders;
    bool m_showMyCard;
	QContactLocalId m_myCardId;
	int nameOrder;
	
    XQSettingsManager* m_Settings;
    XQSettingsKey *m_NameListRowSettingkey;
    int m_currentRowSetting;
    QContactLocalId m_groupId;
    
    CntDisplayTextFormatter* m_Format;
};

#endif // QCONTACTMODELPRIVATE_H

