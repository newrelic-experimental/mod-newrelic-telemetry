#pragma once
#include "mod_newrelic_telemetry.h"
#include "Span.h"
#include "SpanCommon.h"
#include "TraceWriter.h"
#include <string>

namespace newrelic{
class Trace{
private:
   Trace();
   std::shared_ptr<SpanCommon> *spanCommon{};
   const std::string rootSpanName = "Apache HTTPD";
   std::map<std::string, std::shared_ptr<Span>> spans{};
   std::string traceID;
   std::string parentID;
   void setTraceID(request_rec *pRec);
   void setTraceParentID(request_rec *pRec);
   std::shared_ptr<Span> lastSpan;
public:
   explicit Trace(request_rec *rRec, std::shared_ptr<SpanCommon> *spanCommon);
   void beginSpan(request_rec *rRec, const std::string& spanName);
   void endSpan(request_rec *rRec, const std::string& spanName);
   void close(request_rec *rRec, const std::shared_ptr<newrelic::TraceWriter>& traceWriter);
};
}