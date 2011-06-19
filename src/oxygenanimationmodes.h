#ifndef oxygenanimationmodes_h
#define oxygenanimationmodes_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationmodes.h
// animation modes
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////
#include "oxygenflags.h"

namespace Oxygen
{

    //! animation type
    /*! used for menubars, toolbars and menus */
    enum AnimationType
    {
        None,
        Fade,
        FollowMouse
    };

    //! animated widget type
    /*!
    used for engines that keep track of previously(fade-out) and
    currently (fade-in) animated objects
    */
    enum WidgetType
    {
        AnimationCurrent,
        AnimationPrevious
    };

    //! animation mode
    enum AnimationMode
    {
        AnimationNone = 0,
        AnimationHover = 1<<0,
        AnimationFocus = 1<<1,
    };

    OX_DECLARE_FLAGS( AnimationModes, AnimationMode )
    OX_DECLARE_OPERATORS_FOR_FLAGS( AnimationModes )

}

#endif