<?php
/**
 * Copyright (C) 2013-2024 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

class OMVRpcServiceIperf3 extends \OMV\Rpc\ServiceAbstract
{
    public function getName()
    {
        return "iPerf3";
    }

    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");

        $this->registerMethod('getJobList');
        $this->registerMethod('getJob');
        $this->registerMethod('setJob');
        $this->registerMethod('deleteJob');
        $this->registerMethod('doJob');
    }

    public function getSettings($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.iperf3");
        // Remove useless properties from the object.
        $object->remove("jobs");
        return $object->getAssoc();
    }

    public function setSettings($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.iperf3.setsettings");
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.iperf3");
        $object->setAssoc($params);
        $db->set($object);
        // Remove useless properties from the object.
        $object->remove("jobs");
        // Return the configuration object.
        return $object->getAssoc();
    }

    public function getJobList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.getlist");
        // Get configuration data.
        $db = \OMV\Config\Database::getInstance();
        $objects = $db->getAssoc("conf.service.iperf3.job");
        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    function getJob($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.objectuuid");
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.iperf3.job", $params['uuid']);
        return $object->getAssoc();
    }

    function setJob($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.iperf3.setjob");
        // Prepare the configuration object.
        $object = new \OMV\Config\ConfigObject("conf.service.iperf3.job");
        $db = \OMV\Config\Database::getInstance();
        $object->setAssoc($params);
        // Set the configuration object.
        $db->set($object);
        // Return the configuration object.
        return $object->getAssoc();
    }

    public function deleteJob($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.objectuuid");
        // Delete the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.iperf3.job", $params['uuid']);
        $db->delete($object);
        // Return the deleted configuration object.
        return $object->getAssoc();
    }

    public function doJob($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.objectuuid");
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.iperf3.job", $params['uuid']);
        return $this->execBgProc(function($bgStatusFilename, $bgOutputFilename)
            use ($object) {
                $output = [];
                $cmdArgs = [];
                $cmdArgs[] = 'iperf3';
                $cmdArgs[] = sprintf("-c '%s'", $object->get('server'));
                if ($object->get('reverse')) {
                    $cmdArgs[] = '-R';
                }
                if ($object->get('verbose')) {
                    $cmdArgs[] = '--verbose';
                }
                if ($object->get('json')) {
                    $cmdArgs[] = '--json';
                }
                if ($object->get('log')) {
                    $cmdArgs[] = '| tee -a /var/log/omv-iperf3-jobs.log';
                }
                $cmd = new \OMV\System\Process($cmdArgs);
                $cmdLine = $cmd->getCommandLine();
                if (0 !== $this->exec($cmdLine, $output, $bgOutputFilename)) {
                    throw new \OMV\ExecException($cmdLine, $output);
                }
                return $output;
            }
        );
    }
}
