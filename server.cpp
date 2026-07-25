//
//  server.cpp
//  luaserver
//
//  Created by Fernando Testa on 14/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//
#include "server.hpp"

const char * api_session_pointer = "yhRI3OnFVRuq2JRh";

/* Must follow lua_CFunction signature
*/
int funkyfunction(lua_State* L) {
  void * user_data = get_lightuserdata(L, api_session_pointer);
  session * session_ptr = reinterpret_cast<session*>(user_data);
  std::string s;
  CLuaOpt(L, 1) >> s;
  std::cout << "funky(session=" << session_ptr << ", '" << s << "')\n";

  return 0;
}

void session::handle_recv(const boost::system::error_code& error, size_t bytes_transferred) {
  if (error) {
    std::cout << "session[" << this << "]::handle_recv disconnected with error = " << error << "\n";
    io_service_.post(boost::bind(&server::session_erase, server_, this));
    return;
  }

  std::istream stream(&read_buf_);
  std::string data;
  std::getline(stream, data);
  data += '\n';

  response_write_buffer_.clear();
  CLuaCall call_handle_recv(interpreter_, "handle_recv");
  call_handle_recv << data;
  call_handle_recv.call(1); // number of results expected. Must match the number of operator>> calls.
  call_handle_recv >> response_write_buffer_; // get result, remember, Lua is a FILO stack

  if (!response_write_buffer_.empty()) {
    boost::asio::async_write(socket_,
      boost::asio::buffer(response_write_buffer_),
      boost::bind(&session::handle_write, this,
      boost::asio::placeholders::error));
  } else {
    async_read_some();
  }
}

void session::handle_write(const boost::system::error_code& error) {
  if (error) {
    io_service_.post(boost::bind(&server::session_erase, server_, this));
    return;
  }

  async_read_some();
}

void session::handle_accepted() {
  connected_ = true;

  // save our pointer into a global table.
  set_lightuserdata(interpreter_.getState(), api_session_pointer, this);
  interpreter_.register_function("funky", funkyfunction);

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
  if ((*it)->is_connected())
    (*it)->handle_disconnect();
  sessions_.erase(it);
}


void server::start_accept() {
  auto new_session = std::make_shared<session>(this, io_service_, luascript_);
  sessions_.push_back(new_session);
  acceptor_.async_accept(new_session->socket(),
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));
}

void server::handle_accept(SessionPtr new_session, const boost::system::error_code& error) {
  if (error) {
    std::cerr << "server::handle_accept() error " << error << std::endl;
    session_erase(new_session.get());
  } else {
    new_session->handle_accepted();
  }
  start_accept();
}
