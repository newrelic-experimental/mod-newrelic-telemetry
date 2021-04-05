#include "ModuleMonitor.h"
namespace newrelic{
   ModuleMonitor::ModuleMonitor(const char *spanName) {
     this->spanName = spanName;
   }

   int ModuleMonitor::handlePostReadRequest(request_rec *r){
     return DECLINED;
}
}
