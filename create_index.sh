#!/bin/bash

ipdr=$(docker inspect  -f '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' papa_web)
curl -X POST "http:/$(echo $ipdr):732/api/create"
