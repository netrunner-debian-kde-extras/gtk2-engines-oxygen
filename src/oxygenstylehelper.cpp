/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenstylehelper.h"
#include "oxygencairocontext.h"
#include "oxygencairoutils.h"
#include "oxygencolorutils.h"
#include "oxygenrgba.h"

#include <cmath>
#include <gdk/gdk.h>

namespace Oxygen
{

    //__________________________________________________________________
    const double StyleHelper::_slabThickness = 0.45;
    const double StyleHelper::_shadowGain = 1.5;
    const double StyleHelper::_glowBias = 0.6;

    //__________________________________________________________________
    StyleHelper::StyleHelper( void )
    {
        /*
        create dummy widget, get its window,
        creates surface for it, and assign to ref surface
        */
        GtkWidget* widget( gtk_window_new(GTK_WINDOW_TOPLEVEL) );
        gtk_widget_realize( widget );
        Cairo::Context context( gtk_widget_get_window( widget ) );
        _refSurface = Cairo::Surface( cairo_surface_create_similar( cairo_get_target( context ), CAIRO_CONTENT_ALPHA, 1, 1 ) );
        gtk_widget_destroy( widget );
    }

    //__________________________________________________________________
    void StyleHelper::drawSeparator( Cairo::Context& context, const ColorUtils::Rgba& base, int x, int y, int w, int h, bool vertical )
    {

        // get surface
        const Cairo::Surface& surface( separator( base, vertical, vertical ? h:w ) );
        if(!surface) return;

        // translate
        cairo_save( context );
        if( vertical ) cairo_translate( context, x+w/2-1, y );
        else cairo_translate( context, x, y+h/2 );

        cairo_rectangle( context, 0, 0, cairo_surface_get_width( surface ), cairo_surface_get_height( surface ) );
        cairo_set_source_surface( context, surface, 0, 0 );
        cairo_fill( context );
        cairo_restore( context );

    }

    //__________________________________________________________________
    const Cairo::Surface& StyleHelper::separator( const ColorUtils::Rgba& base, bool vertical, int size )
    {

        const SeparatorKey key( base, vertical, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_separatorCache.value(key) )
        { return surface; }

        // for invalid sizes return a null surface
        if( size <= 0 )
        { return m_separatorCache.insert( key, 0L ); }

        // cached not found, create new
        Cairo::Surface surface( vertical ? createSurface( 3, size ):createSurface( size, 2 ) );

        int xStart( 0 );
        int yStart( 0 );
        int xStop( vertical ? 0 : size );
        int yStop( vertical ? size : 0 );
        int xOffset( vertical ? 1:0 );
        int yOffset( vertical ? 0:1 );

        Cairo::Context context( surface );
        cairo_set_line_width( context, 1.0 );

        if( vertical ) cairo_translate( context, 0.5, 0 );
        else cairo_translate( context, 0, 0.5 );

        {
            ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( xStart, yStart, xStop, yStop ) );
            if( vertical ) light.setAlpha( 0.7 );

            cairo_pattern_add_color_stop( pattern, 0.3, light );
            cairo_pattern_add_color_stop( pattern, 0.7, light );
            light.setAlpha( 0 );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, light );
            cairo_set_source( context, pattern );

            if( vertical )
            {
                cairo_move_to( context, xStart, yStart );
                cairo_line_to( context, xStop, yStop );
                cairo_move_to( context, xStart+2*xOffset, yStart+2*yOffset );
                cairo_line_to( context, xStop+2*xOffset, yStop+2*yOffset );

            } else {

                cairo_move_to( context, xStart+xOffset, yStart+yOffset );
                cairo_line_to( context, xStop+xOffset, yStop+yOffset );

            }

