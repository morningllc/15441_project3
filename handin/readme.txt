include files:
	-	Makefile
	-	readmen.txt
	-	writeup.pdf
	src/
		#for proxy
		-	proxy.c				   include main function for proxy server and functions for server structure
		-	proxy_parser.c		   include functions for parsing data packets
		-	proxy_process.c		   include functions for process request
		-	proxy_log.c			   include functions for logging
		-	proxy_bitrate.c		   include functions for calculating bitrate
		-	proxy_socket.c 		   include functions for buffer and socket manipulation
		-	proxy_queue.c 		   API functions for Queue
		-	dns_packet.c 		   include functions for constructing dns request packet
		-	mydns.c 			   include reslove function library

		# for DNS server
		-	dns.c 				   include main function for DNS server
		-	dns_packet_server.c    include functions for constructing DNS reponse packet
		-	dns_parser.c 		   include functions for parsing DNS request
		-	dns_queue.c 		   API functions for Queue and Priority Queue
		-	dns_robin.c 		   include functions for Round Robin method
		-	dns_dijkstra.c 		   include functions for LSAs method
		-	dns_log.c 			   include functions for logging
		-	graph.c 			   include functions for building graph with LSAs file 
	src/inc/
		#for proxy
		-	proxy.h
		-	proxy_parser.h
		-	proxy_process.h
		-	proxy_log.h
		-	proxy_bitrate.h
		-	proxy_socket.h
		-	proxy_queue.h
		-	dns_packet.h
		-	mydns.h

		# for DNS server
		-	dns.h
		-	dns_packet_server.h
		-	dns_parser.h
		-	dns_queue.h
		-	dns_robin.h
		-	dns_dijkstra.h
		-	dns_log.h
		-	graph.h
