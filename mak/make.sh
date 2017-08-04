#!/bin/bash

#多个值要用双引号包含！！！！！！！！！！！！！！

#指定生成动态库dynamic  静态库static 可执行目标exec 对应的debug和release方式及JAVA字节码java
targetType=

#指定目标列表  如：test  动态库会自动扩展为libtest.so 静态库同理
targets=

#指定目标依赖
#命名规则  目标_ojbs  目标_libs  如:test_objs  test_libs test_includepath
xxx_objs=
xxx_libs=
xxx_includepath=


#下面的代码不要修改!!!!!!!!!!!!!!!!!!!!!!!!

. ${PROROOT}/mak/mkcore.sh

