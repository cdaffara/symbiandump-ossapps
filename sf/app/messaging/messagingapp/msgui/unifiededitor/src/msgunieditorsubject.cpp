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

// INCLUDES
#include "debugtraces.h"
#include <HbIconItem>
#include <HbNotificationDialog>
#include <hbmessagebox.h>

// USER INCLUDES
#include "msgunieditorsubject.h"
#include "UniEditorGenUtils.h"
#include "msgunieditorlineedit.h"
#include "msgunieditormonitor.h"

// Localized Constants
#define LOC_SUBJECT hbTrId("txt_messaging_formlabel_subject")
#define LOC_UNABLE_TO_ADD_CONTENT hbTrId("txt_messaging_dpopinfo_unable_to_add_more_content")

//priority icon
const QString HIGH_PRIORITY("qtg_small_priority_high");
const QString LOW_PRIORITY("qtg_small_priority_low");

//---------------------------------------------------------------
// MsgUnifiedEditorSubject::MsgUnifiedEditorSubject
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorSubject::MsgUnifiedEditorSubject( QGraphicsItem *parent ) :
MsgUnifiedEditorBaseWidget(parent),
mPriorityIcon(NULL),
mPriority(ConvergedMessage::Normal),
mGenUtils(0)
{
        mSubjectEdit = new MsgUnifiedEditorLineEdit(LOC_SUBJECT,this);
        mSubjectEdit->setDefaultBehaviour(true);        
        HbStyle::setItemName(mSubjectEdit,"subjectEdit");
        mSubjectEdit->setMinRows(1);
        mSubjectEdit->setMaxRows(10);
        
        mGenUtils = q_check_ptr(new UniEditorGenUtils());
        
        connect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsChanged(const QString&)));
}

//---------------------------------------------------------------
// MsgUnifiedEditorSubject::~MsgUnifiedEditorSubject
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorSubject::~MsgUnifiedEditorSubject()
{
    if(mGenUtils)
    {
        delete mGenUtils;
        mGenUtils = NULL;
    }
}

void MsgUnifiedEditorSubject::setPriority(ConvergedMessage::Priority priority)
{
	mPriority = priority;
    if(mPriorityIcon)
    {// Transition from low/high => low/normal/high
        mPriorityIcon->setParent(NULL);
        delete mPriorityIcon;
        mPriorityIcon = NULL;
    }

    switch(priority)
    {
        case ConvergedMessage::High :
        {
        mPriorityIcon = new HbIconItem(HIGH_PRIORITY, this);
        HbStyle::setItemName(mPriorityIcon,"priorityIcon");
        }
        break;
        case ConvergedMessage::Low :
        {
        mPriorityIcon = new HbIconItem(LOW_PRIORITY, this);
        HbStyle::setItemName(mPriorityIcon,"priorityIcon");
        }
        break;
        default:
        break;
    }

    emit contentChanged();
    this->repolish();
}

QString MsgUnifiedEditorSubject::text()
{
    return mSubjectEdit->content();
}

ConvergedMessage::Priority MsgUnifiedEditorSubject::priority()
{
	return mPriority;
}

void MsgUnifiedEditorSubject::onContentsChanged(const QString& text)
{
    // reject any text input if mms size limit is reached
    int futureSize = subjectSize() +
            MsgUnifiedEditorMonitor::containerSize() + MsgUnifiedEditorMonitor::bodySize();
    if(futureSize > MsgUnifiedEditorMonitor::maxMmsSize())
    {
        // atomic operation
        disconnect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                    this, SLOT(onContentsChanged(const QString&)));
        mSubjectEdit->clearContent();
        mSubjectEdit->setText(mPrevBuffer);
        connect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                        this, SLOT(onContentsChanged(const QString&)));
        HbMessageBox::information(LOC_UNABLE_TO_ADD_CONTENT, 0, 0, HbMessageBox::Ok);
        return;
    }

    mPrevBuffer = text;
    if(!subjectOkInSms())
    {
        emit contentChanged();
    }
}

bool MsgUnifiedEditorSubject::subjectOkInSms()
{
    bool result = false;
    int error;
    TRAP(error, result = mGenUtils->AcceptEmailAddressesL());
    return result;    
}

int MsgUnifiedEditorSubject::subjectSize()
{
    return mGenUtils->UTF8Size(mSubjectEdit->content());
}

void MsgUnifiedEditorSubject::setText(const QString& text)
{
    if(!text.isEmpty())
    {
        mSubjectEdit->setText(text);
    }
}

void MsgUnifiedEditorSubject::setFocus()
{
    mSubjectEdit->setFocus(Qt::MouseFocusReason);
}

//EOF
