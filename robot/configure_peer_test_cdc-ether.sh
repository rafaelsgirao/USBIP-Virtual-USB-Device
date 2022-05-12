#!/bin/bash

set -e

PEER_INTERFACE=enp1s0
PEER_DEVICE=macvlan0
PEER_IP_ADDRESS=10.0.0.2
PEER_IP_MASK=24

function start()
{
    ip link add ${PEER_DEVICE} link ${PEER_INTERFACE} type macvlan mode bridge
    ip addr add ${PEER_IP_ADDRESS}/${PEER_IP_MASK} dev ${PEER_DEVICE}
    ip link set ${PEER_DEVICE} up
}

function stop()
{
    ip link del ${PEER_DEVICE} link ${PEER_INTERFACE} type macvlan mode bridge
}

case $1 in
    start)
        start
        ;;
    stop)
        stop 
        ;;
    restart)
        stop
        start
        ;;
    *)
        echo "Usage: $0 start|stop|restart"
        exit 1
        ;;
esac
