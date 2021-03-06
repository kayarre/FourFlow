// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_GEOMETRY_GEOMETRIES_CONCEPTS_RING_CONCEPT_HPP
#define BOOST_GEOMETRY_GEOMETRIES_CONCEPTS_RING_CONCEPT_HPP


#include <boost/concept_check.hpp>
#include <boost/range/concepts.hpp>
#include <boost/type_traits/remove_const.hpp>

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/point_type.hpp>

#include <boost/geometry/geometries/concepts/point_concept.hpp>

#include <boost/geometry/geometries/concepts/detail/check_clear.hpp>
#include <boost/geometry/geometries/concepts/detail/check_append.hpp>


namespace boost { namespace geometry { namespace concept {


/*!
\brief ring concept
\ingroup concepts
\par Formal definition:
The ring concept is defined as following:
- there must be a specialization of traits::tag defining ring_tag as type
- it must behave like a Boost.Range
- there can optionally be a specialization of traits::point_order defining the
  order or orientation of its points, clockwise or counterclockwise.
- either it can behave like the std library, having pushback
- or it can implement a mechanism for clearing and adding points.
  This is the same as the for the concept Linestring, and described there.

\note to fulfil the concepts, no traits class has to be specialized to
define the point type. The point type is taken using boost::range_value<X>::type
*/
template <typename Geometry>
class Ring
{
#ifndef DOXYGEN_NO_CONCEPT_MEMBERS
    typedef typename point_type<Geometry>::type point_type;

    BOOST_CONCEPT_ASSERT( (concept::Point<point_type>) );
    BOOST_CONCEPT_ASSERT( (boost::RandomAccessRangeConcept<Geometry>) );


public :

    BOOST_CONCEPT_USAGE(Ring)
    {
        // Check if it can be modified
        static const bool use_std = traits::use_std
            <
                typename boost::remove_const<Geometry>::type
            >::value;

        Geometry* ring;
        detail::check_clear<Geometry, use_std>::apply(*ring);

        point_type* p;
        detail::check_append<Geometry, point_type, use_std>::apply(*ring, *p);
    }
#endif
};


/*!
\brief (linear) ring concept (const version)
\ingroup const_concepts
\details The ConstLinearRing concept check the same as the Geometry concept,
but does not check write access.
*/
template <typename Geometry>
class ConstRing
{
#ifndef DOXYGEN_NO_CONCEPT_MEMBERS
    typedef typename point_type<Geometry>::type point_type;

    BOOST_CONCEPT_ASSERT( (concept::ConstPoint<point_type>) );
    BOOST_CONCEPT_ASSERT( (boost::RandomAccessRangeConcept<Geometry>) );


public :

    BOOST_CONCEPT_USAGE(ConstRing)
    {
    }
#endif
};

}}} // namespace boost::geometry::concept


#endif // BOOST_GEOMETRY_GEOMETRIES_CONCEPTS_RING_CONCEPT_HPP
