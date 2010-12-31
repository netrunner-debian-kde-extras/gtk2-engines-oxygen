#ifndef oxygenwidgetsizedata_h
#define oxygenwidgetsizedata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include <gtk/gtk.h>

namespace Oxygen
{
    // track main window resize events
    class WidgetSizeData
    {

        public:

        //! constructor
        WidgetSizeData( void ):
            _width(-1),
            _height(-1)
        {}

        //! destructor
        virtual ~WidgetSizeData( void )
        {}

        //! setup connections
        void connect( GtkWidget* ) const
        {}

        //! disconnect
        void disconnect( GtkWidget* ) const
        {}

        //! update size, returns true if changed
        virtual bool updateSize( int width, int height )
        {
            if( width == _width && height == _height ) return false;
            _width = width;
            _height = height;
            return true;
        }

        private:

        //! old width
        int _width;

        //! old height
        int _height;

    };

}

#endif
