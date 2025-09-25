<?php
session_start();

$input_session_name = '';
$input_session_value = '';
$message = '';

if (isset($_POST['submit_set_session'])) {
    $input_session_name = htmlspecialchars(trim($_POST['session_name']));
    $input_session_value = htmlspecialchars(trim($_POST['session_value']));

    if (empty($input_session_name)) {
        $message = "<span style='color: red;'>Error: Session name cannot be empty!</span>";
    } else {
        $_SESSION[$input_session_name] = $input_session_value;
        $message = "Session variable '<strong>" . $input_session_name . "</strong>' has been set with value: '<strong>" . $input_session_value . "</strong>'.";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Session Result</title>
</head>
<body style="font-family: Arial, sans-serif; background-color: #eef; margin: 20px;">
    <div style="max-width: 800px; margin: auto; background: white; padding: 20px; border-radius: 8px;">
        <h1 style="text-align:center;">Session Handling Result</h1>

        <?php if (!empty($message)): ?>
            <div style="padding: 15px; margin-bottom: 20px; border: 1px solid #ccc; background-color: #dff0d8; color: #3c763d;">
                <?php echo $message; ?>
            </div>
        <?php endif; ?>
 
    </div>
</body>
</html>
