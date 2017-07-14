//
//  TCPServer.cpp
//  luaserver
//
//  Created by Fernando Testa on 14/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#include "server.hpp"


void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
  if (!error)
  {
    std::cout << "session[" << this << "]::handle_recv()";
    
    {
      std::string data(data_, bytes_transferred); // WARNING: makes a copy of the buffer.
      CLuaCall call_handle_recv(interpreter_, "handle_recv");
      call_handle_recv << data;
      call_handle_recv.call(1); // number of results expected. Must match the number of operator>> calls.
      std::string response;
      call_handle_recv >> response_write_buffer_; // get result, remember, Lua is a FILO stack

      boost::asio::async_write(socket_,
        boost::asio::buffer(response_write_buffer_),
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));
    }
    async_read_some();

  }
  else
  {
    if((boost::asio::error::eof == error) ||
      (boost::asio::error::connection_reset == error)) {
        std::cout << "session[" << this << "]::handle_read disconnected\n";
      }
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error) {
  if (!error) {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  else {
    delete this;
  }
}

