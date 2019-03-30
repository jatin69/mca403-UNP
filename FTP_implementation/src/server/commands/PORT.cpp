#include "./../server.h"



/**VISITED. Can be probably simplified.
 * 
 * Converts PORT command format to nice and good variables.
 * 
 * Gives the ip and port from the standard PORT command.
 * For eg. in = "PORT 127,0,0,1,35,40"
 * 			gives ipstr = "127.0.0.1" , portstr = "9000" (35*256 + 40)
 * @param  in      - input PORT command string
 * @param  ipstr   - output ip
 * @param  portstr - output port
 * @return -1 on error, 0 on success
 */
int reverseportstring(string &in, string &ipstr, string &portstr){
	int cnt = 0, pos;
	string ip = in;
	for (size_t i = 0; i < in.size(); ++i)
	{
		if (ip[i] == ',')
		{
			ip[i] = '.';
			cnt++;
			if (cnt == 4)
			{
				pos = i;
				break;
			}
		}
	}

	if (cnt != 4)
		return -1;
		
	ipstr = ip.substr(0, pos);

	/*
	Work in Progress [ WIP ]

	this works with 2 VMs because both have static IPs
	Fails in my router because it does not know how to route this from router to my system
	it will work from my system to a VM when telnet works from VM to my system
	*/
	// ipstr = "35.200.210.239";	// Google cloud
	// ipstr = "103.78.148.10";		// my system's IP for now
	// ipstr = "139.59.79.252";		// Digital Ocean
	// Use `curl ifconfig.me` to find IP address on the client machine

	string port = ip.substr(pos + 1);
	int val = 0;
	size_t i = 0;

	while (i < port.size())
	{
		if (port[i] == ',')
			break;
		val = 10 * val + (port[i] - '0');
		i++;
	}
	val = 256 * val;
	int portval = val;
	val = 0;
	i++;
	while (i < port.size())
	{
		val = 10 * val + (port[i] - '0');
		i++;
	}

	portval = portval + val;
	stringstream ss;
	ss << portval;
	portstr = ss.str();
	return 0;
}
