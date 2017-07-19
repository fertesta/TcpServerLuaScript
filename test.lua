
print("lua: hello!!!\n")

function handle_connect(session_id)
	print("lua: connected session: ", session_id)
end

function handle_disconnect(session_id) 
	print("Lua: disconnected session:", session_id)
end

function handle_recv(data)
	print("lua: income data received")
	print("lua: data:", data)
	-- reply the received data
	return data:len()
end
