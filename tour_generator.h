#ifndef TOUR_GENERATOR_H
#define TOUR_GENERATOR_H

#include "geodb.h"
#include "base_classes.h"

class TourGenerator: public TourGeneratorBase
{
public:
    TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router);
    virtual ~TourGenerator();
    virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;

private:
    const GeoDatabaseBase* ptrToGDB;
    const RouterBase* ptrToRouter;

    string findDirection(double angle) const;
};

#endif //TOUR_GENERATOR_H
