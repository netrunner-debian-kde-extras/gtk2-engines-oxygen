#ifndef oxygenmenuitemengine_h
#define oxygenmenuitemengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenmenuitemdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable menuitemes
    /*!
    ensures that the text entry and the button of editable menuitemes
    gets hovered and focus flags at the same time
    */
    class MenuItemEngine: public GenericEngine<MenuItemData>
    {

        public:

        //! constructor
        MenuItemEngine( Animations* widget ):
            GenericEngine<MenuItemData>( widget )
            {}

        //! destructor
        virtual ~MenuItemEngine( void )
        {}

        //! register all menuItems children of a menu
        virtual bool registerMenu( GtkWidget* );

    };

}

#endif
