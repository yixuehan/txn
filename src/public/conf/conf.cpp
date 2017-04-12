#include <stdlib.h>

#include <iostream>
#include <string>
#include <boost/property_tree/info_parser.hpp>

#include "public/conf/conf.h"

static std::string fileName ;
static bool bInit = false ;

// 获取配置信息
boost::shared_ptr<boost::property_tree::ptree> getSetting()
{
   using namespace boost::property_tree ;
   static boost::shared_ptr<ptree> pPtree ;
   if ( !bInit ) {
      bInit = true ;
      pPtree.reset( new ptree ) ;
      try {
         read_info( fileName, *pPtree ) ;
      }
      catch ( info_parser_error &e ) {
         pPtree = nullptr ;
         bInit = false ;
         std::cout << "解析文件[" << e.filename() << "]在第[" << e.line() << "]行出错[" << e.message() << "]\n" ;
      }
   }
   return pPtree ;
}

// 设置配置信息的配置文件
void setSettingFile( const boost::filesystem::path &p ) 
{
   std::remove_all_extents<decltype(p)>::type root = getenv( "PROROOT" ) ;
   if ( root.empty() ) {
      std::cout << "请先配置环境变量[PROROOT]" << std::endl ;
      return ;
   }
   fileName  = (root / "etc" / p).string();
   std::cout << "etc/fileName:" << fileName << std::endl ;
   bInit = false ;
}
