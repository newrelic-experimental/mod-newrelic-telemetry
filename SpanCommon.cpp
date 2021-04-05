#include "SpanCommon.h"
#include "Util.h"

using newrelic::Util;
namespace newrelic{
   SpanCommon::SpanCommon(server_rec *sRec) {
     attributes["service.name"] = std::string("Apache HTTPD");
     attributes["server.host"] = Util::charToString(sRec->server_hostname);
     attributes["virtual.host"] = Util::charToString(sRec->addrs->virthost);
     common["attributes"] = attributes;
   }

   SpanCommon::Common SpanCommon::getCommon() {
     return this->common;
   }

   SpanCommon::SpanCommon() = default;
}
