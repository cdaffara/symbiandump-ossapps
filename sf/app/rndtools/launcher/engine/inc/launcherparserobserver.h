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

#ifndef LAUNCHERPARSEROBSERVER_H_
#define LAUNCHERPARSEROBSERVER_H_

class CLauncherDLLElement;

class MLauncherParserObserver
{
public:
    
    /**
     * This method is called when parser has parsed next DLL element     
     * @param aDllElement Parsed DLL element.     
     */    
    virtual void ElementParsedL(const CLauncherDLLElement& aDllElement) = 0;
    
    /**
     * This method is called when the parsing is completed. If parsing was
     * stopped due to an error, an error code is given in parameter.
     * @param aErrorCode KErrNone if document was parsed successfully.
     */
    virtual void DocumentParsedL(TInt aErrorCode) = 0;
    
    /**
     * Parser calls this function during parsing to indicate the amount of
     * bytes parsed from the given file.
     * @param aBytes Amount of data (in bytes) parsed. Amount is incremental.
     */
    virtual void ParsingProgressedL(TInt aBytes) = 0;
    
};

#endif /* LAUNCHERPARSEROBSERVER_H_ */
