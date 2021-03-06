// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHM_REMOVE_HOLES_IF_HPP
#define BOOST_GEOMETRY_ALGORITHM_REMOVE_HOLES_IF_HPP

#include <algorithm>

#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/perimeter.hpp>

#include <boost/geometry/core/interior_rings.hpp>

#include <boost/geometry/geometries/concepts/check.hpp>

#include <boost/geometry/multi/core/tags.hpp>
#include <boost/geometry/multi/algorithms/detail/modify_with_predicate.hpp>


namespace boost { namespace geometry
{



#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace remove_holes_if {


template<typename Polygon, typename Predicate>
struct polygon_remove_holes_if
{
    static inline void apply(Polygon& poly, Predicate const& predicate)
    {
        typename interior_type<Polygon>::type& rings = interior_rings(poly);

        // Remove rings using erase-remove-idiom
        // http://en.wikipedia.org/wiki/Erase-remove_idiom
        rings.erase(
            std::remove_if(boost::begin(rings), boost::end(rings), predicate),
            boost::end(rings));
    }
};

}} // namespace detail::remove_holes_if


#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch {

// Default implementation does nothing
template <typename Tag, typename Geometry, typename Predicate>
struct remove_holes_if
{};



template <typename Geometry, typename Predicate>
struct remove_holes_if<polygon_tag, Geometry, Predicate>
    : detail::remove_holes_if::polygon_remove_holes_if<Geometry, Predicate>
{};


template <typename MultiPolygon, typename Predicate>
struct remove_holes_if<multi_polygon_tag, MultiPolygon, Predicate>
    : detail::multi_modify_with_predicate
        <
            MultiPolygon,
            Predicate,
            detail::remove_holes_if::polygon_remove_holes_if
                <
                    typename boost::range_value<MultiPolygon>::type, Predicate
                >
        >
{};



} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH

/*!
    \brief Remove holes from a geometry (polygon, multi-polygon) using a specified condition
 */
template <typename Geometry, typename Predicate>
inline void remove_holes_if(Geometry& geometry, Predicate const& predicate)
{
    concept::check<Geometry>();

    dispatch::remove_holes_if
        <
            typename tag<Geometry>::type,
            Geometry,
            Predicate
        >::apply(geometry, predicate);
}







// CONVENIENT PREDICATES might be moved elsewhere
template <typename Ring>
struct elongated_hole
{
    inline elongated_hole(double ratio)
        : m_ratio(ratio)
    {}

    inline bool operator()(Ring const& ring) const
    {
        if (ring.size() >= 4)
        {
            double a = area(ring);
            double p = perimeter(ring);
            return std::abs(a/p) < m_ratio;
        }
        // Rings with less then 4 points (including closing)
        // are also considered as small and thus removed
        return true;
    }
private :
    double m_ratio;
};


template <typename Ring>
struct invalid_hole
{
    inline bool operator()(Ring const& ring) const
    {
        return ring.size() < 4;
    }
};


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHM_REMOVE_HOLES_IF_HPP
