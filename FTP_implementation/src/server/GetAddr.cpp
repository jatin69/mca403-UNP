#include "server.h"

/**VISITED. - REMOVE THIS after checking compatibility
 * 
 * Should this be eliminated ?? Maybe. Not required in any near future.
 * Because i'm gonna use AF_INET
 * When the world is ready for migration. We'll modify the code at once to support IPv4
 * 
 * A function wrapper to wrap both IPv4 and IPv6
 * @param  struct sockaddr *sa
 */
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

//  THIS SHOULD BE HANDLED WITH CONSISTENCY