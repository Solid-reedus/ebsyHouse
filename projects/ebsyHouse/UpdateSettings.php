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

if (!isset($_POST['hex'])) 
{
    $hex = "0fffff";
}
else
{
    $hex = $_POST['hex'];
}

if (!isset($_POST['humCap'])) 
{
    $humCap = 999;
}
else
{
    $humCap = $_POST['humCap'];
}

UpdateSettingsWeb($hex, $humCap);

function UpdateSettingsWeb($_hex, $_humCap)
{
    $conn = debCon();
    $stmt = $conn->prepare('UPDATE settings SET setting_id = 1, settings_hex = "'.$_hex.'" ,settings_tempCap = '. $_humCap);
    $stmt->execute();
}

header('Location: index.php');
?>