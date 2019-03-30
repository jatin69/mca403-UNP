#include "client.h"


/**VISITED. Same as server
 * To execute any system command
 * @param  cmd - command to be executed
 * @return  output of the command
 */
string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}