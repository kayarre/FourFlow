// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2010, Geodan, Amsterdam, the Netherlands.
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_MULTI_ALGORITHMS_EQUALS_HPP
#define BOOST_GEOMETRY_MULTI_ALGORITHMS_EQUALS_HPP


#include <boost/geometry/multi/core/is_multi.hpp>
#include <boost/geometry/multi/core/tags.hpp>
#include <boost/geometry/multi/core/geometry_id.hpp>

#include <boost/geometry/algorithms/equals.hpp>

namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace equals {


template <typename MultiPolygon1, typename MultiPolygon2>
struct multi_polygon_twice
{
    static inline bool apply(MultiPolygon1 const& multi1,
                MultiPolygon2 const& multi2)
    {
        if (boost::size(multi1) != boost::size(multi2))
        {
            return false;
        }

        typedef polygon_polygon
            <
                typename boost::range_value<MultiPolygon1>::type,
                typename boost::range_value<MultiPolygon2>::type
            > compare;

        return range_range<compare>(multi1, multi2);
    }
};

template <typename Polygon, typename MultiPolygon>
struct single_eq_multi_polygon
{
    static inline bool apply(Polygon const& polygon,
                MultiPolygon const& multi)
    {
        if (boost::size(multi) != 1)
        {
            return false;
        }

        return polygon_polygon
            <
                Polygon,
                typename boost::range_value<MultiPolygon>::type
            >::apply(polygon, multi.front());
    }
};



}} // namespace detail::equals
#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template <typename MultiPolygon1, typename MultiPolygon2>
struct equals
    <
        multi_polygon_tag, multi_polygon_tag,
        true, true,
        MultiPolygon1, MultiPolygon2,
        2
    >
    : detail::equals::multi_polygon_twice
        <
            MultiPolygon1,
            MultiPolygon2
        >
{};



template <typename Polygon, typename MultiPolygon>
struct equals
    <
        polygon_tag, multi_polygon_tag,
        false, true,
        Polygon, MultiPolygon,
        2
    >
    : detail::equals::single_eq_multi_polygon
        <
            Polygon,
            MultiPolygon
        >
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH



}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_MULTI_ALGORITHMS_EQUALS_HPP

