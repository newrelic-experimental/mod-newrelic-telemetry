#include "Span.h"
#include "httpd.h"
#include "http_request.h"
#include "Trace.h"
#include "http_log.h"
#include "Util.h"


namespace newrelic {
   std::string traceID;
   std::shared_ptr<Span> lastSpan = nullptr;
   std::string traceParentID;
   std::string spanParentID;

   void Trace::setTraceID(request_rec *rRec) {
     traceID.append(std::to_string(getpid()));
     traceID.append(":");
     traceID.append(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()));
   }

   void Trace::setTraceParentID(request_rec *rRec) {
     //// TODO W3C/NR DT headers
   }

   Trace::Trace() = default;

   Trace::Trace(request_rec *rRec, std::shared_ptr<SpanCommon> *spanCommon) {
     //Util::LogEnter("Trace()");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Trace(): enter");
     this->setTraceParentID(rRec);
     spanParentID = traceParentID;
     this->setTraceID(rRec);
     this->spanCommon = spanCommon;
     this->beginSpan(rRec, rootSpanName);
     //Util::LogExit("Trace()");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Trace(): exit");
   }

   void Trace::close(request_rec *rRec, const std::shared_ptr<newrelic::TraceWriter> &traceWriter) {
     this->endSpan(rRec, rootSpanName);
     traceWriter->write(spanCommon, spans);
   }

   void Trace::beginSpan(request_rec *rRec, const std::string &spanName) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "trace.beginSpan: enter spanName: %s", spanName.c_str());
     //Util::LogEnter("trace.beginSpan", f.append(spanName));
     if (lastSpan == nullptr) {
     }
     auto span = std::make_shared<newrelic::Span>(rRec, spanName, spanParentID, traceID);
     spans[spanName] = span;
     spanParentID = span->getID();
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "trace.beginSpan: exit");
     //Util::LogExit("trace.beginSpan");
   }

   void Trace::endSpan(request_rec *rRec, const std::string &spanName) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "trace.endSpan: enter spanName: %s", spanName.c_str());
     auto span = spans[spanName];
     if (span != nullptr) {
       span->close(rRec);
     } else {
       ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "    trace.endSpan: spanName: %s: NOT FOUND", spanName.c_str());
     }
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "trace.endSpan: exit spanName: %s", spanName.c_str());
   }
}