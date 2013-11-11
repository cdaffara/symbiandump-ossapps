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
#ifndef INPDOCUMENTFIELD_
#define INPDOCUMENTFIELD_

/**
 * CDocumentField. Instance of this class describes one field of a CSearchDocument. 
 */
class MNPDocumentField
	{
public:
	/**
	 * Name of the field.
	 * @return Reference to the name of the field.
	 */
	virtual const TDesC& Name() const = 0;

	/**
	 * Value of the field.
	 * @return Reference to the value of the field.
	 */
	virtual const TDesC& Value() const = 0;
	
	};

#endif /*INPDOCUMENTFIELD_*/
