<?php
require_once("./php/conexion.php");
header('Access-Control-Allow-Origin: *');

class Api
{
    function __construct()
    {
        // print_r($_GET);
        // print_r($_POST);

        $response = '0';
        $json = array();
        if (count($_FILES) > 0) {

            $json['parameters'] = array(

                'text' => $_GET['parameters'],
                'files' => $_FILES
            );
            $json['module'] = $_POST['module'];
            $json['accion'] = $_POST['accion'];
        } else {

            $json =  json_decode($_POST['json'], true);
            // print_r($json);
            // print_r($json['module']);
        }
        if (!empty($json['module']) && !empty($json['accion'])) {

            $module = $json['module'];
            $accion = $json['accion'];
            $parameters = (!empty($json['parameters']) ? $json['parameters'] : false);
            $moduleUrl = './php/' . $module . '.php';
            if (file_exists($moduleUrl)) {

                require_once($moduleUrl);
                $_module = new $module;
                $response = $_module->{$accion}($parameters);
            } else {

                echo 'Error en la petición, el módulo ' . $module . ' no existe';
            }
        } else {

            echo 'Error en la petición, información sobre modulo y/o funcion incompleta';
        }
        // echo json_encode($response);
        // echo $response;
        // print_r($response);
        print_r(json_encode($response));
    }
}
$api = new Api();
