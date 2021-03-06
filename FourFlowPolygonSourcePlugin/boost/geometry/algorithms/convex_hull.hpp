// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_CONVEX_HULL_HPP
#define BOOST_GEOMETRY_ALGORITHMS_CONVEX_HULL_HPP


#include <boost/type_traits/remove_const.hpp>

#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/is_multi.hpp>
#include <boost/geometry/core/point_order.hpp>
#include <boost/geometry/core/exterior_ring.hpp>

#include <boost/geometry/geometries/concepts/check.hpp>

#include <boost/geometry/iterators/range_type.hpp>

#include <boost/geometry/strategies/convex_hull.hpp>
#include <boost/geometry/strategies/concepts/convex_hull_concept.hpp>

#include <boost/geometry/util/as_range.hpp>


/*!
\defgroup convex_hull convex hull: calculate the convex hull of a geometry
\par Source descriptions:
- OGC description: Returns a geometric object that represents the convex hull of
    this geometric object. Convex hulls, being dependent on straight lines, can
    be accurately represented in linear interpolations
    for any geometry restricted to linear interpolations.
\see http://en.wikipedia.org/wiki/Convex_hull

\par Performance
2776 counties of US are "hulled" in 0.9 seconds
(http://trac.osgeo.org/ggl/wiki/Performance#Convexhull1)

\note The convex hull is always a ring, holes are not possible. Therefore it is
    can also be used in combination with an output iterator.

\par Geometries supported:
In the images below the convex hull is painted in red.

- \b point: will not compile

- \b linestring:

- \b polygon: will deliver a polygon without holes
    \image html svg_convex_hull_country.png

- \b multi_point:
    \image html svg_convex_hull_cities.png

- \b multi_linestring:

- \b multi_polygon:

\par Output geometries supported:

- \b polygon

- \b ring

- inserter version (with output iterator) can output to any array supporting
    points of same type as the input geometry type

*/
namespace boost { namespace geometry {

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace convex_hull {

template
<
    typename Geometry,
    order_selector Order,
    typename Strategy
>
struct hull_inserter
{

    // Member template function, to avoid inconvenient declaration
    // of output-iterator-type, from hull_to_geometry
    template <typename OutputIterator>
    static inline OutputIterator apply(Geometry const& geometry,
            OutputIterator out, Strategy const& strategy)
    {
        typename Strategy::state_type state;

        strategy.apply(geometry, state);
        strategy.result(state, out, Order == clockwise);
        return out;
    }
};

template
<
    typename Geometry,
    typename OutputGeometry,
    typename Strategy
>
struct hull_to_geometry
{
    static inline void apply(Geometry const& geometry, OutputGeometry& out,
            Strategy const& strategy)
    {
        hull_inserter
            <
                Geometry,
                geometry::point_order<OutputGeometry>::value,
                Strategy
            >::apply(geometry,
                std::back_inserter(
                    // Handle both ring and polygon the same:
                    geometry::as_range
                        <
                            typename geometry::range_type<OutputGeometry>::type
                        >(out)), strategy);
    }
};


}} // namespace detail::convex_hull
#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template
<
    typename Tag1,
    bool IsMulti,
    typename Geometry,
    typename Output,
    typename Strategy
>
struct convex_hull
    : detail::convex_hull::hull_to_geometry<Geometry, Output, Strategy>
{};


template
<
    typename GeometryTag,
    order_selector Order,
    bool IsMulti,
    typename GeometryIn, typename Strategy
 >
struct convex_hull_inserter
    : detail::convex_hull::hull_inserter<GeometryIn, Order, Strategy>
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


template<typename Geometry1, typename Geometry2, typename Strategy>
inline void convex_hull(Geometry1 const& geometry,
            Geometry2& out, Strategy const& strategy)
{
    concept::check_concepts_and_equal_dimensions
        <
            const Geometry1,
            Geometry2
        >();

    BOOST_CONCEPT_ASSERT( (geometry::concept::ConvexHullStrategy<Strategy>) );


    dispatch::convex_hull
        <
            typename tag<Geometry1>::type,
            is_multi<Geometry1>::type::value,
            Geometry1,
            Geometry2,
            Strategy
        >::apply(geometry, out, strategy);
}


/*!
    \brief Calculate the convex hull of a geometry
    \ingroup convex_hull
    \tparam Geometry1 the input geometry type
    \tparam Geometry2: the output geometry type
    \param geometry the geometry to calculate convex hull from
    \param out a geometry receiving points of the convex hull
 */
template<typename Geometry1, typename Geometry2>
inline void convex_hull(Geometry1 const& geometry,
            Geometry2& out)
{
    concept::check_concepts_and_equal_dimensions
        <
            const Geometry1,
            Geometry2
        >();

    //typedef typename range_type<Geometry1>::type range_type;
    typedef typename point_type<Geometry2>::type point_type;

    typedef typename strategy_convex_hull
        <
            typename cs_tag<point_type>::type,
            Geometry1,
            point_type
        >::type strategy_type;

    convex_hull(geometry, out, strategy_type());
}


template<typename Geometry, typename OutputIterator, typename Strategy>
inline OutputIterator convex_hull_inserter(Geometry const& geometry,
            OutputIterator out, Strategy const& strategy)
{
    // Concept: output point type = point type of input geometry
    concept::check<const Geometry>();
    concept::check<typename point_type<Geometry>::type>();

    BOOST_CONCEPT_ASSERT( (geometry::concept::ConvexHullStrategy<Strategy>) );

    return dispatch::convex_hull_inserter
        <
            typename tag<Geometry>::type,
            geometry::point_order<Geometry>::value,
            is_multi<Geometry>::type::value,
            Geometry, Strategy
        >::apply(geometry, out, strategy);
}


/*!
    \brief Calculate the convex hull of a geometry, output-iterator version
    \ingroup convex_hull
    \tparam Geometry the input geometry type
    \tparam OutputIterator: an output-iterator
    \param geometry the geometry to calculate convex hull from
    \param out an output iterator outputing points of the convex hull
    \note This overloaded version outputs to an output iterator.
    In this case, nothing is known about its point-type or
        about its clockwise order. Therefore, the input point-type
        and order are copied

 */
template<typename Geometry, typename OutputIterator>
inline OutputIterator convex_hull_inserter(Geometry const& geometry,
            OutputIterator out)
{
    // Concept: output point type = point type of input geometry
    concept::check<const Geometry>();
    concept::check<typename point_type<Geometry>::type>();

    typedef typename range_type<Geometry>::type range_type;
    typedef typename point_type<Geometry>::type point_type;

    typedef typename strategy_convex_hull
        <
            typename cs_tag<point_type>::type,
            Geometry,
            point_type
        >::type strategy_type;

    return convex_hull_inserter(geometry, out, strategy_type());
}


}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_CONVEX_HULL_HPP
