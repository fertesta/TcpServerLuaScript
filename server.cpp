//
//  server.cpp
//  luaserver
//
//  Created by Fernando Testa on 14/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#include "server.hpp"

void session::handle_recv(const boost::system::error_code& error, size_t bytes_transferred) {
  if (!error) {
    std::cout << "session[" << this << "]::handle_recv()";

    std::string data(data_, bytes_transferred); // WARNING: makes a copy of the buffer.

    CLuaCall call_handle_recv(interpreter_, "handle_recv");
    call_handle_recv << data;
    call_handle_recv.call(1); // number of results expected. Must match the number of operator>> calls.
    call_handle_recv >> response_write_buffer_; // get result, remember, Lua is a FILO stack
  
    if (!response_write_buffer_.empty()) {
      boost::asio::async_write(socket_,
        boost::asio::buffer(response_write_buffer_),
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));
    }

  } else {
    std::cout << "session[" << this << "]::handle_recv disconnected with error = " << error << "\n";
    if((boost::asio::error::eof == error) ||
      (boost::asio::error::connection_reset == error)) {
        std::cout << "session[" << this << "]::handle_recv disconnected\n";
    }
    io_service_.post(boost::bind(&server::session_erase, server_, this));
  }
}

void session::handle_write(const boost::system::error_code& error) {
  if (!error) {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_recv, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  else {
    io_service_.post(boost::bind(&server::session_erase, server_, this));
  }
}

void session::handle_accepted() {
  CLuaCall call_handle_connected(interpreter_, "handle_connect");
  call_handle_connected << reinterpret_cast<void*>(this);
  call_handle_connected.call(0);
  async_read_some();
};

void session::handle_disconnect() {
  CLuaCall call_handle_connected(interpreter_, "handle_disconnect");
  call_handle_connected << reinterpret_cast<void*>(this);
  call_handle_connected.call(0);
}


bool operator==(const SessionPtr& x, session *y) {
  return x.get() == y;
}

void server::session_erase(session * s) {
  auto it = std::find(sessions_.begin(), sessions_.end(), s);
  if(it == sessions_.end()) return;
  (*it)->handle_disconnect();
  std::cout << "server::session_erase(session=" << s << ")\n";
  sessions_.erase(it);
}

void server::start_accept() {
  SessionPtr new_session(new session(this, io_service_, luascript_));
  sessions_.push_back(new_session);
  acceptor_.async_accept(new_session->socket(),
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));
}

void server::handle_accept(SessionPtr new_session, const boost::system::error_code& error) {
  if (error) {
    std::cerr << "server::handle_accept() error " << error << std::endl;
  } else {
    new_session->handle_accepted();
  }
  start_accept();
}
