#!/usr/bin/bash
# start the background monitoring and check if it is still
# alive
# can be started from a cronjob (checks every 5 minutes) by 
# */5 * * * * /export/software/oncs/OnlMon/install/bin/monserver.sh >& /dev/null
[ -f ~/.bash_profile ] && . ~/.bash_profile

if [ $# -le 1 ]
then
    echo "Usage : $0 {start|status|stop|restart|valgrind} n (1-5)"
    exit 1
fi
hostname=`hostname -s`
piddir=/tmp/sphnxonlmon
pidfile=${piddir}/onlmon$2
#echo $hostname
[ -d $piddir ] || mkdir $piddir
[ -f $pidfile ] && touch $pidfile

[ -d /scratch/phnxrc/onlmon ] || mkdir -p /scratch/phnxrc/onlmon

case "$1" in
    status)
	if [ -s $pidfile ]
	then
            ps `cat $pidfile`
            if [ $? != 0 ]
            then
		echo "Monitoring Dead"
		rm $pidfile
		exit 1
            else
		#echo "Monitoring running"
		exit 0
            fi
	else
            echo "Monitoring not running"
            [ -f $pidfile ] && rm $pidfile
            exit 1
	fi
	;;
    start)
	$0 stop $2
	#echo starting
	#echo $hostname.$2.cmd
    if [[ "$(hostname)" =~ ^'mvtx'[0-5] ]]
    then
        d=`date +%Y%m%d`
        t=`date +%H%M`
        nohup root.exe -l $ONLMON_SERVERWATCHER/$hostname.monitorserver.$2.cmd > /scratch/phnxrc/onlmon/${hostname}.monitorserver.$2.log 2> /scratch/phnxrc/onlmon/${hostname}.monitorserver.$2_${d}_${t}.err &
    else
        nohup root.exe -l $ONLMON_SERVERWATCHER/$hostname.monitorserver.$2.cmd >& /scratch/phnxrc/onlmon/${hostname}.monitorserver.$2.log &
    fi
    #nohup root.exe -l $ONLMON_SERVERWATCHER/$hostname.monitorserver.$2.cmd >& /scratch/phnxrc/onlmon/${hostname}.monitorserver.$2.log &
	pid=`echo $!`
	echo $pid > $pidfile
	;;
    stop)
	if [ -s $pidfile ]
	then
	    kill -INT `cat $pidfile`
	    if [ $? != 0 ]
	    then
		echo "No Online Monitoring process " `cat $pidfile`
	    else
		echo "Sending INT signal to Online Monitoring process " `cat $pidfile`
                sleep 1
                counter=0
                while ps -p `cat $pidfile` > /dev/null
                do
                    sleep 1
                    counter=$[$counter + 1]
		    if [ $counter -gt 5 ]
		    then
			echo "Online Monitoring process " `cat $pidfile` " survived SIGINT signal, sending SIGKILL"
			kill -9 `cat $pidfile`
		    fi
		done
	    fi
	    rm $pidfile
	fi
	;;
    restart)
	$0 start $2
	;;
esac

#[[ -d /scratch/phnxrc/onlmon ]] || mkdir -p /scratch/phnxrc/onlmon
#[[ -d /tmp/sphnxonlmon ]] || mkdir /tmp/sphnxonlmon
