#!/bin/bash

name=wake_up_light_proxy
tag=0.0.1

docker build \
	-t $name:$tag \
	.
