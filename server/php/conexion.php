<?php
class Conexion
{
    private $server;
    private $user;
    private $password;
    private $database;
    private $port;
    public $connection;

    function __construct()
    {
        $dataList = $this->datosConexion();
        $value = $dataList['conexion'];
        // $value = array(
        //     "server" => "localhost",
        //     "user" => "root",
        //     "password" => "David359265",
        //     "database" => "articulos",
        //     "port" => "3306"
        // );
        $this->server = $value['server'];
        $this->user = $value['user'];
        $this->password = $value['password'];
        $this->database = $value['database'];
        $this->port = $value['port'];
        $this->connection = new mysqli($this->server, $this->user, $this->password, $this->database, $this->port);
        if ($this->connection->connect_error) {
            echo "conexión fallida.";
            die("Error: " . $this->connection->connect_error);
        }
    }

    private function datosConexion()
    {
        $direccion = dirname(__FILE__);
        $jsonData = file_get_contents($direccion . "/config");
        return json_decode($jsonData, true);
    }
}