#!/bin/bash

name=wake_up_light_proxy
tag=0.0.1

sudo docker run \
	--name wake_up_light_proxy \
	-d \
	--restart always \
	-p 3333:3333 \
	$name:$tag
