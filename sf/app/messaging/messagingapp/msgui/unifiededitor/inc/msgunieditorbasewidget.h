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
 * Description:common base class for editor fields(address field,subject field,
 * and body field.
 *
 */

#include <HbWidget>

#ifndef MSG_UNIFIED_EDITOR_BASEWIDGET_H
#define MSG_UNIFIED_EDITOR_BASEWIDGET_H

class MsgUnifiedEditorBaseWidget: public HbWidget
{
    Q_OBJECT
    
public:
    MsgUnifiedEditorBaseWidget(QGraphicsItem *parent = 0):HbWidget(parent){}
    virtual void setFocus(){}
    
};


#endif /* MSG_UNIFIED_EDITOR_BASEWIDGET_H */
