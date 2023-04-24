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


function GetEntries()
{
    $conn = debCon();
    $sql = 'SELECT * FROM entries ORDER BY entries.entry_id DESC LIMIT 30';

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

function GetSetting()
{
    $conn = debCon();
    $sql = 'SELECT * FROM settings WHERE setting_id = 1 ';

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

$entries = GetEntries();
$settings = GetSetting();

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="style.css">
    <title>Document</title>
</head>
<body>

<div class="main">

    <table class="p-md text-center">
        <thead>
            <th>
                id
            </th>
            <th>
                temperature
            </th>
            <th>
                humidity
            </th>
            <th>
                date
            </th>
        </thead>
        
        <tbody>
            <?php
                foreach ($entries as $entry) 
                {
                    ob_start(); ?>
                    <tr>
                        <td><?=$entry['entry_id']?></td>
                        <td><?=$entry['entry_temperature']?></td>
                        <td><?=$entry['entry_humidity']?></td>
                        <td><?=$entry['entry_date']?></td>
                    </tr>
                    <?php echo(ob_get_clean()); 
                }
            ?>
        </tbody>
    </table>
    <div class="blurb p-md text-center">
        <form action="UpdateSettings.php" method="POST">
            <h2>
                <label for="hex">change the color</label>
                <br>
                <input type="color" name="hex" id="hex">
                <br>
                <label for="humCap">change the humidity cap</label>
                <br>
                <input type="number" name="humCap" id="humCap">
                <br>
                <input type="submit" value="submit">
            </h2>
        </form>
    </div> 
    <div class="blurb p-md text-center">

    <?php
        echo "settings_hex = ";
        echo $settings[0]['settings_hex'];
        echo "<br> settings_tempCap = ";
        echo $settings[0]['settings_tempCap'];
		echo "<br> setting_rfid = ";
        echo $settings[0]['setting_rfid'];
    ?>
    </div>
</div>


</body>
</html>