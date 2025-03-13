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

check_rasp_pi_connection(){
   pi_mac_addr=2c:cf:67:4e:f4:fe
   # ARP tables only seem to fill after running this
   nmap -sn 192.168.1.0/24 > /dev/null  

   lines=$(ip neighbor)
   while IFS= read -r line; do 
     ip_addr=$(echo $line | cut -d" " -f 1 )
     mac_addr=$(echo $line | cut -d" " -f 5 )
     if [[ $mac_addr == $pi_mac_addr ]]; then
        echo "Found Raspberry Pi at $ip_addr"
        break
     fi
   done <<< "$lines"
}

if [ -z ${1+x} ]; then
      echo "Error: This script needs one or two cli args"
      echo "./upload.sh <action> <ip addr>"
      exit
fi 

case $1 in 
  "upload")
    if [ -z ${2+x} ]; then
      echo "Error: This script needs two cli args"
      echo "./upload.sh <action> <ip addr>"
      exit
    fi 
    echo "Uploading..."
    upload $2
    ;;
  "check_connection")
    
    echo "Checking Raspberry Pi Connection..."
    check_rasp_pi_connection
    ;;
  *)
    echo "Invalid Command";
    ;;
esac