            cairo_stroke( context );
        }

        {
            ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

            Cairo::Pattern pattern( cairo_pattern_create_linear( xStart, yStart, xStop, yStop ) );
            cairo_pattern_add_color_stop( pattern, 0.3, dark );
            cairo_pattern_add_color_stop( pattern, 0.7, dark );
            dark.setAlpha(0);
            cairo_pattern_add_color_stop( pattern, 0, dark );
            cairo_pattern_add_color_stop( pattern, 1, dark );
            cairo_set_source( context, pattern );

            if( vertical )
            {

                cairo_move_to( context, xStart+xOffset, yStart+yOffset );
                cairo_line_to( context, xStop+xOffset, yStop+yOffset );

            } else {

                cairo_move_to( context, xStart, yStart );
                cairo_line_to( context, xStop, yStop );
            }

            cairo_stroke( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return m_separatorCache.insert( key, surface );

    }

    //______________________________________________________________________________
    const Cairo::Surface& StyleHelper::windecoButton(const ColorUtils::Rgba &base, bool pressed, int size)
    {

        const WindecoButtonKey key( base, size, pressed );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_windecoButtonCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( size, size ) );

        // calculate colors
        ColorUtils::Rgba light = ColorUtils::lightColor(base);
        ColorUtils::Rgba dark = ColorUtils::darkColor(base);

        // create cairo context
        Cairo::Context context( surface );
        const double u = double(size)/18.0;
        cairo_translate( context, 0.5*u, (0.5-0.668)*u );

        {

            // plain background
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, u*1.665, 0, u*(12.33+1.665) ) );
            if( pressed )
            {
                cairo_pattern_add_color_stop( pattern, 1, light );
                cairo_pattern_add_color_stop( pattern, 0, dark );
            } else {
                cairo_pattern_add_color_stop( pattern, 0, light );
                cairo_pattern_add_color_stop( pattern, 1, dark );
            }

            cairo_ellipse( context, u*0.5*(17-12.33), u*1.665, u*12.33, u*12.33 );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        {
            // outline circle
            const double penWidth( 0.7 );
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, u*1.665, 0, u*(2.0*12.33+1.665) ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, dark );

            cairo_ellipse( context, u*0.5*(17-12.33+penWidth), u*(1.665+penWidth), u*(12.33-penWidth), u*(12.33-penWidth) );
            cairo_set_source( context, pattern );
            cairo_set_line_width( context, penWidth );
            cairo_stroke( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return m_windecoButtonCache.insert( key, surface );

    }

    //_______________________________________________________________________
    const Cairo::Surface& StyleHelper::windecoButtonGlow(const ColorUtils::Rgba &base, int size)
    {

        const WindecoButtonGlowKey key( base, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_windecoButtonGlowCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( size, size ) );

        // right now the same color is used for the two shadows
        const ColorUtils::Rgba& light( base );
        const ColorUtils::Rgba& dark( base );

        Cairo::Context context( surface );
        const double u = double(size)/18.0;
        cairo_translate( context, 0.5*u, (0.5-0.668)*u );

        {

            // outer shadow
            Cairo::Pattern pattern( cairo_pattern_create_radial( u*8.5, u*8.5, u*8.5 ) );

            static const int nPoints( 5 );
            double x[5] = { 0.61, 0.72, 0.81, 0.9, 1};
            double values[5] = { 255-172, 255-178, 255-210, 255-250, 0 };
            ColorUtils::Rgba c = dark;
            for( int i = 0; i<nPoints; i++ )
            { c.setAlpha( values[i]/255 ); cairo_pattern_add_color_stop( pattern, x[i], c ); }

            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, size, size );
            cairo_fill( context );
        }

        {
            // inner shadow
            Cairo::Pattern pattern( cairo_pattern_create_radial( u*8.5, u*8.5, u*8.5 ) );

            static const int nPoints(6);
            const double x[6] = { 0.61, 0.67, 0.7, 0.74, 0.78, 1 };
            const double values[6] = { 255-92, 255-100, 255-135, 255-205, 255-250, 0 };
            ColorUtils::Rgba c( light );
            for( int i = 0; i<nPoints; i++ )
            { c.setAlpha( values[i]/255 ); cairo_pattern_add_color_stop( pattern, x[i], c ); }

            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, size, size );
            cairo_fill( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return m_windecoButtonGlowCache.insert( key, surface );

    }

    //_________________________________________________
    const Cairo::Surface& StyleHelper::verticalGradient( const ColorUtils::Rgba& base, int height )
    {

        const VerticalGradientKey key( base, height );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_verticalGradientCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( 32, height ) );

        {
            ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
            ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, height ) );
            cairo_pattern_add_color_stop( pattern, 0, top );
            cairo_pattern_add_color_stop( pattern, 0.5, base );
            cairo_pattern_add_color_stop( pattern, 1, bottom );

            Cairo::Context context( surface );
            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, 32, height );
            cairo_fill( context );
        }

        return m_verticalGradientCache.insert( key, surface );
    }

    //_________________________________________________
    const Cairo::Surface& StyleHelper::radialGradient( const ColorUtils::Rgba& base, int radius )
    {

        const RadialGradientKey key( base, radius );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_radialGradientCache.value(key) )
        { return surface; }

        // cached not found, create new
        Cairo::Surface surface( createSurface( 2*radius, radius ) );

        {
            // create radial pattern
            ColorUtils::Rgba radial( ColorUtils::backgroundRadialColor( base ) );
            Cairo::Pattern pattern( cairo_pattern_create_radial( radius, 0, radius ) );
            cairo_pattern_add_color_stop( pattern, 0, radial );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( radial, 101.0/255 ) );
            cairo_pattern_add_color_stop( pattern, 0.75, ColorUtils::alphaColor( radial, 37.0/255 ) );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( radial, 0 ) );

            Cairo::Context context( surface );
            cairo_set_source( context, pattern );
            cairo_rectangle( context, 0, 0, 2*radius, radius );
            cairo_fill( context );
        }

        return m_radialGradientCache.insert( key, surface );

    }

    //_________________________________________________
    const TileSet& StyleHelper::slab(const ColorUtils::Rgba& base, double shade, int size)
    {

        const SlabKey key( base, shade, size );
        const TileSet& tileSet( m_slabCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create surface and initialize
        const int w( 2*size );
        const int h( 2*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {

            // create cairo context
            Cairo::Context context( surface );
            cairo_scale( context, double(size)/7, double(size)/7 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::Rgba::transparent( base ) );
            cairo_fill( context );

            if( base.isValid() )
            {
                drawShadow( context, ColorUtils::shadowColor( base ), 14 );
                drawSlab( context, base, shade );
            }

        }

        // create tileSet
        return m_slabCache.insert( key, TileSet( surface,  size, size, size, size, size-1, size, 2, 1 ) );

    }

    //_________________________________________________
    const TileSet& StyleHelper::slabFocused(const ColorUtils::Rgba& base, const ColorUtils::Rgba& glow, double shade, int size)
    {

        const SlabFocusedKey key( base, glow, shade, size );
        const TileSet& tileSet( m_slabFocusedCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create surface and initialize
        const int w( 2*size );
        const int h( 2*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {
            // create cairo context
            Cairo::Context context( surface );
            cairo_scale( context, double(size)/7, double(size)/7 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::Rgba::transparent( base ) );
            cairo_fill( context );

            if( base.isValid() ) drawShadow( context, ColorUtils::shadowColor( base ), 14 );
            if( glow.isValid() ) drawOuterGlow( context, glow, 14 );
            if( base.isValid() ) drawSlab( context, base, shade );

        }

        // create tileSet
        return m_slabFocusedCache.insert( key, TileSet( surface,  size, size, size, size, size-1, size, 2, 1) );

    }

    //__________________________________________________________________
    const TileSet& StyleHelper::slabSunken( const ColorUtils::Rgba& base, double shade, int size )
    {

        const SlabKey key( base, shade, size );
        const TileSet& tileSet( m_slabSunkenCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create surface and initialize
        const int w( 2*size );
        const int h( 2*size );
        Cairo::Surface surface( createSurface( w, h ) );

        {

            // create cairo context
            Cairo::Context context( surface );
            cairo_scale( context, double(size)/7, double(size)/7 );
            cairo_rectangle( context, 0, 0, 14, 14 );
            cairo_set_source( context, ColorUtils::Rgba::transparent( base ) );
            cairo_fill( context );

            if( base.isValid() )
            {
                drawSlab( context, base, shade );
                drawInverseShadow( context, ColorUtils::shadowColor(base), 3, 8, 0.0);
            }

        }

        // create tileSet
        return m_slabSunkenCache.insert( key, TileSet( surface,  size, size, size, size, size-1, size, 2, 1) );

    }

    //______________________________________________________________________________
    const Cairo::Surface& StyleHelper::roundSlab( const ColorUtils::Rgba& base, double shade, int size )
    {

        const SlabKey key( base, shade, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_roundSlabCache.value( key ) )
        { return surface; }

        // cached not found, create new
        const int w( 3*size );
        const int h( 3*size );
        Cairo::Surface surface( createSurface( w, h ) );

        // create cairo context
        Cairo::Context context( surface );
        cairo_scale( context, double(size)/7, double(size)/7 );

        // shadow
        if( base.isValid() )
        {
            drawShadow( context, ColorUtils::shadowColor(base), 21 );
            drawRoundSlab( context, base, shade );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return m_roundSlabCache.insert( key, surface );

    }

    //__________________________________________________________________________________________________________
    const Cairo::Surface& StyleHelper::roundSlabFocused(const ColorUtils::Rgba& base, const ColorUtils::Rgba& glow, double shade, int size)
    {

        SlabFocusedKey key( base, glow, shade, size );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_roundSlabFocusedCache.value( key ) )
        { return surface; }

        // cached not found, create new
        const int w( 3*size );
        const int h( 3*size );
        Cairo::Surface surface( createSurface( w, h ) );

        // create cairo context
        Cairo::Context context( surface );
        cairo_scale( context, double(size)/7, double(size)/7 );

        // shadow
        if( base.isValid() ) drawShadow( context, ColorUtils::shadowColor(base), 21 );
        if( glow.isValid() ) drawOuterGlow( context, glow, 21 );
        if( base.isValid() ) drawRoundSlab( context, base, shade );

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return m_roundSlabFocusedCache.insert( key, surface );

    }

    //__________________________________________________________________
    void StyleHelper::fillSlab( Cairo::Context& context, int x, int y, int w, int h, const TileSet::Tiles& tiles ) const
    {

        const double s( 3.6 + ( 0.5 * _slabThickness )  );
        const double r( s/2 );

        Corners corners( CornersNone );
        if( tiles & TileSet::Top )
        {
            if( tiles & TileSet::Left ) corners |= CornersTopLeft;
            if( tiles & TileSet::Right ) corners |= CornersTopRight;
        }

        if( tiles & TileSet::Bottom )
        {
            if( tiles & TileSet::Left ) corners |= CornersBottomLeft;
            if( tiles & TileSet::Right ) corners |= CornersBottomRight;
        }


        cairo_rounded_rectangle( context, double(x)+s, double(y)+s, double(w)-2*s, double(h)-2*s, r, corners );
        cairo_fill( context );
    }

    //__________________________________________________________________
    GdkPixmap* StyleHelper::roundMask( int w, int h, int radius ) const
    {

        GdkPixmap* mask( gdk_pixmap_new( 0L, w, h, 1 ) );

        {
            Cairo::Context context( GDK_DRAWABLE(mask) );

            // clear the window
            cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
            cairo_set_source( context, ColorUtils::Rgba::transparent() );
            cairo_paint( context );

            // now draw roundrect mask
            cairo_set_operator( context, CAIRO_OPERATOR_OVER );
            cairo_set_source( context, ColorUtils::Rgba::black() );

            // FIXME: radius found empirically
            cairo_rounded_rectangle( context, 0, 0, w, h, radius );
            cairo_fill( context );
        }

        return mask;

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::hole( const ColorUtils::Rgba &base, const ColorUtils::Rgba& fill, double shade, int size )
    {

        const HoleKey key( base, fill, shade, size );
        const TileSet& tileSet( m_holeCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create pixbuf and initialize
        const int rsize( (int)ceil(double(size) * 5.0/7.0 ) );
        const int w( 2*rsize );
        const int h( 2*rsize );
        Cairo::Surface surface( createSurface( w, h ) );

        {

            Cairo::Context context( surface );
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 10.0/w, 10.0/h );

            // inside
            if( fill.isValid() )
            {
                cairo_ellipse( context, 4, 3, 6, 7 );
                cairo_set_source( context, fill );
                cairo_fill( context );
            }

            // shadow
            drawInverseShadow( context, ColorUtils::shadowColor( base ), 3, 8, 0.0);

        }

        return m_holeCache.insert( key, TileSet( surface, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 ) );
    }

    //______________________________________________________________________________
    const TileSet& StyleHelper::holeFocused( const ColorUtils::Rgba &base, const ColorUtils::Rgba &fill, const ColorUtils::Rgba &glow, double shade, int size )
    {

        const HoleFocusedKey key( base, fill, glow, shade, size );
        const TileSet& tileSet( m_holeFocusedCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create surface and initialize
        const int rsize( (int)ceil(double(size) * 5.0/7.0 ) );
        const int w( 2*rsize );
        const int h( 2*rsize );

        Cairo::Surface surface( createSurface( w, h ) );
        {

            Cairo::Context context( surface );
            const TileSet& holeTileSet = hole( base, fill, shade, size );

            // hole
            holeTileSet.render( context, 0, 0, w, h );

            // glow
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 10.0/w, 10.0/h );

            drawInverseGlow( context, glow, 3, 8, size );

        }

        return m_holeFocusedCache.insert( key, TileSet( surface, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 ) );

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::holeFlat( const ColorUtils::Rgba& base, double shade, int size )
    {

        const HoleFlatKey key( base, shade, size );
        const TileSet& tileSet( m_holeFlatCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int rsize( ( int )ceil( double( size ) * 5.0/7.0 ) );
        const int w( 2*rsize );
        const int h( 2*rsize );

        Cairo::Surface surface( createSurface( w, h ) );

        {

            Cairo::Context context( surface );
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 10.0/w, 10.0/h );

            // hole
            drawHole( context, base, shade, 7 );

            // hole inside
            cairo_set_source( context, base );
            cairo_ellipse( context, 3.4, 3.4, 7.2, 7.2 );
            cairo_fill( context );

        }

        return m_holeFlatCache.insert( key, TileSet( surface, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 ) );

    }

    //______________________________________________________________________________
    const TileSet& StyleHelper::scrollHole( const ColorUtils::Rgba& base, bool vertical )
    {

        const ScrollHoleKey key( base, vertical );
        const TileSet& tileSet( m_scrollHoleCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // define colors
        const ColorUtils::Rgba& dark( ColorUtils::darkColor( base ) );
        const ColorUtils::Rgba& light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba& shadow( ColorUtils::shadowColor( base ) );

        // create pixmap
        const int w( 15 );
        const int h( 15 );
        Cairo::Surface surface( createSurface( w, h ) );

        Cairo::Context context( surface );

        GdkRectangle r = { 0, 0, w, h };
        GdkRectangle rect = { 1, 0, w-2, h-1 };

        const int shadowWidth( vertical ? 2:3 );

        // base
        {
            cairo_set_source( context, dark );
            gdk_cairo_rounded_rectangle( context, &rect, 4.5 );
            cairo_fill( context );
        }

        // light shadow across the whole hole
        {
            Cairo::Pattern pattern;
            if( vertical ) pattern.set( cairo_pattern_create_linear( rect.x, 0, rect.x + rect.width, 0 ) );
            else pattern.set( cairo_pattern_create_linear( 0, rect.y, 0, rect.y + rect.height ) );
            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.1 ) );
            cairo_pattern_add_color_stop( pattern, 0.6, ColorUtils::Rgba::transparent( shadow ) );

            cairo_set_source( context, pattern );
            gdk_cairo_rounded_rectangle( context, &rect, 4.5 );
            cairo_fill( context );
        }

        // strong shadow
        {
            // left
            Cairo::Pattern pattern( cairo_pattern_create_linear( rect.x, 0, rect.x + shadowWidth, 0 ) );
            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, vertical ? 0.2 : 0.3 ) );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( shadow, 0.1 ) );
            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( shadow ) );

            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, rect.x, rect.y, shadowWidth, rect.height, shadowWidth, CornersLeft );
            cairo_fill( context );
        }

        {
            // right
            Cairo::Pattern pattern( cairo_pattern_create_linear( rect.x + rect.width - shadowWidth, 0, rect.x + rect.width, 0 ) );
            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::Rgba::transparent( shadow ) );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( shadow, 0.2 ) );
            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( shadow, vertical ? 0.2 : 0.3 ) );

            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, rect.x + rect.width - shadowWidth, rect.y, shadowWidth, rect.height, shadowWidth, CornersRight );
            cairo_fill( context );
        }

        {
            // top
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, rect.y, 0, rect.y+3 ) );
            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.3 ) );
            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( shadow ) );

            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, rect.x, rect.y, rect.width, 3, 3, CornersTop );
            cairo_fill( context );
        }

        // light bottom border
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, r.y + r.height/2 - 1, 0, r.y + r.height ) );
            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::Rgba::transparent( light ) );
            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( light, 0.8 ) );

            cairo_set_source( context, pattern );
            cairo_set_line_width( context, 1.0 );
            cairo_rounded_rectangle( context, r.x+0.5, r.y, r.width-1, r.height, 5.0 );
            cairo_stroke( context );
        }

        return m_scrollHoleCache.insert( key, TileSet( surface, 7, 7, 1, 1 ) );

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::slitFocused( const ColorUtils::Rgba& glow )
    {

        const SlitFocusedKey key( glow );
        const TileSet& tileSet( m_slitFocusedCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // create pixmap
        const int w( 9 );
        const int h( 9 );
        Cairo::Surface surface( createSurface( w, h ) );
        {
            Cairo::Context context( surface );

            cairo_pattern_t* pattern( cairo_pattern_create_radial( 4.5, 4.5, 4.5 ) );
            ColorUtils::Rgba tmp( ColorUtils::alphaColor( glow, 180.0/255 ) );
            cairo_pattern_add_color_stop( pattern, 0.75, tmp );

            tmp.setAlpha( 0 );
            cairo_pattern_add_color_stop( pattern,  0.90, tmp );
            cairo_pattern_add_color_stop( pattern,  0.4, tmp );

            cairo_set_source( context, pattern );
            cairo_ellipse( context, 0, 0, 9, 9 ) ;
            cairo_fill( context );

        }

        return m_slitFocusedCache.insert( key, TileSet( surface, 4, 4, 1, 1 ) );

    }

    //____________________________________________________________________
    const TileSet& StyleHelper::dockFrame( const ColorUtils::Rgba &base, int w )
    {

        // width should be odd
        if( !w&1 ) --w;

        // key
        DockFrameKey key( base, w );
        const TileSet& tileSet( m_dockFrameCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        // fixed height
        const int h( 9 );

        Cairo::Surface surface( createSurface( w, h ) );
        {

            Cairo::Context context( surface );
            cairo_save( context );
            cairo_translate( context, 0.5, 0.5 );
            cairo_scale( context, 1.0, 4.0/5.0 );

            // local rectangle
            const double xl(0);
            const double yl(0);
            const double wl( w-1 );
            const double hl( (5.0*h)/4.0 );


            const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
            const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

            {
                // left and right border
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, w, 0 ) );
                const ColorUtils::Rgba mixed( ColorUtils::alphaColor( light, 0.6 ) );
                cairo_pattern_add_color_stop( pattern, 0, mixed );
                cairo_pattern_add_color_stop( pattern, 0.1, ColorUtils::Rgba::transparent( light ) );
                cairo_pattern_add_color_stop( pattern, 0.9, ColorUtils::Rgba::transparent( light ) );
                cairo_pattern_add_color_stop( pattern, 1.0, mixed );

                cairo_set_line_width( context, 1 );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, xl, yl-1, wl, hl, 4.5 );
                cairo_stroke( context );

                cairo_rounded_rectangle( context, xl+2, yl+1, wl-4, hl-3, 4 );
                cairo_stroke( context );
            }

            {
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, w, 0 ) );
                cairo_pattern_add_color_stop( pattern, 0, dark );
                cairo_pattern_add_color_stop( pattern, 0.1, ColorUtils::Rgba::transparent( dark ) );
                cairo_pattern_add_color_stop( pattern, 0.9, ColorUtils::Rgba::transparent( dark ) );
                cairo_pattern_add_color_stop( pattern, 1.0, dark );

                cairo_set_line_width( context, 1 );
                cairo_set_source( context, pattern );
                cairo_rounded_rectangle( context, xl+1, yl, wl-2, hl-2, 4 );
                cairo_stroke( context );
            }

            cairo_restore( context );
            drawSeparator( context, base, 0, -1, w, 2, false );
            drawSeparator( context, base, 0, h-3, w, 2, false );
        }

        return m_dockFrameCache.insert( key, TileSet( surface, 4, 4, w-8, h-8 ) );
    }

    //____________________________________________________________________
    const Cairo::Surface& StyleHelper::progressBarIndicator(const ColorUtils::Rgba& base, const ColorUtils::Rgba& highlight, int w, int h )
    {

        ProgressBarIndicatorKey key( base, highlight, w, h );

        // try find in cache and return
        if( const Cairo::Surface& surface = m_progressBarIndicatorCache.value( key ) )
        { return surface; }

        // cached not found, create new

        // local rect
        int xl = 0;
        int yl = 0;
        int wl = w+2;
        int hl = h+3;

        Cairo::Surface surface( createSurface( wl, hl ) );
        Cairo::Context context( surface );

        // adjust rect
        xl += 1;
        yl += 1;
        wl -= 2;
        hl -= 2;

        // colors
        const ColorUtils::Rgba lhighlight( ColorUtils::lightColor( highlight ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );
        const ColorUtils::Rgba shadow( ColorUtils::shadowColor( base ) );

        {
            // shadow
            cairo_rounded_rectangle( context, double(xl)+0.5, double(yl)-0.5, wl, hl+2, 2.3 );
            cairo_set_source( context, ColorUtils::alphaColor( shadow, 0.6 ) );
            cairo_set_line_width( context, 0.6 );
            cairo_stroke( context );
        }

        {
            // filling
            cairo_set_source( context, ColorUtils::mix( highlight, dark, 0.2 ) );
            cairo_rectangle( context, xl+1, yl, wl-2, hl );
            cairo_fill( context );
        }

        // create pattern pixbuf
        wl--;
        if( wl > 0 )
        {

            Cairo::Pattern mask( cairo_pattern_create_linear( 0, 0, wl, 0 ) );
            cairo_pattern_add_color_stop( mask, 0, ColorUtils::Rgba::transparent() );
            cairo_pattern_add_color_stop( mask, 0.4, ColorUtils::Rgba::black() );
            cairo_pattern_add_color_stop( mask, 0.6, ColorUtils::Rgba::black() );
            cairo_pattern_add_color_stop( mask, 1.0, ColorUtils::Rgba::transparent() );

            const ColorUtils::Rgba mix( ColorUtils::mix( lhighlight, light, 0.3 ) );
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, hl ) );
            cairo_pattern_add_color_stop( pattern, 0,  mix );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::Rgba::transparent( mix ) );
            cairo_pattern_add_color_stop( pattern, 0.6, ColorUtils::Rgba::transparent( mix ) );
            cairo_pattern_add_color_stop( pattern, 1.0, mix );

            Cairo::Surface localSurface( createSurface( wl, hl ) );
            Cairo::Context localContext( localSurface );
            cairo_rectangle( localContext, 0, 0, wl, hl );
            cairo_set_source( localContext, pattern );
            cairo_mask( localContext, mask );
            localContext.free();

            cairo_save( context );
            cairo_translate( context, 1, 1 );
            cairo_rectangle( context, 0, 0, wl, hl );
            cairo_set_source_surface( context, localSurface, 0, 0 );
            cairo_fill( context );
            cairo_restore( context );

        }

        cairo_set_antialias( context, CAIRO_ANTIALIAS_NONE );
        {
            // bevel
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, hl ) );
            cairo_pattern_add_color_stop( pattern, 0.0, lhighlight );
            cairo_pattern_add_color_stop( pattern, 0.5, highlight );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::darkColor(highlight) );
            cairo_set_line_width( context, 1.0 );
            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, xl+0.5, yl+0.5, wl, hl, 1.5 );
            cairo_stroke( context );
        }

        {
            // bright top edge
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, wl, 0 ) );
            const ColorUtils::Rgba mix( ColorUtils::mix( lhighlight, light, 0.8 ) );
            cairo_pattern_add_color_stop( pattern, 0.0, ColorUtils::Rgba::transparent( mix ) );
            cairo_pattern_add_color_stop( pattern, 0.5, mix );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::Rgba::transparent( mix ) );
            cairo_set_line_width( context, 1.0 );
            cairo_set_source( context, pattern );
            cairo_move_to( context, xl+0.5, yl+0.5 );
            cairo_line_to( context, xl+wl-0.5, yl+0.5 );
            cairo_stroke( context );
        }

        // note: we can't return the surface directly, because it is a temporary
        // we have to return the inserted object instead
        return m_progressBarIndicatorCache.insert( key, surface );

    }

    //________________________________________________________________________________________________________
    const TileSet& StyleHelper::groove( const ColorUtils::Rgba &base, double shade, int size )
    {

        const GrooveKey key( base, shade, size );
        const TileSet& tileSet( m_grooveCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int rsize( ( int )ceil( double( size ) * 3.0/7.0 ) );
        const int w( rsize*2 );
        const int h( rsize*2 );
        Cairo::Surface surface( createSurface( w, h ) );

        {

            Cairo::Context context( surface );
            cairo_translate( context, -2, -2 );
            cairo_scale( context, 6/w, 6/h );

            Cairo::Pattern pattern( inverseShadowGradient( ColorUtils::shadowColor( base ), 3, 4, 0.0 ) );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3, 3, 4, 4 );
            cairo_ellipse_negative( context, 4, 4, 2, 2 );
            cairo_fill( context );

        }

        return m_grooveCache.insert( key, TileSet( surface, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 ) );

    }

    //____________________________________________________________________
    const TileSet& StyleHelper::selection( const ColorUtils::Rgba& base, int h, bool custom )
    {

        const SelectionKey key( base, h, custom );
        const TileSet& tileSet( m_selectionCache.value( key ) );
        if( tileSet.isValid() ) return tileSet;

        const int w = 32+16;
        Cairo::Surface surface( createSurface( w, h ) );

        {

            // adjust height
            const int x = 0;
            const int y = 0;
            const double rounding( 2.5 );

            Cairo::Context context( surface );
            cairo_set_line_width( context, 1.0 );

            {
                // filling
                const int lightenAmount( custom ? 110 : 130 );
                const ColorUtils::Rgba light( base.light( lightenAmount ) );
                Cairo::Pattern pattern( cairo_pattern_create_linear( 0, y, 0, y+h ) );
                cairo_pattern_add_color_stop( pattern, 0, light );
                cairo_pattern_add_color_stop( pattern, 1, base );

                cairo_rounded_rectangle( context, x, y, w, h, rounding );
                cairo_set_source( context, pattern );
                cairo_fill( context );

                cairo_rounded_rectangle( context, double(x)+0.5, double(y)+0.5, w-1, h-1, rounding );
                cairo_set_source( context, base );
                cairo_stroke( context );
            }

            {
                // contrast
                cairo_rounded_rectangle( context, double(x)+1.5, double(y)+1.5, w-3, h-3, rounding-1 );
                cairo_set_source( context, ColorUtils::alphaColor( ColorUtils::Rgba::white(), 64.0/255 ) );
                cairo_stroke( context );
            }

        }

        return m_selectionCache.insert( key, TileSet( surface, 8, 0, 32, h ) );

    }

    //____________________________________________________________________
    void StyleHelper::renderDot( Cairo::Context& context, const ColorUtils::Rgba& base, int x, int y ) const
    {
        // Reduce diameter to make dots look more like in Oxygen-Qt
        const double diameter( 1.8 - 0.35 );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ).dark(130) );

        cairo_ellipse( context, double(x) + 1.0 - diameter/2, double(y) + 1.0 - diameter/2.0, diameter, diameter );
        cairo_set_source( context, light );
        cairo_fill( context );

        cairo_ellipse( context, double(x) + 0.5 - diameter/2, double(y) + 0.5 - diameter/2.0, diameter, diameter );
        cairo_set_source( context, dark );
        cairo_fill( context );
    }

    //______________________________________________________________________________________
    void StyleHelper::drawSlab( Cairo::Context& context, const ColorUtils::Rgba& color, double shade) const
    {

        const ColorUtils::Rgba light( ColorUtils::shade( lightColor(color), shade ) );
        const ColorUtils::Rgba base( ColorUtils::alphaColor( light, 0.85 ) );
        const ColorUtils::Rgba dark( ColorUtils::shade( darkColor(color), shade ) );

        // mask dimensions
        const double ic = 3.6 + 0.5*_slabThickness;
        const double is = 14.0 - 2.0*ic;

        // bevel, part 1
        {

            const double y( ColorUtils::luma(base) );
            const double yl( ColorUtils::luma(light) );
            const double yd( ColorUtils::luma(dark) );

            // create pattern
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 7, 0, 11 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            if (y < yl && y > yd)
            {
                // no middle when color is very light/dark
                cairo_pattern_add_color_stop( pattern, 0.5, base );
            }

            cairo_pattern_add_color_stop( pattern, 0.9, base );
            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, 3.0, 3.0, 8, 8, 3.5 );
            cairo_ellipse_negative( context, ic, ic, is, is );
            cairo_fill( context );

        }

        // bevel, part 2
        if( _slabThickness > 0.0 )
        {

            /*
            for some reason need to make ic a bit thicker
            this is likely due to differences between cairo and qpainter
            as a result, need to make the second pattern brighter
            */
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 6, 0, 19 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.9, base );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3.6, 3.6, 6.8, 6.8 );

            cairo_ellipse_negative( context, ic, ic, is, is );
            cairo_fill( context );

        }

    }

    //___________________________________________________________________________________________
    void StyleHelper::drawHole( Cairo::Context& context, const ColorUtils::Rgba& color, double shade, int r ) const
    {

        const int r2( 2*r );
        const ColorUtils::Rgba base( ColorUtils::shade( color, shade ) );
        const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor( color ), shade ) );
        const ColorUtils::Rgba dark( ColorUtils::shade( ColorUtils::darkColor( color ), shade ) );
        const ColorUtils::Rgba mid( ColorUtils::shade( ColorUtils::midColor( color ), shade ) );

        // bevel
        const double y( ColorUtils::luma( base ) );
        const double yl( ColorUtils::luma( light ) );
        const double yd( ColorUtils::luma( dark ) );

        Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 2, 0, r2-2 ) );
        cairo_pattern_add_color_stop( pattern, 0.2, dark );
        cairo_pattern_add_color_stop( pattern, 0.5, mid );
        cairo_pattern_add_color_stop( pattern, 1.0, light );
        if( y < yl && y > yd )
        {
            // no middle when color is very light/dark
            cairo_pattern_add_color_stop( pattern, 0.6, base );
        }

        cairo_set_source( context, pattern );
        cairo_ellipse( context, 3, 3, r2-6, r2-6 );
        cairo_fill( context );

    }

    //___________________________________________________________________________________________
    void StyleHelper::drawShadow( Cairo::Context& context, const ColorUtils::Rgba& base, int size) const
    {

        const double m( double(size-2)*0.5 );
        const double offset( 0.8 );
        const double k0( (m-4.0) / m );

        const double x(m+1);
        const double y(m+offset+1);

        // mask dimensions
        const double ic = 3.6 + 0.5*_slabThickness - 1;
        const double is = double(size) - 2.0*ic;

        Cairo::Pattern pattern( cairo_pattern_create_radial(x, y, m ) );
        for (int i = 0; i < 8; i++)
        {
            // sinusoidal pattern
            const double k1( (k0 * double(8 - i) + double(i)) * 0.125 );
            const double a( (cos( M_PI * i * 0.125) + 1.0) * 0.30 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( base ) );
        cairo_set_source( context, pattern );
        cairo_ellipse( context, 0, 0, size, size );
        cairo_ellipse_negative( context, ic, ic, is, is );
        cairo_fill( context );

    }

    //_______________________________________________________________________
    void StyleHelper::drawOuterGlow( Cairo::Context& context, const ColorUtils::Rgba& base, int size ) const
    {

        const double m( double(size)*0.5 );

        // mask dimensions
        const double ic = 3.6 + 0.5*_slabThickness - 1;
        const double is = double(size) - 2.0*ic;

        const double w( 4 );

        const double bias( _glowBias * double(14)/size );

        // k0 is located at w - bias from the outer edge
        const double gm( m + bias - 0.9 );
        const double k0( (m-w+bias) / gm );


        const double x(m);
        const double y(m);

        Cairo::Pattern pattern( cairo_pattern_create_radial(x, y, m ) );
        for (int i = 0; i < 8; i++)
        {
            // parabolic pattern
            const double k1( k0 + double(i)*(1.0-k0)/8.0 );
            const double a( 1.0 - sqrt(double(i)/8) );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( base ) );

        cairo_set_source( context, pattern );
        cairo_ellipse( context, 0, 0, size, size );
        cairo_ellipse_negative( context, ic, ic, is, is );
        cairo_fill( context );

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseShadow(
        Cairo::Context& context, const ColorUtils::Rgba& base,
        int pad, int size, double fuzz ) const
    {

        Cairo::Pattern pattern( inverseShadowGradient( base, pad, size, fuzz ) );

        cairo_set_source( context, pattern );
        cairo_ellipse( context, pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0 );
        cairo_fill( context );

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseGlow(
        Cairo::Context& context, const ColorUtils::Rgba& base,
        int pad, int size, int rsize ) const
    {

        const double m( double( size )*0.5 );

        const double width( 3.5 );
        const double bias( _glowBias*7.0/rsize );
        const double k0( ( m-width )/( m-bias ) );
        Cairo::Pattern pattern( cairo_pattern_create_radial( pad+m, pad+m, m-bias ) );
        for ( int i = 0; i < 8; i++ )
        {
            // inverse parabolic gradient
            double k1 = ( k0 * double( i ) + double( 8 - i ) ) * 0.125;
            double a = 1.0 - sqrt( i * 0.125 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a ) );

        }

        cairo_pattern_add_color_stop( pattern, k0, ColorUtils::Rgba::transparent( base ) );
        cairo_set_source( context, pattern );
        cairo_ellipse( context, pad, pad, size, size );
        cairo_fill( context );
        return;
    }

    //__________________________________________________________________________________________________________
    void StyleHelper::drawRoundSlab( Cairo::Context& context, const ColorUtils::Rgba& color, double shade ) const
    {

        // colors
        const ColorUtils::Rgba base( ColorUtils::shade(color, shade) );
        const ColorUtils::Rgba light( ColorUtils::shade( ColorUtils::lightColor(color), shade) );

        // bevel, part 1
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 10, 0, 18 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.9, ColorUtils::alphaColor( light, 0.85 ) );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3, 3, 15, 15 );
            cairo_fill( context );
        }

        // bevel, part 2
        if( _slabThickness > 0 )
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 7, 0, 28 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.9, base );
            cairo_set_source( context, pattern );
            cairo_ellipse( context, 3.6, 3.6, 13.8, 13.8 );
            cairo_fill( context );
        }

        // inside
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, -17, 0, 20 ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1, base );
            cairo_set_source( context, pattern );

            const double ic( 3.6 + _slabThickness );
            const double is( 21 - 2.0*ic );
            cairo_ellipse( context, ic, ic, is, is );
            cairo_fill( context );
        }

    }

    //________________________________________________________________________________________________________
    cairo_pattern_t* StyleHelper::inverseShadowGradient(
        const ColorUtils::Rgba& base,
        int pad, int size, double fuzz ) const
    {

        const double m( double(size)*0.5 );
        const double offset( 0.8 );
        const double k0( (m-2) / double(m+2.0) );

        const double x(pad+m);
        const double y(pad+m+offset);

        cairo_pattern_t* pattern( cairo_pattern_create_radial(x, y, m+2 ) );
        for (int i = 0; i < 8; i++)
        {
            // sinusoidal pattern
            const double k1( (double(8 - i) + k0 * double(i)) * 0.125 );
            const double a( (cos(3.14159 * i * 0.125) + 1.0) * 0.25 );
            cairo_pattern_add_color_stop( pattern, k1, ColorUtils::alphaColor( base, a*_shadowGain ) );
        }

        cairo_pattern_add_color_stop( pattern, k0, ColorUtils::Rgba::transparent( base ) );
        return pattern;

    }


}
