/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

/**
 * This class is to test the List:
 *
 */

public class Test6 extends List implements CommandListener
{

    //the instance of this
    private Test6 instance = null;
    //the command to go back to the ListTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //elements to this list
    private final String element1 = "1st Element is the longest one and it should be truncated if it is too long.";
    private final String element2 ="2nd Element";
    private final String element3 ="3rd Element";
    private final String element4 ="4th Element";
    private final String element5 ="5th Element";
    private final String element6 ="6th Element";
    private final String element7 ="7th Element";
    private final String element8 ="8th Element";
    private final String element9 ="9th Element";
    private final String element10 ="10th Element";
    private final String element11 ="11th Element";

    public Test6(int listType)
    {
        super("Test6", listType);
        //append elements to this list
        append(element1, null);
        append(element2, null);
        append(element3, null);
        append(element4, null);
        append(element5, null);
        append(element6, null);
        append(element7, null);
        append(element8, null);
        append(element9, null);
        append(element10, null);
        append(element11, null);
        //add commands to this list
        addCommand(cmdBack);
        setCommandListener(this);

        if (instance == null)
            instance = this;
    }
    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
            ListTests.show();
    }
}


