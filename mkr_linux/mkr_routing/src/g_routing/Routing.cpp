#include "Routing.hpp"


#ifdef WIN32
#include <iostream>
#include <fstream>
#else
#include <mbgl/util/event.hpp>
#include <mbgl/util/logging.hpp>
#include "mkr_mbgl_jni.hpp"
#endif
using namespace std;

namespace mkr {
namespace routing {

	int route_json_parse(const std::string& json, PRouteObj& rObj);

	class RoutingCls::Impl {
#ifndef WIN32
		std::unique_ptr<mbgl::AsyncRequest> req;
#endif
		PRouteObj rObj;
		//std::unique_ptr <RouteObj> rObj = std::make_unique <RouteObj>();
		int test;
	public:
		Impl() { test = 1; }
		~Impl()
		{
			test = 0;
			
		}
#ifndef WIN32
		void doRequest(string& url, RouteResponse_CB cb) {
			mbgl::HTTPFileSource fs;
			//const std::string& url = "http://router.project-osrm.org/route/v1/driving/13.388860,52.517037;13.397634,52.529407;13.428555,52.523219?overview=false";

			mbgl::Log::Info(mbgl::Event::Android, "route_Req URL:%s", url.c_str());
			req = fs.request(mbgl::Resource::source(url), [this, url,cb](mbgl::Response res) {
				if (res.error) {
					mbgl::Log::Info(mbgl::Event::Android, "route_Req Error:%s ", res.error->message.c_str());
					//observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(res.error->message)));
				}
				else if (res.notModified|| res.noContent) {
					mbgl::Log::Info(mbgl::Event::Android, "route_Req Not..");
					return;
				}
				
				else {

					try {
						//newTileset = parseTileJSON(*res.data, url, type, tileSize);
						//std::string str = *res.data.get();
						//Log::Info(Event::Android, "route_Req data=%s ", str.c_str());
						rObj.Clear();
						route_json_parse(*res.data, rObj);
						//decode
						//https://mapzen.com/documentation/mobility/decoding/#

						if (cb != 0)
							cb(rObj);
					}
					catch (...) {
						//observer->onSourceError(base, std::current_exception());
						return;
					}
				}
			});

		} //void route_request()

#else
		void readBinaryFile(std::string fname, std::string &fdata)
		{
			//ifstream in(fname.c_str(), ios::binary);
			ifstream in(fname.c_str());
			in.seekg(0, ios::end);
			int iSize = in.tellg();
			in.seekg(0, ios::beg);

			fdata.resize(iSize);//std::vector<char> pBuff(iSize);
			if (iSize > 0)
				in.read((char*)fdata.data(), iSize);//in.read(&pBuff[0], iSize);
			in.close();
		}

		void doRequest(string& url, RouteResponse_CB cb)
		{
			string json;
			readBinaryFile("route1.json", json);
			int size = json.size();
			if (size > 0)
			{
				rObj.cleanUp();
				route_json_parse(/* *res.data*/json, rObj);

				if (cb != 0)
					cb(rObj);
			}
		}
		RouteObj& getRouteObj() { return rObj; }
#endif

		PRouteObj& getRoutingObj(int i)
		{
			return rObj;
		}

	}; //class RoutingCls::Impl


	RoutingCls::RoutingCls()
			: impl(std::make_unique<Impl>()) {
	}

	RoutingCls::~RoutingCls() {};

	void RoutingCls::doRequest(string& url, RouteResponse_CB cb)
	{
		impl->doRequest(url,cb);
	}
	
	PRouteObj& RoutingCls::getRoutingObj(int i)
	{
		return impl->getRoutingObj(i);
	}

} // namespace routing
} // namespace mkr