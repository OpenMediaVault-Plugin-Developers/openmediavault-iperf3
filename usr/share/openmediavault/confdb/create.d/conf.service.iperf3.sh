#!/bin/sh

set -e

. /usr/share/openmediavault/scripts/helper-functions

if ! omv_config_exists "/config/services/iperf3"; then
  omv_config_add_node "/config/services" "iperf3"
  omv_config_add_key "/config/services/iperf3" "enable" "0"
  omv_config_add_node "/config/services/iperf3" "jobs"
fi

exit 0
