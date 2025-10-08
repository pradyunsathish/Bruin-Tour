#include "geodb.h"
#include "geotools.h"
#include <istream>
#include <sstream>

using namespace std;

GeoDatabase::GeoDatabase()
{}

GeoDatabase::~GeoDatabase() {}


bool GeoDatabase::load(const std::string &map_data_file) {

    ifstream infile(map_data_file);

    string line;
    while (getline(infile, line)) { // O()

        istringstream first(line);
        string streetName = line;


        if (!getline(infile, line)) {
            cerr << "BAD LINE: " << line << endl;
            continue;
        }

        istringstream second(line);
        string startLat, startLong, endLat, endLong;
        if (!(second >> startLat >> startLong >> endLat >> endLong)) {
            cerr << "BAD LINE: " << line << endl;
            continue;
        }

        string coordinateLine = line;
        string reverseCoordinateLine = endLat + " " + endLong + " " + startLat + " " + startLong;
        GeoPoint start(startLat, startLong);
        GeoPoint end(endLat, endLong);

        //convert to lines
        string startLine = start.sLatitude + " " + start.sLongitude;
        string endLine = end.sLatitude + " " + end.sLongitude;

        if (!getline(infile, line)) {
            cerr << "BAD LINE: " << line << endl;
            continue;
        }

        istringstream third(line);
        int numOfPois;
        if (!(third >> numOfPois)) {
            cerr << "BAD LINE: " << line << endl;
            continue;
        }


        //create segments for street and connect
        Segment street(streetName, start, end);
        Segment reverseStreet(streetName, end, start);
        streetSegments.insert(coordinateLine, street);
        streetSegments.insert(reverseCoordinateLine, reverseStreet);
        connectedPoints[startLine].push_back(end);
        connectedPoints[endLine].push_back(start);


        //handle if poi
        if (numOfPois > 0) {

            //find midpoint
            GeoPoint midpointCoordinate = midpoint(start, end);

            //convert to line
            string midpointLine = midpointCoordinate.sLatitude + " " + midpointCoordinate.sLongitude;

            //segment street start to midpoint
            string startToMidLine = startLine + " " + midpointLine;
            string midToStartLine = midpointLine + " " + startLine;
            streetSegments.insert(startToMidLine, Segment(streetName, start, midpointCoordinate));
            streetSegments.insert(midToStartLine, Segment(streetName, midpointCoordinate, start));
            connectedPoints[startLine].push_back(midpointCoordinate);
            connectedPoints[midpointLine].push_back(start);

            //segment street end to midpoint
            string endToMidLine = endLine + " " + midpointLine;
            string midToEndLine = midpointLine + " " + endLine;
            streetSegments.insert(endToMidLine, Segment(streetName, end, midpointCoordinate));
            streetSegments.insert(midToEndLine, Segment(streetName, midpointCoordinate, end));
            connectedPoints[endLine].push_back(midpointCoordinate);
            connectedPoints[midpointLine].push_back(end);

            for (int i = 0; i < numOfPois; i++) {
                if (!getline(infile, line)) {
                    cerr << "BAD LINE: " << line << endl;
                    continue;
                }

                istringstream poiLine(line);
                string poiName;
                string poiLat, poiLong;
                if (!(getline(poiLine, poiName, '|') >> poiLat >> poiLong)) {
                    cerr << "BAD LINE: " << line << endl;
                    continue;
                }

                GeoPoint poiCoordinate(poiLat, poiLong);
                string poiCoordinateLine = poiLat + " " + poiLong;
                string midToPoi = midpointLine + " " + poiCoordinateLine;
                string poiToMid = poiCoordinateLine + " " + midpointLine;
                poiLocations.insert(poiName, poiCoordinate);

                streetSegments.insert(midToPoi, Segment("a path", midpointCoordinate, poiCoordinate));
                streetSegments.insert(poiToMid, Segment("a path", poiCoordinate, midpointCoordinate));
                connectedPoints[poiCoordinateLine].push_back(midpointCoordinate);
                connectedPoints[midpointLine].push_back(poiCoordinate);
            }



        }


    }

    return true;

}


bool GeoDatabase::get_poi_location(const std::string &poi, GeoPoint &point) const {
    const GeoPoint* location = poiLocations.find(poi);
    if (location != nullptr) {
        point = *location;
        return true;
    }

    return false;
}



std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint &pt) const {

    string line = pt.sLatitude + " " + pt.sLongitude;
    const vector<GeoPoint>* ptrToConnections = connectedPoints.find(line); // O(1)

    vector<GeoPoint> listOfConnections;

    if (ptrToConnections != nullptr) {
        listOfConnections = * ptrToConnections;
    }

    return listOfConnections;
}


std::string GeoDatabase::get_street_name(const GeoPoint &pt1, const GeoPoint &pt2) const {
    string coordinateString = pt1.sLatitude + " " + pt1.sLongitude + " " + pt2.sLatitude + " " + pt2.sLongitude;

    const Segment* currentSegment = streetSegments.find(coordinateString);
    if (currentSegment != nullptr) {
        return currentSegment->m_streetName;
    }

    return "";
}