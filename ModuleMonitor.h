#pragma once

#include <http_protocol.h>

namespace newrelic{
class ModuleMonitor {

public:
   explicit ModuleMonitor(const char *);
   int handlePostReadRequest(request_rec *r);
   ap_HOOK_post_read_request_t *handlerPtr;

private:
   const char *spanName;
};
}