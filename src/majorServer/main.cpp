#include <boost/format.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "majorServer.h"
#include "public/conf/conf.h"

int main()
{
   try {
      setSettingFile( "majorServer.info" ) ;
      MajorServer<chat> server ;
      server.run() ;
   }
	catch ( boost::property_tree::info_parser_error &ec ) {
      boost::format str( "文件%1%在%2%行解析出错.%3%") ;
      str % ec.filename() % ec.line() % ec.message() ;
      std::cout << str.str() << std::endl ;
   }
   catch ( boost::property_tree::xml_parser_error &ec ) {
      boost::format str( "文件%1%在%2%行解析出错.%3%") ;
      str % ec.filename() % ec.line() % ec.message() ;
      std::cout << str.str() << std::endl ;
   }
   catch ( boost::property_tree::ptree_bad_path &ec ) {
      boost::format str( "%1%") ;
      str % ec.what() ;
      std::cout << str.str() << std::endl ;
   }
   catch ( boost::property_tree::ptree_bad_data &ec ) {
      boost::format str( "%1%") ;
      str % ec.what();
      std::cout << str.str() << std::endl ;
   }
   catch ( boost::bad_any_cast ec)
   {
      boost::format str("%1%") ;
      str % ec.what() ;
      std::cout << str.str() << std::endl ;
   }
   catch(std::exception &e)
   {
      boost::format str("%1%") ;
      str % e.what();
      std::cout << str.str() << std::endl ;
   }
   catch(...)
   {
      boost::format str("Unknown exception") ;
      std::cout << str.str() << std::endl ;
   }
}
