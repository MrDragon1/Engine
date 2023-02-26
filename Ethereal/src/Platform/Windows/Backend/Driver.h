#pragma once

#include "DriverApi.h"

namespace Ethereal {
namespace Backend {

/*
 * Driver is the abstract class that aggregates all the operations in DriverApi, is between
 * hardware layer  and application layer, which means rendersystem can access Driver directly not
 * DriverApi.
 * */

class Driver {
public:

private:
    DriverApi* mDriverApi;

};

} // Ethereal
} // Backend
