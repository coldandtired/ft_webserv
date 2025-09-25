<!DOCTYPE html>
<html>
<head><title>PHP GET</title></head>
<body>

<h1>PHP CGI</h1>

<?php
$var = explode("&", $_SERVER["QUERY_STRING"]);
$var2 = explode("=", $var[0]);
echo("You selected ");
echo ($var2[1]);
?>

</body>
</html>
