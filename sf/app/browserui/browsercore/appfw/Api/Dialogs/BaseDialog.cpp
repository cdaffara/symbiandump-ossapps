/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include "BaseDialog.h"

#define BASE_DIALOG_FLAGS ( Qt::Dialog )

static int dialogCounter = 0; 	
  
BaseDialog::BaseDialog(QWidget* parent, Qt::WindowFlags flags) :
	QDialog(parent, (BASE_DIALOG_FLAGS | flags))
{
	dialogCounter++; 
	if (parent)
	{
		setFixedWidth(parent->size().width() * .97);
	}
}

void BaseDialog::setWindowFlags(Qt::WindowFlags flags)
{
	QWidget::setWindowFlags(BASE_DIALOG_FLAGS | flags);
}

void BaseDialog::disableSizeGrip()
{
	QDialog::setSizeGripEnabled(false);
}

int BaseDialog::getCount()
{ 
    return dialogCounter; 
} 

BaseDialog::~BaseDialog()
{
    dialogCounter--;
}