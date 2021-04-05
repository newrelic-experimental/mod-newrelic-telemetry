#pragma once
#include <cstdio>
#include <map>
#include <string>
#include "apr_hash.h"
#include "ap_config.h"
#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "mod_newrelic_telemetry.h"
#include "Trace.h"
#include "SpanCommon.h"
#include "TraceWriter.h"

class Telemetry {
private:
   Telemetry();
   std::map<request_rec *, std::shared_ptr<newrelic::Trace>> traces {};
   std::shared_ptr<newrelic::SpanCommon> spanCommon;
   std::shared_ptr<newrelic::TraceWriter> traceWriter;
public:
   Telemetry(Telemetry const &) = delete;
   void operator=(Telemetry const &) = delete;
   explicit Telemetry(server_rec *pRec);
   void beginTrace(request_rec *rRec);
   void endTrace(request_rec *rRec);
   void beginSpan(request_rec *rRec, std::string spanName);
   void endSpan(request_rec *rRec, std::string spanName);
};