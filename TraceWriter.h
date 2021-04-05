#pragma once

#include <ap_expr.h>
#include "SpanCommon.h"
#include "Span.h"
#include "root_certificates.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include "mod_newrelic_telemetry.h"
#include "boost/asio/ssl/context_base.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace newrelic {
   class TraceWriter : public std::enable_shared_from_this<TraceWriter> {
   public:
      TraceWriter();
      void write(std::shared_ptr<newrelic::SpanCommon> *spanCommon, const std::map<std::string, std::shared_ptr<Span>> &spanMap);
   private:
      typedef boost::json::object Payload;
      typedef boost::json::array Spans;
      typedef boost::json::array Outer;
      net::io_context ioContext;
      // Obscure C++ init idiom
      ssl::context sslContext = ssl::context(ssl::context::tlsv12_client);
      tcp::resolver resolver = tcp::resolver (ioContext);
      beast::ssl_stream<beast::tcp_stream> stream = beast::ssl_stream<beast::tcp_stream>(ioContext, sslContext);
      void setStream();
      void resetConnection();

      void writeStream(std::string body);
   };
}
