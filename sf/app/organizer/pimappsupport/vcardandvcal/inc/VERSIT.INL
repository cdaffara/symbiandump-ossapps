// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

inline void CLineReader::SetPlugIn(MVersitPlugIn* aPlugIn)
/** Sets the plug-in.

Allows implemented functions of the MVersitPlugIn class to be used in specifying 
options for adding and deleting spaces to/from the buffer. 

Specifically, the plug-in is used to determine whether an additional space should 
be added when appending a line to the buffer (by default a space will be added), 
and whether the white space at the start of a line should form part of the data 
(by default it will not form part of the data).

Using a plug-in is optional. 

@param aPlugIn A pointer to an MVersitPlugIn instance. */
	{iPlugIn=aPlugIn;}

inline void CLineReader::SetSkipWhiteSpaceAtStart(TBool aDoSkip)
	{iSkipWhiteSpaceAtStart=aDoSkip;}

inline void CVersitParser::SetObserver(MVersitObserver* aObserver)
/** Sets the Versit observer.

@param aObserver A pointer to the observer. */
	{iObserver=aObserver;}

inline MVersitObserver* CVersitParser::Observer()
/** Gets a pointer to the Versit observer.

@return A pointer to the observer. */
	{return iObserver;}

inline void CVersitParser::SetPlugIn(MVersitPlugIn* aPlugIn)
/** Sets the Versit plug-in.

If there is one, the Versit plug-in needs to be set before any properties are 
added to the parser. This is done for you when internalising (using InternalizeL()) 
or adding properties (using AddPropertyL()).

@param aPlugIn A pointer to the plug in. */
	{iPlugIn=aPlugIn;}

inline MVersitPlugIn* CVersitParser::PlugIn()
/** Gets a pointer to the Versit plug-in.

@return A pointer to the plug-in. */
	{return iPlugIn;}

/* Gets a pointer to the read buffer owned by the CLineReader.
 *
 * @return Pointer to the read buffer. */
inline TPtr8& CVersitParser::BufPtr()
	{return iLineReader->iBufPtr;}

/*
 * Check the iFlags to see if <code>ESupportsVersion</code> is set
 *
 * @return    " TBool "
 *            <code>ETrue</code> if the parser supports the version property
 *            <code>EFalse</code> otherwise
 */
inline TBool CVersitParser::SupportsVersion() const
	{return iFlags & ESupportsVersion;}

/*
 * Sets the iFlags to <code>ESupportsVersion</code>
 */
inline void CVersitParser::SetSupportsVersion()
	{iFlags |= ESupportsVersion;}

/*
 * Clear the bit of <code>ESupportsVersion</code> in iFlags'
 */
inline void CVersitParser::ClearSupportsVersion()
	{iFlags &= ~ESupportsVersion;}

/*
 * Set Flags
 */
inline void CVersitParser::SetFlags(TUint aFlags)
	{iFlags |= aFlags;}

/*
 * Gets a reference to <code>CVersitUnicodeUtils</code>
 * which is used for charset conversion 
 *
 * @return    " CVersitUnicodeUtils&"
 *            reference to <code>CVersitUnicodeUtils</code>
 */
inline CVersitUnicodeUtils& CVersitParser::UnicodeUtils()
	{ return iStaticUtils->UnicodeUtils(); }
