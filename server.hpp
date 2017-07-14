//
//  TCPServer.hpp
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
#include "lua_utils.hpp"

class session
{
  boost::asio::ip::tcp::socket socket_;
  static const int max_length = 1024;
  char data_[max_length];
  std::string response_write_buffer_;
  CLuaInterpreter interpreter_;
public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service) {
  }

  boost::asio::ip::tcp::socket& socket() {
    return socket_;
  }

  void async_read_some() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

private:
  void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);
};


class server
{
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_; 
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept() {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(session* new_session, const boost::system::error_code& error) {
    std::cout << "handle_accept: " << std::hex << new_session << " error=" << error << '\n';
    if (!error) {
      new_session->async_read_some();
    } else {
      delete new_session;
    }
    start_accept();
  }

  void handle_timeout() {
    std::cout <<  "handle_timeout\n";
  }
};

#endif /* TCPServer_hpp */
