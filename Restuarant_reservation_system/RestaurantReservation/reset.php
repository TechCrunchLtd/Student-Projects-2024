<?php 
require 'config.php';
$username = $password = $c_password = $usernameError = $passwordError = $success = $successMsg = "";
$security_question_err = $security_answer_err = $security_question = $security_answer = "";
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST["username"];
    $password = trim($_POST["password"]);
    $c_password = $_POST['c_password'];
    $security_question = $_POST["security_question"];
    $security_answer = $_POST["security_answer"];

    if (empty($username)) {
        $usernameError = "Username is required.";
    }

    if (empty($password)) {
        $passwordError = "Password is required.";
    } elseif ($password !== $c_password){
        $passwordError = "Password do not match.";
    }

    if (empty($security_question)) {
        $security_question_err = "Please select security question.";
    } 

    if (empty($security_answer)) {
        $security_answer_err = "Security answer is required.";
    }

    if (empty($usernameError) && empty($passwordError) && empty($security_question_err) && empty($security_answer_err) ){
        $new_password = md5($password);
        $sql = "SELECT username, password, role FROM users WHERE username = ? AND password = ?";

        $stmt = $conn->prepare($sql);
        $stmt->execute([$username, $new_password]);
        $stmt->execute();

        if ($stmt->rowCount() == 1) {
            $success = true;
            $successMsg = "Password reset successfully. Redirecting to login page";
            ob_start();
            header("refresh: 3; url=login.php");
            ob_end_flush();
        } else {
            $sql = "UPDATE users SET password = ? WHERE username = ? AND security_question = ? AND security_answer = ?";
            try{
                $stmt = $conn->prepare($sql);
                $stmt->execute([$new_password, $username, $security_question, $security_answer]);
                if ($stmt->rowCount() > 0){
                    $success = true;
                    $successMsg = "Password reset successfully. Redirecting to login page";
                    ob_start();
                    header("refresh: 3; url=login.php");
                    ob_end_flush();
                } else {
                    $success = false;
                    $successMsg = "Incorrect username, security question or answer";
                }
                
            } catch (PDOException $e) {
                $success = false;
                $successMsg = "Error: " . $e->getMessage();
            }
        }
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dashboard</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
<header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link active" href="#">Order Food</a></li>
            <?php if ($username): ?>
                <li class="nav-item"><a class="nav-link" href="myreservations.php">My Reservations</a></li>
                <li class="nav-item"><a class="nav-link" href="account.php">Account</a></li>
                <li class="nav-item"><a class="nav-link" href="basket.php">Basket</a></li>
                <li><a class="nav-link" href="logout.php">Log out</a></li>
            <?php else: ?>
                <li><a class="nav-link" href="login.php">Login</a></li>
                <li><a class="nav-link" href="signup.php">Sign up</a></li>
            <?php endif ?>
        </ul>
    </header>
    <div class="form-holder">
        <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
            <div class="text-secondary" style="font-size: 25px;">Reset Password</div>
            <p style="font-size: 12px;" class='<?php echo $success ? "text-success" : "text-danger"; ?>'><?php echo $successMsg; ?></p>
            
            <label>Username</label> <input class="form-control" type="text" name="username" value="<?php echo htmlspecialchars($username); ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $usernameError; ?></p>
            <label>Security Question</label>
            <select class="form-select" name="security_question">
                <option value="In what city were you born?">In what city were you born?</option>
                <option value="What was the make and model of your first car?">What was the make and model of your first car?</option>
                <option value="What is the name of the street you grew up on?">What is the name of the street you grew up on?</option>
                <option value="What is the name of your best friend?">What is the name of your best friend?</option>
            </select>
            <p class="text-danger" style="font-size: 12px;"><?php echo $security_question_err; ?></p>

            <label>Security Answer</label>
            <input class="form-control" type="text" name="security_answer" value="<?php echo $security_answer; ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $security_answer_err; ?></p>
            
            <label>Password</label> <input class="form-control" type="password" name="password" value="<?php echo $password; ?>"><br>
            <label>Confirm Password</label> <input class="form-control" type="password" name="c_password" value="<?php echo $c_password; ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $passwordError; ?></p>

            <input class="btn btn-primary" type="submit" value="Reset Password">
        </form>
    </div>
    <div class="footer-space"></div>
    <footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
</body>
</html>