#ifndef _CONF_H
#define _CONF_H
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

// 获取配置信息
boost::shared_ptr<boost::property_tree::ptree> getSetting() ;

// 设置配置信息的配置文件
void setSettingFile( const boost::filesystem::path & ) ;
#endif 
