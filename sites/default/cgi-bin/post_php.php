<?php
$s_name = dirname(__FILE__);
$s_name = str_replace("cgi-bin", $_SERVER["UPLOAD_DIR"], $s_name);

$nofile = false;
$target_file = "";
$imageFileType = "";
if (isset($_FILES["filename"]))
{
	$target_file = $s_name . "/" . basename($_FILES["filename"]["name"]);
	$uploadOk = 1;
	$imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
}
else {
	$nofile = true;
	$uploadOk = 0;
}

echo "<!DOCTYPE html>", "\n";
echo "<html>", "\n", "\n";
echo "<head>", "\n";
echo "\t", "<title>Webserv</title>", "\n";
echo "</head>", "\n", "\n";
echo '<body class="main">', "\n";
if ($nofile)
	echo "Please, choose a file.<br>", "\n";
else if (file_exists($target_file))
{
	echo "Sorry, file already exists.<br>", "\n";
	$uploadOk = 0;
}

if ($uploadOk == 0)
	echo "Your file was not uploaded.<br>", "\n";
else
{
	echo "<br/>";
	if (move_uploaded_file($_FILES["filename"]["tmp_name"], $target_file))
		echo "The file ". htmlspecialchars( basename( $_FILES["filename"]["name"])). " has been uploaded.<br>", "\n";
	else
		echo "Sorry, there was an error uploading your file.<br>", "\n";
}

echo '</body>', "\n";
echo "</html>", "\n";

?>
