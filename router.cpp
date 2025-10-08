#include "router.h"
#include "geotools.h"
#include <queue>

using namespace std;

Router::Router(const GeoDatabaseBase &geo_db)
    : ptrToGDB(&geo_db) {}


Router::~Router() {}


vector<GeoPoint> Router::route(const GeoPoint &pt1, const GeoPoint &pt2) const {
    priority_queue<GeoPos, vector<GeoPos>, ComparePos> openList;
    HashMap<GeoPoint> closedList;
    HashMap<GeoPoint> locationOfPrevPoint;

    openList.push(GeoPos(pt1, 0.0));
    while (!openList.empty()) {     // O(N)
        GeoPos curr = openList.top();
        openList.pop();

        vector<GeoPoint> routeTrack;
        if (curr.position.sLatitude == pt2.sLatitude && curr.position.sLongitude == pt2.sLongitude) {

            GeoPoint previous = pt2;

            while (previous.sLatitude != pt1.sLatitude || previous.sLongitude != pt1.sLongitude) { // O(N)
                routeTrack.push_back(previous);
                string prevCoordinateLine = previous.sLatitude + " " + previous.sLongitude;
                const GeoPoint* prevPtr = locationOfPrevPoint.find(prevCoordinateLine);
                previous = *prevPtr;
            }

            routeTrack.push_back(pt1);
            reverse(routeTrack.begin(), routeTrack.end());
            return routeTrack;

        }

        vector<GeoPoint> positionsNear = ptrToGDB->get_connected_points(curr.position);

        size_t i = 0;
        while (i < positionsNear.size()) { // o(nlogn)
            string positionLine = positionsNear[i].sLatitude + " " + positionsNear[i].sLongitude;
            GeoPoint* checkIfExplored = closedList.find(positionLine);
            if (checkIfExplored == nullptr) {
                closedList.insert(positionLine, positionsNear[i]);
                double f = distance_earth_km(positionsNear[i], pt1) + distance_earth_km(positionsNear[i], pt2);
                locationOfPrevPoint.insert(positionLine, curr.position);
                openList.push(GeoPos(positionsNear[i], f));
            }
            i++;
        }

    }

    return vector<GeoPoint>();
}