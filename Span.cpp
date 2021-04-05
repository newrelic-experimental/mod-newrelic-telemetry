#include "Span.h"
#include "Util.h"

namespace newrelic {
   std::string id;
   Span::Span(request_rec *rRec, const std::string& name, const std::string& parentID, const std::string& traceID) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Span() enter: name: %s", name.c_str());
     //Util::LogEnter("Span()");
     // TODO is there a better way to deal with this?
     if(!parentID.empty()) {
       attributes["parent.id"] = parentID;
     }
     attributes["name"] = name;
     spanBody["trace.id"] = traceID;
     spanBody["id"] = generateID();
     spanBody["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
     //Util::LogExit("Span()");
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "Span() exit");
   }

   void Span::close(request_rec *rRec) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "span.close enter");
     attributes["duration.ms"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - spanBody["timestamp"].as_int64();
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "span.close enter");
   }

   std::string Span::generateID() {
     id.append(std::to_string(getpid()));
     id.append(":");
     id.append(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()));
     return id;
   }

   Span::SpanBody Span::getBody() {
     // Don't put the attributes into the body until the very last
     spanBody["attributes"] = attributes;
     return this->spanBody;
   }

   std::string Span::getID() {
     return id;
   }

   Span::Span() = default;
}