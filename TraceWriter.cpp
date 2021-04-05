#include <boost/json/src.hpp>
#include "TraceWriter.h"
#include "Util.h"
#include "mod_newrelic_telemetry.h"
#include <openssl/err.h>
#include <boost/stacktrace.hpp>

namespace newrelic {

   // TODO move SpanCommon here as a param
   TraceWriter::TraceWriter() {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "TraceWriter(): enter");
     this->setStream();
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "TraceWriter(): exit");
   }

   void TraceWriter::write(std::shared_ptr<newrelic::SpanCommon> *spanCommon, const std::map<std::string, std::shared_ptr<Span>> &spanMap) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.write enter");
     std::string s;
     try {
       Payload payload;
       Spans spans;
       payload["common"] = (*spanCommon)->getCommon();
       for (auto const&[key, span] : spanMap) {
         spans.push_back(span->getBody());
       }
       payload["spans"] = spans;
       Outer outer = {payload};
       s = boost::json::serialize(outer);
       this->writeStream(s);
       //ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "    traceWriter.writer JSON: %s", s.c_str());
     } catch (std::exception &e) {
       ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.write std::exception: %s", e.what());
       std::cerr << boost::stacktrace::stacktrace() << std::endl;
       // If we get end of stream re-establish connection and retry
       beast::error_code ec;
       stream.shutdown(ec);
       if (ec == net::error::eof) {
         // Rationale:
         // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
         ec = {};
       }
       if (ec) {
         ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.write boost::error_code:");
         std::cerr << ec << std::endl;
       }
       this->resetConnection();
       this->setStream();
       this->writeStream(s);
     } catch (...) {
       ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.write unknown exception");
       std::cerr << boost::stacktrace::stacktrace() << std::endl;
     }
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.write exit");
   }

   void TraceWriter::writeStream(std::string body) {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream enter: writing: %s", body.c_str());
     auto const host = newRelicConfig.newRelicTraceHost;
     auto const path = newRelicConfig.newRelicTracePath;
     int version = 11;

     // Set up an HTTP GET request message
     http::request<http::string_body> req{http::verb::post, path, version, body};
     req.set(http::field::host, host);
     req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
     req.set(http::field::content_type, "application/json");
     req.set("Api-Key", newRelicConfig.insightsInsertKey);
     req.set("Data-Format", "newrelic");
     req.set("Data-Format-Version", "1");
     req.content_length(body.size());
     req.keep_alive(true);


     // Send the HTTP request to the remote host
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream request");
     std::cerr << req << std::endl;
     auto out = http::write(stream, req);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream 1: out: %lu", out);

     // This buffer is used for reading and must be persisted
     beast::flat_buffer buffer;
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream 2: out: %lu", out);

     // Declare a container to hold the response
     http::response<http::dynamic_body> res;
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream 3: out: %lu", out);

     // Receive the HTTP response
     auto in = http::read(stream, buffer, res);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream 4: in: %lu", in);

     // Write the message to standard out
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream response");
     std::cerr << res << std::endl;
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.writeStream exit");
   }

   void TraceWriter::setStream() {
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream enter");
     auto const host = newRelicConfig.newRelicTraceHost;
     auto const path = newRelicConfig.newRelicTracePath;
     auto const port = "443";

     // The io_context is required for all I/O

     // The SSL context is required, and holds certificates
     //ssl::context ctx(ssl::context::tlsv12_client);

     // This holds the root certificate used for verification
     load_root_certificates(sslContext);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 3");

     // Verify the remote server's certificate
     sslContext.set_verify_mode(ssl::verify_peer);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 4");

     // These objects perform our I/O
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 5");
     //stream = std::make_shared<beast::ssl_stream<beast::tcp_stream>> (ioContext, sslContext);
     //beast::ssl_stream<beast::tcp_stream> stream(ioContext, ctx);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 6");

     // Set SNI Hostname (many hosts need this to handshake successfully)
     if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
       beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
       ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 7");
       throw beast::system_error{ec};
     }
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 8");

     // Look up the domain name
     auto const results = resolver.resolve(host, port);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 9");

     // Make the connection on the IP address we get from a lookup
     beast::get_lowest_layer(stream).connect(results);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 10");

     // Perform the SSL handshake
     (stream).handshake(ssl::stream_base::client);
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream 11");
     //this->stream = &stream;
     ap_log_error(APLOG_MARK, APLOG_ERR, 0, nullptr, "traceWriter.setStream exit");
   }

   void TraceWriter::resetConnection() {
     net::io_context ioc;
     //ioContext = ioc;
     sslContext = ssl::context(ssl::context::tlsv12_client);
     resolver = tcp::resolver(ioc);
     stream = beast::ssl_stream<beast::tcp_stream>(ioc, sslContext);
   }
}
