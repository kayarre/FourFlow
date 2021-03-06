// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2008-2009, Geodan, Amsterdam, the Netherlands.
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_EXTENSIONS_GIS_IO_WKT_WRITE_WKT_HPP
#define BOOST_GEOMETRY_EXTENSIONS_GIS_IO_WKT_WRITE_WKT_HPP

#include <ostream>
#include <string>

#include <boost/concept/assert.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/geometry/algorithms/convert.hpp>
#include <boost/geometry/core/exterior_ring.hpp>
#include <boost/geometry/core/interior_rings.hpp>
#include <boost/geometry/core/ring_type.hpp>

#include <boost/geometry/geometries/concepts/check.hpp>
#include <boost/geometry/geometries/linear_ring.hpp>

#include <boost/geometry/extensions/gis/io/wkt/detail/wkt.hpp>

/*!
\defgroup wkt wkt: parse and stream WKT (Well-Known Text)
The wkt classes stream the specified geometry as \ref OGC Well Known Text (\ref WKT). It is defined for OGC geometries.
It is therefore not defined for all geometries (e.g. not for circle)
\note The implementation is independant from point type, point_xy and point_ll are supported,
as well as points with more than two coordinates.
*/

namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace wkt {

template <typename P, int I, int Count>
struct stream_coordinate
{
    template <typename Char, typename Traits>
    static inline void apply(std::basic_ostream<Char, Traits>& os, P const& p)
    {
        os << (I > 0 ? " " : "") << get<I>(p);
        stream_coordinate<P, I + 1, Count>::apply(os, p);
    }
};

template <typename P, int Count>
struct stream_coordinate<P, Count, Count>
{
    template <typename Char, typename Traits>
    static inline void apply(std::basic_ostream<Char, Traits>&, P const&)
    {}
};

struct prefix_linestring_par
{
    static inline const char* apply() { return "LINESTRING("; }
};

struct prefix_ring_par_par
{
    // Note, double parentheses are intentional, indicating WKT ring begin/end
    static inline const char* apply() { return "POLYGON(("; }
};

struct opening_parenthesis
{
    static inline const char* apply() { return "("; }
};

struct closing_parenthesis
{
    static inline const char* apply() { return ")"; }
};

struct double_closing_parenthesis
{
    static inline const char* apply() { return "))"; }
};




/*!
\brief Stream points as \ref WKT
*/
template <typename Point, typename Policy>
struct wkt_point
{
    template <typename Char, typename Traits>
    static inline void apply(std::basic_ostream<Char, Traits>& os, Point const& p)
    {
        os << Policy::apply() << "(";
        stream_coordinate<Point, 0, dimension<Point>::type::value>::apply(os, p);
        os << ")";
    }
};

/*!
\brief Stream ranges as WKT
\note policy is used to stream prefix/postfix, enabling derived classes to override this
*/
template <typename Range, typename PrefixPolicy, typename SuffixPolicy>
struct wkt_range
{
    template <typename Char, typename Traits>
    static inline void apply(std::basic_ostream<Char, Traits>& os,
                Range const& range)
    {
        typedef typename boost::range_const_iterator<Range>::type iterator_type;

        bool first = true;

        os << PrefixPolicy::apply();

        // TODO: check EMPTY here

        for (iterator_type it = boost::begin(range);
            it != boost::end(range);
            ++it)
        {
            os << (first ? "" : ",");
            stream_coordinate
                <
                    point_type, 0, dimension<point_type>::type::value
                >::apply(os, *it);
            first = false;
        }

        os << SuffixPolicy::apply();
    }

private:
    typedef typename boost::range_value<Range>::type point_type;
};

/*!
\brief Stream sequence of points as WKT-part, e.g. (1 2),(3 4)
\note Used in polygon, all multi-geometries
*/



template <typename Range>
struct wkt_sequence
    : wkt_range
        <
            Range,
            opening_parenthesis,
            closing_parenthesis
        >
{};


template <typename Polygon, typename PrefixPolicy>
struct wkt_poly
{
    template <typename Char, typename Traits>
    static inline void apply(std::basic_ostream<Char, Traits>& os,
                Polygon const& poly)
    {
        typedef typename ring_type<Polygon>::type ring;
        typedef typename boost::range_const_iterator<
                    typename interior_type<Polygon>::type>::type iterator;

        os << PrefixPolicy::apply();
        // TODO: check EMPTY here
        os << "(";
        wkt_sequence<ring>::apply(os, exterior_ring(poly));
        for (iterator it = boost::begin(interior_rings(poly));
            it != boost::end(interior_rings(poly));
            ++it)
        {
            os << ",";
            wkt_sequence<ring>::apply(os, *it);
        }
        os << ")";
    }
};


template <typename Box>
struct wkt_box
{
    typedef typename point_type<Box>::type point_type;

    template <typename Char, typename Traits>
    static inline void apply(std::basic_ostream<Char, Traits>& os,
                Box const& box)
    {
        // Convert to linear ring, then stream
        typedef linear_ring<point_type> ring_type;
        ring_type ring;
        geometry::convert(box, ring);
        os << "POLYGON(";
        wkt_sequence<ring_type>::apply(os, ring);
        os << ")";
    }

    private:

        inline wkt_box()
        {
            // Only streaming of boxes with two dimensions is support, otherwise it is a polyhedron!
            //assert_dimension<B, 2>();
        }
};

}} // namespace detail::wkt
#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch {

