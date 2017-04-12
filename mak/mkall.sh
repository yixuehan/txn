#!/bin/bash
cd $PROROOT/src/public/conf
bash make.sh $1 $2

cd $PROROOT/src/public/communication
bash make.sh $1 $2

cd $PROROOT/src/public/modules
bash make.sh $1 $2

cd $PROROOT/src/public/service
bash make.sh $1 $2

cd $PROROOT/src/majorServer
bash make.sh $1 $2

cd $PROROOT/src/service/account
bash make.sh $1 $2



