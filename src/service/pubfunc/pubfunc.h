#include "core.h"
#include "public/service/service.h"



class Pubfunc : public Service
{
public:
   Pubfunc() ;
   void addService() ;
   void calculator( boost::shared_array<char> req_ptr, size_t req_byte ) ;
private:

} ;
