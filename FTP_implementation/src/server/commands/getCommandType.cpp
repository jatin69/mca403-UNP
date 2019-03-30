#include "./../server.h"

/**
 * Any addition to commandList should also be reflected here.
 * handle cases etc
 **/
cmd::Command getCommandType(const std::string &msg) {
    using namespace cmd;
    std::string cmd(msg);
    if (cmd.find("NOOP") == 0) return NOOP;
    return INVALID;
}