#include "tour_generator.h"
#include "geotools.h"

using namespace std;

TourGenerator::TourGenerator(const GeoDatabaseBase &geodb, const RouterBase &router)
    : ptrToGDB(&geodb), ptrToRouter(&router) {}

TourGenerator::~TourGenerator() {}


string TourGenerator::findDirection(double angle) const {
     string dir;

    if (angle >= 22.5 && angle < 67.5)
        dir = "northeast";
    else if (angle >= 67.5 && angle < 112.5)
        dir = "north";
    else if (angle >= 112.5 && angle < 157.5)
        dir = "northwest";
    else if (angle >= 157.5 && angle < 202.5)
        dir = "west";
    else if (angle >= 202.5 && angle < 247.5)
        dir = "southwest";
    else if (angle >= 247.5 && angle < 292.5)
        dir = "south";
    else if (angle >= 292.5 && angle < 337.5)
        dir = "southeast";
    else
        dir = "east";

    return dir;
}


vector<TourCommand> TourGenerator::generate_tour(const Stops &stops) const {
    vector<TourCommand> tourCmds;

    GeoPoint start, end;

    for (int i = 0; i < stops.size(); i++) {
        string poi;
        string description;
        stops.get_poi_data(i, poi, description);

        TourCommand commentaryCmd;
        commentaryCmd.init_commentary(poi, description);
        tourCmds.push_back(commentaryCmd);

        if (i < stops.size() - 1) {
            string currPoi, nextPoi;
            GeoPoint currGP, nextGP;

            stops.get_poi_data(i, currPoi, description);
            stops.get_poi_data(i + 1, nextPoi, description);

            ptrToGDB->get_poi_location(currPoi, currGP);
            ptrToGDB->get_poi_location(nextPoi, nextGP);

            vector<GeoPoint> route = ptrToRouter->route(currGP, nextGP);

            for (size_t k = 0; k < route.size() - 1; k++) {
                start = route[k];
                end = route[k + 1];

                double dist = distance_earth_miles(start, end);
                string streetName = ptrToGDB->get_street_name(start, end);

                double angle = angle_of_line(start, end);
                string dir = findDirection(angle);

                TourCommand moveCmd;
                moveCmd.init_proceed(dir, streetName, dist, start, end);
                tourCmds.push_back(moveCmd);

                if (k < route.size() - 2) {
                    GeoPoint nextLocation = route[k + 2];
                    string nextStreet = ptrToGDB->get_street_name(end, nextLocation);
                    double turnAngle = angle_of_turn(start, end, nextLocation);

                    if (streetName != nextStreet && turnAngle >= 1 && turnAngle < 180) {
                        TourCommand turnCmd;
                        turnCmd.init_turn("left", nextStreet);
                        tourCmds.push_back(turnCmd);
                    } else if (streetName != nextStreet && turnAngle >= 180 && turnAngle <= 359) {
                        TourCommand turnCmd;
                        turnCmd.init_turn("right", nextStreet);
                        tourCmds.push_back(turnCmd);
                    }

                }

            }

        }

    }

    return tourCmds;
}