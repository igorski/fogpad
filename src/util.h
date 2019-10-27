/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Igor Zinken - https://www.igorski.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__

#include <fstream>
#include <string>
#include <time.h>

namespace Igorski {
namespace Util {

    /**
     * Convenience method to log a message to a file
     * multiple messages can be written to the same file (are
     * separated by a new line)
     *
     * This should be used for debugging purposes only
     */
    void log( const char* message, const char* filename )
    {
        std::ofstream out;

        char buff[20];
        struct tm *sTm;

        time_t now = time( 0 );
        sTm        = gmtime( &now );

        strftime( buff, sizeof( buff ), "%Y-%m-%d %H:%M:%S", sTm );

        out.open( filename, std::ios_base::app );
        out << buff << " " << message << "\n";

        out.close();
    }

    void log( std::string message, const char* filename )
    {
        log( message.c_str(), filename );
    }

    void log( float value, const char* filename )
    {
        log( std::to_string( value ), filename );
    }

    void log( int value, const char* filename )
    {
        log( std::to_string( value ), filename );
    }

}
}

#endif
