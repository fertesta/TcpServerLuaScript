//
//  TCPServer.cpp
//  luaserver
//
//  Created by Fernando Testa on 14/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#include "TCPServer.hpp"


TCPServer::TCPServer(unsigned short port)
:
  _tcp_acceptor(_ioservice, io::ip::tcp::endpoint(io::ip::tcp::v4(), port)),
  _tcp_socket{_ioservice}
{
  _tcp_acceptor.listen();
}

void TCPServer::run()
{
  _tcp_acceptor.async_accept(_tcp_socket, accept_handler);
  _ioservice.run();
}

template<typename Handler>
void TCPServer::set_handler_receive(Handler& handler) {
  
}
