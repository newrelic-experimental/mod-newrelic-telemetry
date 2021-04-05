#include "Telemetry.h"
#include "Trace.h"
#include "Util.h"
#include "SpanCommon.h"

using newrelic::Util;
using newrelic::SpanCommon;

//// One instance of Telemetry per process. Manage all "process singletons" here
Telemetry::Telemetry() = default;
Telemetry::Telemetry(server_rec *sRec) {
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Telemetry(server_rec): enter");
  //Util::LogEnter("Telemetry(server_rec)");
  spanCommon = std::make_shared<newrelic::SpanCommon>(sRec);
  traceWriter = std::make_shared<newrelic::TraceWriter>();
  //Util::LogExit("Telemetry(server_rec)");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Telemetry(server_rec): exit");
}

void Telemetry::beginTrace(request_rec *rRec) {
  //// TODO where to add the parent.id header if we're the start?
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.beginTrace: enter");
  //Util::LogEnter("telemetry.beginTrace");
  traces[rRec] = std::make_unique<newrelic::Trace>(rRec, &spanCommon);
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.beginTrace: exit");
  //Util::LogExit("telemetry.beginTrace");
//  this->beginSpan(rRec, rRec->uri);
}

void Telemetry::endTrace(request_rec *rRec) {
  //Util::LogEnter("telemetry.endTrace");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.endTrace: enter");
  traces[rRec]->close(rRec, traceWriter);
  traces.clear();
  //Util::LogExit("telemetry.endTrace");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.endTrace: exit");
}

void Telemetry::beginSpan(request_rec *rRec, std::string spanName) {
  //Util::LogEnter("telemetry.beginSpan");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.beginSpan: enter: spanName: %s", spanName.c_str());
  traces[rRec]->beginSpan(rRec, spanName);
  //Util::LogExit("telemetry.beginSpan");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.beginSpan: exit");
}

void Telemetry::endSpan(request_rec *rRec, std::string spanName) {
  //Util::LogEnter( "telemetry.endSpan");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.endSpan: enter: spanName: %s", spanName.c_str());
  traces[rRec]->endSpan(rRec, spanName);
  //Util::LogExit( "telemetry.endSpan");
  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "telemetry.endSpan: exit");
}