template <typename Tag, typename Geometry>
struct wkt {};


template <typename Point>
struct wkt<point_tag, Point>
    : detail::wkt::wkt_point
        <
            Point,
            detail::wkt::prefix_point
        >
{};


template <typename Linestring>
struct wkt<linestring_tag, Linestring>
    : detail::wkt::wkt_range
        <
            Linestring,
            detail::wkt::prefix_linestring_par,
            detail::wkt::closing_parenthesis
        >
{};


/*!
\brief Specialization to stream a box as WKT
\details A "box" does not exist in WKT.
It is therefore streamed as a polygon
*/
template <typename Box>
struct wkt<box_tag, Box>
    : detail::wkt::wkt_box<Box>
{};


/*!
\brief Specialization to stream a ring as WKT
\details A "linear_ring" does not exist in WKT.
A linear ring is equivalent to a polygon without inner rings
It is therefore streamed as a polygon
*/
template <typename Ring>
struct wkt<ring_tag, Ring>
    : detail::wkt::wkt_range
        <
            Ring,
            detail::wkt::prefix_ring_par_par,
            detail::wkt::double_closing_parenthesis
        >
{};


/*!
\brief Specialization to stream polygon as WKT
*/
template <typename Polygon>
struct wkt<polygon_tag, Polygon>
    : detail::wkt::wkt_poly
        <
            Polygon,
            detail::wkt::prefix_polygon
        >
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


/*!
\brief Generic geometry template manipulator class, takes corresponding output class from traits class
\ingroup wkt
\details Stream manipulator, streams geometry classes as \ref WKT streams
\par Example:
Small example showing how to use the wkt class
\dontinclude doxygen_1.cpp
\skip example_as_wkt_point
\line {
\until }
*/
template <typename Geometry>
class wkt_manipulator
{
public:

    inline wkt_manipulator(Geometry const& g)
        : m_geometry(g)
    {}

    template <typename Char, typename Traits>
    inline friend std::basic_ostream<Char, Traits>& operator<<(
            std::basic_ostream<Char, Traits>& os,
            wkt_manipulator const& m)
    {
        dispatch::wkt
            <
                typename tag<Geometry>::type,
                Geometry
            >::apply(os, m.m_geometry);
        os.flush();
        return os;
    }

private:
    Geometry const& m_geometry;
};

/*!
\brief Main WKT-streaming function
\ingroup wkt
\par Example:
Small example showing how to use the wkt helper function
\dontinclude doxygen_1.cpp
\skip example_as_wkt_vector
\line {
\until }
*/
template <typename Geometry>
inline wkt_manipulator<Geometry> wkt(Geometry const& geometry)
{
    concept::check<const Geometry>();

    return wkt_manipulator<Geometry>(geometry);
}


// Backward compatibility
template <typename Geometry>
inline wkt_manipulator<Geometry> make_wkt(Geometry const& geometry)
{
    concept::check<const Geometry>();

    return wkt_manipulator<Geometry>(geometry);
}

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_EXTENSIONS_GIS_IO_WKT_WRITE_WKT_HPP
