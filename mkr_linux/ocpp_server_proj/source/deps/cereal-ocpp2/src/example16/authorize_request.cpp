#include "../cereal/no_pretty_json.hpp"
#include <sstream>
#include "../protocol16/authorize.hpp"
#include "../protocol16/boot_notification.hpp"
//int main_authorize_test()
//{
// 
//    using namespace lgpl3::ocpp16;
//    AuthorizeRequest request;
//    request.idTag = "deadbeef";
//    std::stringstream ss;
//    {
//        cereal::JSONOutputArchive archive(ss);
//        request.serialize(archive);
//    }
//
//    std::cout << "json payload: " << ss.str() << std::endl;
//
//    AuthorizeRequest check_request;
//    {
//        cereal::JSONInputArchive archive(ss);
//        check_request.serialize(archive);
//    }
// 
//    std::cout << "Check round trip request id token: " 
//        << check_request.idTag.to_string() << std::endl;
// 
//    return 0;
//}

int main_authorize_test()
{
	using namespace lgpl3::ocpp16;
	BootNotificationRequest request;
	
	request.chargeBoxSerialNumber = "SN-deadbeef";
	request.chargePointModel = "Model-123";
	request.chargePointVendor = "Vendor-999";
	    std::stringstream ss;
	    {
	        cereal::JSONOutputArchive archive(ss);
	        request.serialize(archive);
	    }
	
		std::string json = ss.str();
	    std::cout << "json payload: " << json << std::endl;
	
		std::string rjson = "[2," + std::string("\"121898234\",\"") + request.action() +"\","+ json + "]";


		rapidjson::Document doc;
		doc.Parse(rjson.c_str());

		//assert(doc.IsObject());

		for (auto& v : doc.GetArray())
		{
			if(v.IsInt())
				printf("%d ", v.GetInt());
			else if(v.IsString())
				printf("%s ", v.GetString());
			else if (v.IsObject())
			{

			}

		}


		BootNotificationRequest check_request;
	    {
	        cereal::JSONInputArchive archive(ss);
	        check_request.serialize(archive);
	    }
	 
	    std::cout << "Check round trip request id token: " 
	        << check_request.chargePointVendor.to_string() << std::endl;
	 
	    return 0;
}
