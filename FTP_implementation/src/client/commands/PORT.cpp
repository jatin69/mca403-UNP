#include "./../client.h"

/**VISITED. Nice function.
 * 
 * Used in only one place. Can probably be removed and merged there.
 * Instead use : std::to_string(3.1415926);
 * and compile with c++11 flag
 * 
 * Converts int to string
 * @param  k - supplied integer
 * @return   string
 */
string int2str(int k){
	stringstream ss;
	ss<<k;
	return ss.str();
}

/**VISITED. 
 * This code returns the local-IP of client.
 * 
 * To communicate on internet, this should return its internet address hopefully.
 * This can be done with curl command & exec function.
 * Basically 3 Modes will be handled here.
 * SELF system.
 * LAN System.
 * WAN system.
 * Maybe take it from user which one he wants. -> BEST OPTION
 * Other option is to detect from user input. Will be Buggy.
 * 
 * Gives the local ip of the current system on which this code is running
 * @param  m_sd - the socket descriptor
 * @return - ip string 
 */
string getownip(int m_sd){
	struct sockaddr_in localAddress;
	socklen_t addressLength = sizeof(localAddress);
	getsockname(m_sd, (struct sockaddr*)&localAddress, &addressLength);
	return string(inet_ntoa( localAddress.sin_addr));
}

/**VISITED. Returns a string as per PORT command format
 * This works fine. Just that it needs correct IP passed.
 * // Needs a mechanism
 * 
 * Selects a random port and gives its port string by assigning portstr and also giving port in port
 * @param ownip   - the supplied ip (Eg. 127.0.0.1 )
 * @param portstr - returned PORT command string. (Eg- PORT 127,0,0,1,35,40)
 * @param port    - returned port (Eg - 9000)
 */
void getportstring(string ownip,string& portstr, string& port){
	for (size_t i = 0; i < ownip.size(); ++i)
	{
		if(ownip[i] == '.') ownip[i] = ',';
	}
	int portnum = 9001 + rand() % 10;
	string p1 = int2str(portnum/256);
	string p2 = int2str(portnum%256);
	portstr = "PORT "+ownip+","+p1+","+p2+"\r\n";
	//portstr = "PORT 127,0,0,1,"+p1+","+p2+"\r\n";
	port = int2str(portnum);
}
