/** bplIO.hpp
 *
 * Copyright 2021 BartekkPL
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * Author
 * ~~~
 * Bartłomiej Kunikowski, aka BartekkPL, ©BartekkPL 2021
 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <termios.h>

#include <cstdio>
#include <unistd.h>


/**
 * Offers raw input methods known from Windows system that allows for low level
 * input reading.
 *
 * This version needs from user to enable it when starting work
 * with it and disable it after work is finished, or when you want back to
 * standard library input methods. Standard library's output methods like cout,
 * or printf can be used simultaneously.
 *
 * It's destination are Unix systems.
 */
class bplIO
{
public:
    /**
     * Initializes bplIO and enables it.
     *
     * @note You should disable it with disableIO() function if you want use
     * stdin in normal way (eg. cin or scanf), because it will work in no buffer
     * way otherwise.
     *
     * @see disableIO
     */
    static void enableIO()
    {
        if ( !initialized )
        {
            // Use termios to turn off line buffering
            termios term;
            tcgetattr( STDIN, &term );
            term_backup = term;
            term.c_lflag &= ~ICANON;
            tcsetattr( STDIN, TCSANOW, &term );
            initialized = true;
        }

        setbuf( stdin, nullptr );
    }

    /**
     * Disables bplIO and restores default settings for terminal and for
     * standard input.
     *
     * @note If you call earlier enableIO() function and
     * want to use standard library IO then you should use this function.
     *
     * @see enableIO
     */
    static void disableIO()
    {
        setlinebuf( stdin );
        tcsetattr( STDIN, TCSANOW, &term_backup );
    }

    /**
     * Checks if there was any button pressed.
     *
     * @note bplIO need to be enabled with enableIO()
     *
     * @return It returns non-zero value if there is key pressed in the buffer.
     * Otherwise returns 0.
     *
     * @see enableIO
     */
    static int _kbhit()
    {
        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }

    /**
     * It returns character from input buffer. Don't have any error handling.
     *
     * @note This function can read single character from the input without
     * echo. It cannot be used to read CTRL + C combination. During reading
     * combination of keys or arrow keys, this function needs to be called more
     * than once. bplIO need to be enabled with enableIO()
     *
     * @return It returns non-zero value if there is key pressed in the buffer. Otherwise returns 0.
     *
     * @see enableIO
     */
    static int _getch()
    {
        char buf[1];
        read(STDIN, buf, 1);

        return buf[0];
    }

    /**
     * It clears input buffer from unread characters.
     *
     * @note bplIO need to be enabled with enableIO()
     *
     * @see enableIO
     */
    static void _flush()
    {
        ioctl(STDIN, TCFLSH, 0);
    }

private:
    static const int STDIN = 0;
    static bool initialized;
    static termios term_backup;

}; // class bplIO

// ------------------------------------------------------------
//  Static variables initialization
// ------------------------------------------------------------

bool bplIO::initialized = false;
termios bplIO::term_backup;
