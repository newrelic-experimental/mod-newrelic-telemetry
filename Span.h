#pragma once
#include <httpd.h>
#include <boost/json/object.hpp>
#include <cstring>

namespace newrelic {
   class Span {
      typedef boost::json::object Attributes;
      typedef boost::json::object SpanBody;

   public:
      Span(request_rec *rRec, const std::string& spanName, const std::string& parentID, const std::string& traceID);
      void close(request_rec *rRec);
      SpanBody getBody();
      std::string getID();

   private:
      std::string id;
      SpanBody spanBody;
      Attributes attributes;
      Span();
      std::string generateID();
   };
}