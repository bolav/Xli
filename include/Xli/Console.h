/* * Copyright (C) 2010-2014 Outracks Technologies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __XLI_CONSOLE_H__
#define __XLI_CONSOLE_H__

#include <Xli/TextReader.h>
#include <Xli/TextWriter.h>

namespace Xli
{
    /**
        \ingroup XliDiagnostics
    */
    class Console
    {
    public:
        static void Init();
        static void Done();
    };

    /**
        \ingroup XliDiagnostics
    */
    class StdOutAccessor
    {
    public:
        TextWriter* operator ->();
        operator TextWriter*();
    };

    /**
        \ingroup XliDiagnostics
    */
    class StdErrAccessor
    {
    public:
        TextWriter* operator ->();
        operator TextWriter*();
    };

    /**
        \ingroup XliDiagnostics
    */
    class StdInAccessor
    {
    public:
        TextReader* operator ->();
        operator TextReader*();
    };

    /**
        \addtogroup XliDiagnostics
        @{
    */

    extern StdOutAccessor Out;
    extern StdErrAccessor Err;
    extern StdInAccessor In;

    template <typename T> 
    XLI_INLINE void Print(const T& t)
    {
        Out->Write(t);
    }

    template <typename T> 
    XLI_INLINE void PrintLine(const T& t)
    {
        Out->WriteLine(t);
    }

    XLI_INLINE void PrintLine()
    {
        Out->Write('\n');
    }

    template <typename T> 
    void ErrorPrint(const T& t)
    {
        Err->Write(t);
    }

    template <typename T> 
    void ErrorPrintLine(const T& t)
    {
        Err->WriteLine(t);
    }

    XLI_INLINE void ErrorPrintLine()
    {
        Err->Write('\n');
    }

    /** @} */
}


#endif
