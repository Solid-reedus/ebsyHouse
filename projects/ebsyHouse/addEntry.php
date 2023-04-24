<?php

function debCon()
{
    $servername = "localhost";
    $username = "main";
    $password = "Cw3as155$";
    $dbname = "85307_ebsyHouseDB";

    $conn = new mysqli($servername, $username, $password);
    if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
    }

    $conn->select_db($dbname);
    return $conn;
}

//////////////
if (!isset($_POST['temperature'])) 
{
    $temp = 1.1;
}
else
{
    $temp = $_POST['temperature'];
}

//////////////
if (!isset($_POST['rfidValue'])) 
{
    $rfidValue = "12345679";
}
else
{
    $rfidValue = $_POST['rfidValue'];
}

//////////////
if (!isset($_POST['humidity'])) 
{
    $hum = 1.1;
}
else
{
    $hum = $_POST['humidity'];
}

AddEntry($temp, $hum);
UpdateSettingsAdruino($rfidValue);

function AddEntry($_temp, $_hum)
{
    $conn = debCon();
    $stmt = $conn->prepare('INSERT INTO entries
    (entry_date, entry_temperature, entry_humidity) VALUES ( NOW() , '.$_temp.' , '.$_hum.')');
    $stmt->execute();
}

function UpdateSettingsAdruino($_rfidValue)
{
    $conn = debCon();
    $stmt = $conn->prepare('UPDATE settings SET setting_rfidValue='.$_rfidValue.' WHERE setting_id = 1');
    $stmt->execute();
}

function ReturnString()
{
    return "";
}

echo ReturnString();

//echo the stuff from the db

?>
