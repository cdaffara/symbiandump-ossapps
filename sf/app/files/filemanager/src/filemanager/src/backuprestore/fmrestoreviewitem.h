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
 *     The header file of the restore view list item of file manager
 */

#ifndef FMRESTOREVIEWITEM_H
#define FMRESTOREVIEWITEM_H

#include <hblistviewitem.h>

class HbLabel;
class HbCheckBox;
class HbLineEdit;
class QGraphicsLinearLayout;
class HbWidget;
class FmFileIconProvider;
//FmRestoreViewItem
class FmRestoreViewItem : public HbListViewItem
{
	Q_OBJECT

public:
    FmRestoreViewItem( QGraphicsItem *parent = 0  );
	~FmRestoreViewItem();

public:
	virtual HbAbstractViewItem *createItem();
	virtual void updateChildItems();
	void setCheckBoxState();
	bool getCheckBoxState();
signals:
    void stateChanged(int);
	
	
private:
    virtual void polish(HbStyleParameters& params);

private:
    void init();

private:
    HbLabel *mRestoreContentLabel;
    HbLabel *mDateTimeLabel;    

	HbCheckBox *mCheckBox;
	QGraphicsLinearLayout *hLayout;
	HbWidget *mParentWidget;
	HbLabel *mIconLabel;
    FmFileIconProvider   *mIconProvider;
};

#endif
