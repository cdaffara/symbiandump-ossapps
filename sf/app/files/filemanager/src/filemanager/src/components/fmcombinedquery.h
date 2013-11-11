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
 * 
 * Description:
 *     The header file of combined query of of file manager
 */
#ifndef FMCOMBINEDQUERY_H
#define FMCOMBINEDQUERY_H

#include "fmcommon.h"
#include "fmdialog.h"

#include <QString>
#include <QMap>
#include <QStringList>
#include <QGraphicsItem>

class HbListWidget;
class QGraphicsWidget;

// CLASS DECLARATION
/**
 *  This class is used for popup query dialog
 *  Support multi, single select
 *
 */
class FmCombinedQuery : public FmDialog
{
    Q_OBJECT
public:
	enum SelectMode 
    {
        NoSelectMode        = 0,
        SingleSelectMode    = 1,
        MultiSelectMode     = 2,
    };

	FmCombinedQuery( QGraphicsItem *parent = 0 );
	~FmCombinedQuery( void );

    /**
     * Set select mode
     * @param mode select mode
     * @see SelectMode
     */
	void setSelectMode( SelectMode mode );
    
    /**
     * Set select mode
     * @param mode select mode
     * @return return select mode
     * @see SelectMode
     */
    inline SelectMode selectMode() const
    {
		return mSelectMode;
	}

    /**
     * Set one row as marked
     * @param row the row number which need be marked
     */
    void setMarkedListItem( int row, bool mark = true );

    /**
     * Set dominant index for query list, if click dominant item, clear check status of other items.
     * @param index dominant index
     */
    void setDominantIndex( int index );

    /**
     * Set list item data
     * @param list item data as QMap, key is the userdata of list item as value will be displayed.
     * @see selectedValues 
     * @see selectedKeys
     */
    void setListItems(const QMap< int, QString > &list);
   
    /**
     * Fetch the selected result of vaules from QMap those are set by setListItems function
     * @return QStringList of selected values
     * @see setListItems 
     * @see selectedKeys
     */
    QStringList selectedValues() const;

    /**
     * Fetch the selected result of keys from QMap those are set by setListItems function
     * @return QList with int of selected keys
     * @see setListItems 
     * @see selectedValues
     */
    QList<int> selectedKeys() const;

signals:
    void doCheckForPrimaryAction();

public slots:
	void activated(const QModelIndex &index); 
    void checkForPrimaryAction();
private:
    SelectMode      mSelectMode;        /// select mode , MultiSelectMode is default value
	QGraphicsWidget *mContentWidget;    /// Content widget of popup dialog
    HbListWidget    *mListWidget;       /// list widget
	int	            mDominantIndex;      /// index of dominant item. -1 for none
};

#endif //FMCOMBINEDQUERY_H
