#! /bin/bash

upload (){
  # Put the ip addr of the raspberry pi in here
  cd ../
  tar cvzf meter_blockage.tar.gz meter_blockage_detection/
  scp meter_blockage.tar.gz supertitan@$1:/home/supertitan
  ssh supertitan@$1 'working_dir="/home/supertitan/coding/meter_blockage_detection/microwave_sensor"
  tar xzvf meter_blockage.tar.gz -C $working_dir
  cd $working_dir/meter_blockage_detection
  make'
}

unzip(){
  working_dir="~/coding/meter_blockage_detection/microwave/"
  tar xzvf meter_blockage.tar.gz $working_dir
  cd $working_dir/meter_blockage_detection/
  make
}

if [ -z ${1+x} ] | [ -z ${2+x} ]; then
  echo "Error: This script needs to cli args"
  echo "./upload.sh <action> <ip addr>"
  exit
fi

case $1 in 
  "upload")
    echo "Uploading..."
    upload $2
    ;;
  "unzip")
    echo "Unziping and running..."
    unzip 
    ;;
  *)
    echo "Running everything..";
    ;;
esac




