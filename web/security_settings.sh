#!/bin/bash


ipdrElastic=$(docker inspect  -f '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' es717)
curl -XPUT -H "Content-Type: application/json" http://$(echo $ipdrElastic):9200/_cluster/settings -d '{ "transient": { "cluster.routing.allocation.disk.threshold_enabled": false } }'
curl -XPUT -H "Content-Type: application/json" http://$(echo $ipdrElastic):9200/_all/_settings -d '{"index.blocks.read_only_allow_delete": null}'
