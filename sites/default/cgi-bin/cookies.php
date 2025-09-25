<?php
$input_cookie_name = '';
$input_cookie_value = '';
$message = '';
$set_cookie_name = '';

ini_set('session.cookie_httponly', '1');
ini_set('session.cookie_secure', '0');
ini_set('session.cookie_samesite', 'Strict');

if (isset($_POST['submit_set_cookie'])) {
    $input_cookie_name = htmlspecialchars(trim($_POST['cookie_name']));
    $input_cookie_value = htmlspecialchars(trim($_POST['cookie_value']));

    if (empty($input_cookie_name)) {
        $message = "<span style='color: red;'>Error: Cookie name cannot be empty!</span>";
    } else {
        // Set the cookie to expire in 1 hour
        setcookie($input_cookie_name, $input_cookie_value, time() + 3600, "/", "", false, true);
        $message = "Cookie '<strong>" . $input_cookie_name . "</strong>' has been set with value: '<strong>" . $input_cookie_value . "</strong>'.<br><em>Please refresh to confirm the cookie.</em>";
        $set_cookie_name = $input_cookie_name;
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Cookie Result</title>
</head>
<body style="font-family: Arial, sans-serif; background-color: #eef; margin: 20px;">
    <div style="max-width: 800px; margin: auto; background: white; padding: 20px; border-radius: 8px;">
        <h1 style="text-align:center;">Cookie Setting Result</h1>

        <?php if (!empty($message)): ?>
            <div style="padding: 15px; margin-bottom: 20px; border: 1px solid #ccc; background-color: #dff0d8; color: #3c763d;">
                <?php echo $message; ?>
            </div>
        <?php endif; ?>

     

  
    </div>
</body>
</html>
