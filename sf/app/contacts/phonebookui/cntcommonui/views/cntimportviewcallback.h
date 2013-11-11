/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTIMPORTVIEWCALLBACK_H_
#define CNTIMPORTVIEWCALLBACK_H_

class QString;

/*!
 * CntImportViewInterface
 * interface for imports view, enables clients to set data in the list box items
 */

class CntImportViewCallback
{
public:
    virtual ~CntImportViewCallback() {};
    virtual void setListBoxItemText(QString& aPrimary, QString& aSecondary) = 0;
    virtual void setListBoxItemEnabled(bool aEnabled) = 0;
};

#endif /* CNTIMPORTVIEWCALLBACK_H_ */
