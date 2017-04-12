#include "majorServer.h"

std::map<boost::shared_ptr<tcp::socket>, size_t> ServiceState::_mapLoad ;
