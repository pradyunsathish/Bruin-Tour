#ifndef GEODB_H
#define GEODB_H

#include <istream>

#include "base_classes.h"
#include "geopoint.h"
#include "hashmap.h"

using namespace std;

class GeoDatabase: public GeoDatabaseBase
{
public:
    GeoDatabase();
    virtual ~GeoDatabase();
    virtual bool load(const std::string& map_data_file);
    virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
    virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
    virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;

private:
    struct Segment {
        string m_streetName;
        GeoPoint m_start;
        GeoPoint m_end;

        Segment(string name, const GeoPoint& start, const GeoPoint& end)
        : m_streetName(name), m_start(start), m_end(end) {}
    };

    HashMap<Segment> streetSegments;
    HashMap<vector<GeoPoint>> connectedPoints;
    HashMap<GeoPoint> poiLocations;
};

#endif //GEODB_H
