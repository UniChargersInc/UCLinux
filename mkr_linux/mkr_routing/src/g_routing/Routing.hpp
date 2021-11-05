#pragma once
#ifndef WIN32
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>

#include <mbgl/util/async_task.hpp>
#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/network_status.hpp>


#endif

#include <memory>
#include <functional>


#include "HosEngine/Data/Definitions.h"

namespace mkr {
namespace routing {
	//typedef void(*RouteResponse_CB)(RouteObj& rObj);
	using RouteResponse_CB = std::function<void(PRouteObj&)>;

		class RoutingCls  {
		public:
			RoutingCls();
			~RoutingCls();

			void doRequest(std::string& url, RouteResponse_CB cb);
			PRouteObj& getRoutingObj(int i);
			

		private:
			class Impl;
			std::unique_ptr<Impl> impl;

		};

} // namespace routing
} // namespace mkr
