
print("lua: hello!!!\n")

function handle_connect()
	print("lua: someone connected")
end

function handle_recv(data)
	print("lua: income data received")
	print("lua: data:", data)
	-- reply the received data
	return data 
end
