#!/bin/bash  

function monitor_Logfile() {
    rm $1
    sleep 2
    while [ 1 ]
    do
    LOGFILE=$(tail -n 800 $1)
    TMP=$(curl -H "Content-Type: text/plain" --silent -d "$LOGFILE" https://api.omnetlense.com/runs/$2/progress)
    sleep 2
    done
}


SCENARIO_NAME=$1
CAMPAIGN_NAME=$(<lense-campaign.config)
echo "##### Starting $SCENARIO_NAME #####"
echo ""

POST_DATA="{\"campaign\":\"$CAMPAIGN_NAME\", \"name\":\"$1-$2\"}"
RUN_ID=$(curl -H "Content-Type: application/json" --silent -d "$POST_DATA" https://api.omnetlense.com/runs/init)
RUN_ID="${RUN_ID%\"}"
RUN_ID="${RUN_ID#\"}"

monitor_Logfile results/out-$SCENARIO_NAME-$2.log $RUN_ID & 
monitor_pid=$!

opp_runall ../src/intairnet-link-layer -m -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases --image-path=../../inet/images -l ../../inet/src/INET -c $1 -r $2


sleep 10
kill $monitor_pid
wait $monitor_pid 2>/dev/null
#rm -rf results/out-$2.log
cd results
mkdir -p json

scavetool x $1-#$2.sca $1-#$2.vec -o ./json/tmp-$1-$2.json
UPLOAD_URL=$(curl --silent https://api.omnetlense.com/upload/url/$RUN_ID)
curl -X PUT --data "@json/tmp-$1-$2.json" -H "Content-Type: application/json" --silent $UPLOAD_URL
curl --silent -X POST https://api.omnetlense.com/runs/$RUN_ID
#rm -rf json
echo ""
echo "##### Finished $SCENARIO_NAME #####"

trap 'kill $monitor_pid' SIGINT
