<?php
session_start(); // Start the session

if (isset($_SESSION['username']) && isset($_SESSION["role"]) && $_SESSION['loggedin'] == true) {
    $_SESSION["role"] == "admin" ? header("location: admin_dash.php") : header("location: index.php");
}

require 'config.php';

$usernameError = $passwordError = $loginError = "";
$username = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = trim($_POST["username"]);
    $password = trim($_POST["password"]);

    // Input validation
    if (empty($username)) {
        $usernameError = "Username is required.";
    }

    if (empty($password)) {
        $passwordError = "Password is required.";
    }

    // Attempt login if there are no input errors
    if (empty($usernameError) && empty($passwordError)) {
        $sql = "SELECT username, password, role FROM users WHERE username = :username";

        $stmt = $conn->prepare($sql);
        $stmt->bindParam(':username', $username, PDO::PARAM_STR);
        $stmt->execute();

        if ($stmt->rowCount() == 1) {
            if ($row = $stmt->fetch()) {
                $hashedPassword = $row["password"];
                if (md5($password) == $hashedPassword) {
                    // Password is correct, so start a new session
                    session_regenerate_id();
                    $_SESSION["loggedin"] = true;
                    $_SESSION["username"] = $row["username"]; 
                    
                    $role = $row["role"];
                    $_SESSION["role"] = $role; 
                    // Redirect user to dashboard and admin to admin page
                    $role == "admin" ? header("location: admin_dash.php") : header("location: index.php");
                    exit;
                } else {
                    // Display an error message if password is not valid
                    $loginError = "Invalid username or password.";
                }
            }
        } else {
            // Display an error message if username doesn't exist
            $loginError = "Invalid username or password.";
        }
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Login</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link" href="view_foods.php">Order Food</a></li>
            <li><a class="nav-link active" href="#">Login</a></li>
            <li><a class="nav-link" href="signup.php">Sign up</a></li>
        </ul>
    </header>
    <div>
        <div class="form-holder">
            <form  action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
                <div class="text-secondary" style="font-size: 25px;">Login</div>
                <p class="text-danger" style="font-size: 12px;"><?php echo $loginError; ?></p>
                <label>Username</label> 
                <input class="form-control" type="text" name="username" id="username" value="<?php echo htmlspecialchars($username); ?>">
                <p class="text-danger" style="font-size: 12px;"><?php echo $usernameError; ?></p>

                <label>Password</label> 
                <input class="form-control" type="password" name="password">
                <p class="text-danger" style="font-size: 12px;"><?php echo $passwordError; ?></p>

                <div class="text-secondary" style="font-size: 12px;">Forgot Password? <a href="reset.php">Reset</a></div>
                <p class="text-secondary" style="font-size: 12px;">Don't have an account? <a href="signup.php">Sign Up</a></p>

                <input class="btn btn-primary" type="submit" value="Login">
            </form>
        </div>
        
    </div> 
    <footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
  <div class="footer-space"></div>
</footer>   
</body>
</html>
<?php
// Close connection
$conn = null;
?>