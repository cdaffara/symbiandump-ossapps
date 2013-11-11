/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: msgmonitor.inl
*/



// ========== INLINE METHODS ===============================

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::setSkipNote
// @see header file
//---------------------------------------------------------------
inline void MsgUnifiedEditorMonitor::setSkipNote(bool skip)
{
    mSkipNote = skip;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::messageType
// @see header file
//---------------------------------------------------------------
inline ConvergedMessage::MessageType MsgUnifiedEditorMonitor::messageType()
{
    return mMessageType;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::messageSize
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::messageSize()
{
    return mBodySize + mContainerSize + mSubjectSize;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::bodySize
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::bodySize()
{
    return mBodySize;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::containerSize
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::containerSize()
{
    return mContainerSize;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::subjectSize
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::subjectSize()
{
    return mSubjectSize;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::maxMmsSize
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::maxMmsSize()
{
    return mMaxMmsSize;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::maxSmsRecipients
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::maxSmsRecipients()
{
    return mMaxSmsRecipients;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::maxMmsRecipients
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::maxMmsRecipients()
{
    return mMaxMmsRecipients;
}

//---------------------------------------------------------------
// MsgUnifiedEditorMonitor::msgAddressCount
// @see header file
//---------------------------------------------------------------
inline int MsgUnifiedEditorMonitor::msgAddressCount()
{
    return mMsgCurrAddressCount;
}

//  End of File
