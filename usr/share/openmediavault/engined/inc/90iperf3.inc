<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @copyright Copyright (c) 2013-2024 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 require_once("openmediavault/functions.inc");

\OMV\System\LogFileSpec::registerSpecification("iperf3", [
  "command" => "export SYSTEMD_COLORS=0; sudo journalctl --quiet --no-pager SYSLOG_IDENTIFIER='iperf3' --output=short-iso",
  "filename" => "iperf3",
  "regex" => "/^(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}[+-]\d{4})\s+(\S+)\s+(.+)$/",
  "columns" => [
    "ts" => [
      "index" => 1,
      "func" => function($v) { return strpdate($v,"Y-m-d*H:i:sT"); }
    ],
    "date" => [
      "index" => 1,
      "func" => function($v) {
        return strftime("%c", strpdate($v,"Y-m-d*H:i:sT"));
      }
    ],
    "hostname" => 2,
    "message" => 3
  ]]);
