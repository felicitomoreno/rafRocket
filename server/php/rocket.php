<?php
class rocket extends Conexion
{
    public $Conexions;

    function __construct()
    {

        parent::__construct();
        $this->Conexions = new Conexion;
    }
    public function getValues($arg)
    { //no necesita ningun parámetro realmente.
        $sql = "SELECT * FROM rocket";
        $array = $this->Conexions->connection->query($sql);
        if (!$this->Conexions->connection->query($sql) === true) {

            echo "\n- Error en la petición -> " . $this->connection->connect_error . "\n";
        }
        $response = array();
        while ($row = $array->fetch_assoc()) {
            array_push($response, $row);
        }
        return json_encode($response, true);
        //return $response;
    }
    public function getSystemVar($arg)
    { //no necesita ningun parámetro realmente.
        $sql = "SELECT * FROM systemVar";
        $array = $this->Conexions->connection->query($sql);
        if (!$this->Conexions->connection->query($sql) === true) {

            echo "\n- Error en la petición -> " . $this->connection->connect_error . "\n";
        }
        $response = array();
        while ($row = $array->fetch_assoc()) {
            array_push($response, $row);
        }
        //return json_encode($response, true);
        return $response;
    }
    public function setSystemVar($arg)
    {
        //print_r($arg);
        $field = $arg['field'];
        $value = $arg['value'];
        $sql = "UPDATE systemVar SET $field = '$value' WHERE 1";

        if (!$this->Conexions->connection->query($sql)) {
            echo 'petición fallida -> ' . $this->connection->connect_error;
            return 0;
        } else {
            //return '{1: 1}';
            return 1;
        }
    }
}