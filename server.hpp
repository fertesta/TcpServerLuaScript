//
//  server.hpp
//  luaserver
//
//  Created by Fernando Testa on 14/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#ifndef TCPServer_hpp
#define TCPServer_hpp

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <algorithm>
#include "lua_utils.hpp"

class session;
class server;

using SessionPtr = std::shared_ptr<session>;

class session {
  static const int max_length = 1024;
  char data_[max_length];
  boost::asio::ip::tcp::socket socket_;
  std::string response_write_buffer_;
  CLuaInterpreter interpreter_;
  boost::asio::io_service& io_service_;
  server * server_ = nullptr;
public:
  session(server * srv, boost::asio::io_service& io_service, const std::string& luascript)
    : socket_(io_service), io_service_(io_service), server_(srv) {
      interpreter_.load_file(luascript);
  }

  ~session() {
    std::cout << "~session()[" << this << "]\n";
  }

  boost::asio::ip::tcp::socket& socket() {
    return socket_;
  }

  void async_read_some() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&session::handle_recv, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  }

  void handle_accepted();
private:
  void handle_recv(const boost::system::error_code& error, size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);
};


class server
{
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::string luascript_;
  std::vector<SessionPtr> sessions_;
public:
  server(boost::asio::io_service& io_service, short port, const std::string& luascript)
    : io_service_(io_service),
      acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), 
      luascript_(luascript) {
    start_accept();
  }

  void session_erase(session * s);

private:
  void start_accept();

  void handle_accept(SessionPtr new_session, const boost::system::error_code& error);

  void handle_timeout() {
    std::cout <<  "handle_timeout\n";
  }
};

#endif // 
