#pragma once

#include "httpd.h"
#include "http_config.h"
#include <string>

// This has to be outside of the namespace
//#if AP_MODULE_MAGIC_AT_LEAST(20111129, 0)
//APLOG_USE_MODULE(newrelic_telemetry);
//#endif

namespace newrelic {
   struct NewRelicConfiguration {
      std::string insightsInsertKey;
      std::string newRelicEventHost;
      std::string newRelicEventPath;
      std::string newRelicMetricHost;
      std::string newRelicMetricPath;
      std::string newRelicTraceHost;
      std::string newRelicTracePath;
      // TODO list of modules to trace
   };
   extern struct NewRelicConfiguration newRelicConfig;
}