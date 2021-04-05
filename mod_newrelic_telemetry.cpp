#include <map>
#include "apr_hash.h"
#include "ap_config.h"
#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "mod_newrelic_telemetry.h"
#include "Telemetry.h"
#include <unistd.h>
#include "Util.h"

namespace newrelic {
   //// Directives
   struct NewRelicConfiguration newRelicConfig;
   const char *setInsightsKey(cmd_parms *cmd, void *cfg, const char *arg) {
   newRelicConfig.insightsInsertKey = arg;
   return nullptr;
}
const char *setTraceHost(cmd_parms *cmd, void *cfg, const char *arg) {
   newRelicConfig.newRelicTraceHost = arg;
   return nullptr;
}
const char *setTracePath(cmd_parms *cmd, void *cfg, const char *arg) {
     newRelicConfig.newRelicTracePath= arg;
     return nullptr;
   }
   // NOTE: when directive functions run Apache logging is not yet active
   const command_rec registerDirectives[4] = {
     AP_INIT_TAKE1("InsightsInsertKey", reinterpret_cast<cmd_func>(setInsightsKey), nullptr, RSRC_CONF, "Insights API Insert Key"),
     AP_INIT_TAKE1("NewRelicTraceHost", reinterpret_cast<cmd_func>(setTraceHost), nullptr, RSRC_CONF, "Trace API host"),
     AP_INIT_TAKE1("NewRelicTracePath", reinterpret_cast<cmd_func>(setTracePath), nullptr, RSRC_CONF, "Trace API path"),
     {nullptr}};

   // This seems like the simplest way to track Telemetry instances- associate them with a pid
   std::map<pid_t, std::unique_ptr<Telemetry>> telemetry;
   //// Hooks
   // Called once per Apache process
   void handleProcessInit(apr_pool_t *pool, server_rec *s) {
     //Util::LogEnter("handleProcessInit");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleProcessInit: enter: pid: %d", getpid());
     // Allocate an instance of our monitor on this thread
     telemetry[getpid()] = std::make_unique<Telemetry>(s);
     //Util::LogExit("handleProcessInit");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleProcessInit: exit");
   }

   // Called at the start of a request
   int handleTraceStart(request_rec *r) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleTraceStart: enter: pid: %d", getpid());
     //Util::LogEnter("handleTraceStart");
     // Dispatch to our instance based on thread
     telemetry.at(getpid())->beginTrace(r);
     //Util::LogExit("handleTraceStart");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleTraceStart: exit");
     return DECLINED;
   }

   int handleModuleStart(request_rec *r){
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleModuleStart: enter: pid: %d", getpid());
     //Util::LogEnter("handleModuleStart");
     // TODO Figure-out how to know what module we're called before
     telemetry.at(getpid())->beginSpan(r, "mod_heartmonitor");
     //Util::LogExit("handleModuleStart");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleModuleStart: exit: pid: %d", getpid());
     return DECLINED;
   }

   int handleModuleEnd(request_rec *r){
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleModuleEnd: enter: pid: %d", getpid());
     //Util::LogEnter("handleModuleEnd");
     // TODO Figure-out how to know what module we're called after
     telemetry.at(getpid())->endSpan(r, "mod_heartmonitor");
     //Util::LogExit("handleModuleEnd");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleModuleEnd: exit: pid: %d", getpid());
     return DECLINED;
   }

   // Called at the end of a request
   int handleTraceEnd(request_rec *r) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleTraceEnd: enter: pid: %d", getpid());
     //Util::LogEnter("handleTraceEnd");
     // Dispatch to our instance based on thread
     telemetry.at(getpid())->endTrace(r);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "handleTraceEnd: exit");
     //Util::LogExit("handleTraceEnd");
     return DECLINED;
   }

   // Called once when the module is loaded
   void registerHooks(apr_pool_t *pool) {
     // TODO set predecessor & successor hooks for tracing other modules specified by configuration
     static const char * const aszPred[]={ "mod_heartmonitor.c", nullptr };

     // We can install more than one hook on the same event
     // If there's no hook "meta data" then we're in trouble as we cannot set the hook handler with a pointer to a handler inside an instance
     newRelicConfig.newRelicEventHost = "insights-collector.newrelic.com";
     newRelicConfig.newRelicEventPath = "/metric/v1";
     newRelicConfig.newRelicMetricHost = "metric-api.newrelic.com";
     newRelicConfig.newRelicMetricPath = "/metric/v1";
     newRelicConfig.newRelicTraceHost = "trace-api.newrelic.com";
     newRelicConfig.newRelicTracePath = "/trace/v1";
     ap_hook_child_init(handleProcessInit, nullptr, nullptr, APR_HOOK_FIRST);
     ap_hook_post_read_request(handleTraceStart, nullptr, nullptr, APR_HOOK_REALLY_FIRST);
     ap_hook_post_read_request(handleModuleStart, aszPred, nullptr, APR_HOOK_FIRST);
     ap_hook_post_read_request(handleModuleEnd, nullptr, aszPred, APR_HOOK_FIRST);
     ap_hook_log_transaction(handleTraceEnd, nullptr, nullptr, APR_HOOK_REALLY_LAST);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "registerHooks: module loaded");
   }
}

// This has to be outside of the namespace
//// Module definition
module AP_MODULE_DECLARE_DATA newrelic_telemetry_module = {STANDARD20_MODULE_STUFF, nullptr,                  /* create per-dir    config structures */
  nullptr,                  /* merge  per-dir    config structures */
  nullptr,                  /* create per-server config structures */
  nullptr,                  /* merge  per-server config structures */
  newrelic::registerDirectives,                  /* table of config file commands       */
  newrelic::registerHooks  /* register hooks                      */
};