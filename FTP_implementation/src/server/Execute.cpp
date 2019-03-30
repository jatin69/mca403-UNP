#include "./server.h"


/**VISITED. Works Perfectly.
 * 
 * can also be used to create and delete directories.
 * executes a command and returns its output
 * @param const char *cmd  - command to execute
 * @return The output of the command
 */
string exec(const char *cmd){

	FILE *pipe = popen(cmd, "r");
	if (!pipe)
		return "ERROR";
	char buffer[128];
	std::string result = "";
	while (!feof(pipe))
	{
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

// ALSO IMPLEMENT the direct SYSTEM command
// also figure out differences between these two