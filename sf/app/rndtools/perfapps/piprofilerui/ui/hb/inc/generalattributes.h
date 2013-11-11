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

#ifndef GENERALATTRIBUTES_H_
#define GENERALATTRIBUTES_H_
/*
 * 
 * GeneralAttributes class definition, internal settings format
 *  
 */
class GeneralAttributes
{
public:
    QString mTraceOutput;
    QString mTraceFilePrefix;
    QString mSaveFileDrive;
    int mTimedSamplingPeriod;
};

#endif /* GENERALATTRIBUTES_H_ */
