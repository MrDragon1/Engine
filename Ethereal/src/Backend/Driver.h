#pragma once

#include "DriverApi.h"

namespace Ethereal {
namespace Backend {
/*
 * Driver is the abstract class that aggregates all the operations in DriverApi, is between
 * hardware layer and application layer, which means RenderSystem can access Driver directly not
 * DriverApi.
 * */
class Driver {
   public:
    Driver(BackendType type) { mDriverApi = DriverApi::CreateApi(type); }
    Ref<DriverApi> GetApi() { return mDriverApi; }

   private:
    Ref<DriverApi> mDriverApi;
};
}  // namespace Backend
}  // namespace Ethereal
