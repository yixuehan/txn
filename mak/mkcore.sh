#!/bin/bash
#默认为debug方式
if [ 0 -eq $# ]
then
   cmds=debug
else
   cmds=$@
fi

make_func()
{
   if [ ! -d "depend" ]
   then
      mkdir "depend"
   fi

   if [ ! -d "obj" ]
   then
      mkdir "obj"
   fi

   for target in ${targets}
   do
      for cmd in $@
      do
    #支持默认目标文件
    export STARGET=${target} 
    export SOBJS="`eval echo '$'{${target}_objs'}'`"
         if [ -z "${SOBJS}" ]
         then
           export   SOBJS="${target}.o"
         fi

    export SLIBS="`eval echo '$'{${target}_libs'}'`" 
         export SINCLUDEPATH="`eval echo '$'{${target}_includepath'}'`"

         if [ "clean" = ${cmd} ]
         then
            make -f ${PROROOT}/mak/mkcore.mak ${cmd} 
         elif [ "all" = ${cmd} ]
         then
            cmd=debug${targetType}
            make -f ${PROROOT}/mak/mkcore.mak ${cmd}
         else
            cmd=${cmd}${targetType}
            make -f ${PROROOT}/mak/mkcore.mak ${cmd}
         fi
      done
   done
}

for cmd in ${cmds}
do
   make_func ${cmd}
done
