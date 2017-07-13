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

namespace io = boost::asio;


class TCPServer 
{
	io::io_service _ioservice;
	io::ip::tcp::acceptor _tcp_acceptor;
	io::ip::tcp::socket _tcp_socket;
public:
	TCPServer(unsigned short port);
	void run();

	io::io_service& ioservice() 
	{
		return _ioservice;
	}

	io::ip::tcp::socket& tcp_socket() 
	{
		return _tcp_socket;
	}
	
	template<typename Handler>
	void set_handler_receive(Handler& handler);
};


#endif /* TCPServer_hpp */
