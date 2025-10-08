#ifndef ROUTER_H
#define ROUTER_H

#include "geodb.h"
#include "base_classes.h"


class Router: public RouterBase
{
public:
    Router(const GeoDatabaseBase& geo_db);
    virtual ~Router();
    virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;

private:
    const GeoDatabaseBase* ptrToGDB;

    struct GeoPos {
        GeoPoint position;
        double f_cost;

        GeoPos(const GeoPoint& pos, const double& cost)
        : position(pos), f_cost(cost) {}
    };

    struct ComparePos {
        bool operator()(const GeoPos& lhs, const GeoPos& rhs) {
            return lhs.f_cost > rhs.f_cost;
        }
    };
};

#endif //ROUTER_H
