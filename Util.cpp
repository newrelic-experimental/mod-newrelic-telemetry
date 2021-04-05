#include "Util.h"
#include <unistd.h>

std::string newrelic::Util::charToString(char *c, std::string def ) {
  if (c == nullptr)
    return def;
  else
    return std::string(c);
}

//std::string newrelic::Util::spacer = "";
//void newrelic:://Util::LogEnter(std::string location){
//  //fprintf(stderr, "[%d]newrelic:: %s%s: enter\n", getpid(), spacer.c_str(), location.c_str());
//  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "%s%s: enter", spacer.c_str(), location.c_str());
//  spacer.resize((spacer.size()-indent), ' ');
//}
//
//void newrelic:://Util::LogEnter(std::string location, std::string params){
//  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Telemetry(server_rec): enter");
//  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, buildFormat(location, "enter", params).c_str());
//  //fprintf(stderr, "[%d]newrelic:: %s%s: enter\n", getpid(), spacer.c_str(), buildFormat(location, "enter", args.c_str()));
//  spacer.resize((spacer.size()+indent), ' ');
//}
//
//void newrelic:://Util::LogExit(std::string location, std::string params){
//  spacer.resize((spacer.size()-indent), ' ');
//  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, buildFormat(location, "exit", params).c_str());
//}
//
//void newrelic:://Util::LogExit(std::string location){
//  spacer.resize((spacer.size()-indent), ' ');
//  ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "%s%s: exit", spacer.c_str(), location.c_str());
//}
//
//std::string  newrelic::Util::buildFormat(std::string location, std::string point, std::string params){
//  std::string result;
//  result.append(spacer);
//  result.append(location);
//  result.append(": ");
//  result.append(point);
//  result.append(": ");
//  result.append(params);
//  return result;
//}
