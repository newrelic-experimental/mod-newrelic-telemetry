#pragma once

#include <string>
#include "httpd.h"
#include "http_log.h"
#include <stdio.h>
#include <iostream>

//// Must be outside of namespace
#if AP_MODULE_MAGIC_AT_LEAST(20111129, 0)
APLOG_USE_MODULE(newrelic_telemetry);
#endif

namespace newrelic {
   class Util {
   public:
      static std::string charToString(char *c, std::string def = "");
//      static void LogEnter(std::string location);
//      static void LogEnter(std::string location, std::string params);
//      static void LogExit(std::string location, std::string params);
//      static void LogExit(std::string location);
//   private:
//      static std::string spacer;
//      const static int indent = 3;
//      static std::string buildFormat(std::string location, std::string point, std::string params);
   };
}