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
if (!isset($_POST['humidity'])) 
{
    $hum = 1.1;
}
else
{
    $hum = $_POST['humidity'];
}

AddEntry($temp, $hum);


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
    $stmt = $conn->prepare('UPDATE settings SET setting_rfid = "'.$_rfidValue.'" WHERE setting_id = 1');
    $stmt->execute();
}


if (isset($_POST['rfidValue'])) 
{
	UpdateSettingsAdruino($_POST['rfidValue']);  
}


function GetSetting()
{
    $conn = debCon();
    $sql = 'SELECT * FROM settings WHERE setting_id = 1';

    $result = $conn->query($sql);

    if ($result->num_rows > 0) 
    {
        return $result->fetch_all(MYSQLI_ASSOC);
    }
    else
    {
        return [];
    }
}

$settings = GetSetting();

echo "rfidWeb*" . $settings[0]['setting_rfid'] . "!humCap*" . $settings[0]['settings_tempCap'] ."!HexVal*". $settings[0]['settings_hex'] . "!";



?>
