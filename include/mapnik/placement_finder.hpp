/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2011 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_PLACEMENT_FINDER_HPP
#define MAPNIK_PLACEMENT_FINDER_HPP

//mapnik
#include <mapnik/geometry.hpp>
#include <mapnik/text_properties.hpp>
#include <mapnik/text_placements/base.hpp>
#include <mapnik/symbolizer_helpers.hpp>

//stl
#include <queue>

namespace mapnik
{

class text_placement_info;
class string_info;
class text_path;


template <typename DetectorT>
class placement_finder : boost::noncopyable
{
public:
    placement_finder(Feature const& feature,
                     text_placement_info const& placement_info,
                     string_info const& info,
                     DetectorT & detector,
                     box2d<double> const& extent);

    /** Try place a single label at the given point. */
    void find_point_placement(double pos_x, double pos_y, double angle=0.0);

    /** Iterate over the given path, placing point labels with respect to label_spacing. */
    template <typename T>
    void find_point_placements(T & path);

    /** Iterate over the given path, placing line-following labels with respect to label_spacing. */
    template <typename T>
    void find_line_placements(T & path);

    /** Add placements to detector. */
    void update_detector();

    /** Remove old placements. */
    void clear_placements();

    inline placements_type &get_results() { return placements_; }

private:
    ///Helpers for find_line_placement

    ///Returns a possible placement on the given line, does not test for collisions
    //index: index of the node the current line ends on
    //distance: distance along the given index that the placement should start at, this includes the offset,
    //          as such it may be > or < the length of the current line, so this must be checked for
    //orientation: if set to != 0 the placement will be attempted with the given orientation
    //             otherwise it will autodetect the orientation.
    //             If >= 50% of the characters end up upside down, it will be retried the other way.
    //             RETURN: 1/-1 depending which way up the string ends up being.
    std::auto_ptr<text_path> get_placement_offset(const std::vector<vertex2d> & path_positions,
                                                  const std::vector<double> & path_distances,
                                                  int & orientation, unsigned index, double distance);

    ///Tests wether the given text_path be placed without a collision
    // Returns true if it can
    // NOTE: This edits p.envelopes so it can be used afterwards (you must clear it otherwise)
    bool test_placement(const std::auto_ptr<text_path> & current_placement, const int & orientation);

    ///Does a line-circle intersect calculation
    // NOTE: Follow the strict pre conditions
    // Pre Conditions: x1,y1 is within radius distance of cx,cy. x2,y2 is outside radius distance of cx,cy
    //                 This means there is exactly one intersect point
    // Result is returned in ix, iy
    void find_line_circle_intersection(
        const double &cx, const double &cy, const double &radius,
        const double &x1, const double &y1, const double &x2, const double &y2,
        double &ix, double &iy);

    void find_line_breaks();
    void init_string_size();
    void init_alignment();
    void adjust_position(text_path *current_placement, double label_x, double label_y);
    void add_line(double width, double height, bool first_line);

    ///General Internals
    DetectorT & detector_;
    box2d<double> const& dimensions_;
    string_info const& info_;
    text_symbolizer_properties const& p;
    text_placement_info const& pi;
    /** Length of the longest line after linebreaks.
      * Before find_line_breaks() this is the total length of the string.
      */
    double string_width_;
    /** Height of the string after linebreaks.
      * Before find_line_breaks() this is the total length of the string.
      */
    double string_height_;
    /** Height of the tallest font in the first line not including line spacing.
      * Used to determine the correct offset for the first line.
      */
    double first_line_space_;
    vertical_alignment_e valign_;
    horizontal_alignment_e halign_;
    std::vector<unsigned> line_breaks_;
    std::vector<std::pair<double, double> > line_sizes_;
    std::queue< box2d<double> > envelopes_;
    /** Used to return all placements found. */
    placements_type placements_;
    /** Bounding box of all texts placed. */
    box2d<double> extents_;
};
}


#endif // MAPNIK_PLACEMENT_FINDER_HPP
