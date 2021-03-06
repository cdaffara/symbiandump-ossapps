/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *  This is the main class for the Midp_General_05 tests.
 */

public class Midp_General_05 extends MIDlet implements CommandListener
{

    private Canvas canvas;
    private Display display;
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public Midp_General_05()
    {
        display = Display.getDisplay(this);
        canvas = new EmptyCanvas();
        canvas.addCommand(cmdExit);
        canvas.setCommandListener(this);
    }

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        display.setCurrent(canvas);
    }

    /**
     *  Signals the MIDlet to terminate and enter the Destroyed state.
     *
     */
    protected void destroyApp(boolean unconditional)
    {
    }

    /**
     *  Signals the MIDlet to stop and enter the Paused state.
     */
    protected void pauseApp()
    {
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdExit)
        {
            destroyApp(false);
            notifyDestroyed();
        }
    }
}

class EmptyCanvas extends Canvas
{
    protected void paint(Graphics g)
    {
        g.setColor(0, 0, 0);
        g.drawString("Empty Canvas", getWidth()/2, getHeight()/2, Graphics.TOP | Graphics.HCENTER);
    }
}
