#ifndef oxygenanimationmodes_h
#define oxygenanimationmodes_h

/*
* oxygenanimationmodes.h
* animation modes
* -------------------
*
* Copyright (c) 2012 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* Largely inspired from Qtcurve style
* Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

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
