#pragma once

#include <ap_expr.h>
#include <map>
#include <boost/json/object.hpp>

namespace newrelic {
   class SpanCommon {
   public:
      typedef boost::json::object Common;
      typedef boost::json::object Attributes;
      explicit SpanCommon(server_rec *pRec);
      Common getCommon();


   private:
      SpanCommon();
      Attributes attributes;
      Common common;
   };
}