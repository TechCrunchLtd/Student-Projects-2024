<?php
// Include database connection settings
require 'config.php';

// clear errors and variables
$usernameError = $firstnameError = $lastnameError = $emailError = $passwordError = $phonenumber_err = $security_answer_err = $security_question_err = "";
$username = $firstname = $lastname = $email = $password = $successMsg = $phonenumber = $c_password = $security_question = $security_answer = "";
$success = false;

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // get user input
    $username = trim($_POST["username"]);
    $firstname = trim($_POST["firstname"]);
    $lastname = trim($_POST["lastname"]);
    $email = trim($_POST["email"]);
    $password = trim($_POST["password"]);
    $phonenumber = trim($_POST["phonenumber"]);
    $security_question = $_POST["security_question"];
    $security_answer = $_POST["security_answer"];
    $c_password = $_POST['c_password'];

    // Input validation - check all values are present
    if (empty($username)) {
        $usernameError = "Username is required.";
    } else {
        // check if username exists
        $sql = "SELECT firstname FROM users WHERE username = ?";
        $stmt = $conn->prepare($sql);
        $stmt->execute([$username]);
        if ($stmt->rowCount() > 0) {
            $usernameError = "Username is already taken.";
        }
    }
    if (empty($firstname)) {
        $firstnameError = "First name is required.";
    }
    if (empty($lastname)) {
        $lastnameError = "Last name is required.";
    }
    if (empty($email)) {
        $emailError = "Email is required.";
    } elseif (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
        $emailError = "Invalid email format.";
    }
    if (empty($password)) {
        $passwordError = "Password is required.";
    } elseif ($password !== $c_password){
        $passwordError = "Password do not match.";
    }
    if (empty($phonenumber)) {
        $phonenumber_err = "Phonenumber is required.";
    } elseif (strlen($phonenumber) < 10){
        // o7
        $phonenumber_err = "Phonenumber should atleast 10 characters.";
    } elseif (strlen($phonenumber) > 12){
        //+254
        $phonenumber_err = "Phonenumber should not be more than 12 characters.";
    }

    if (empty($security_question)) {
        $security_question_err = "Please select security question.";
    } 

    if (empty($security_answer)) {
        $security_answer_err = "Security answer is required.";
    } 

    // Check if all errors are empty
    if (empty($usernameError) && empty($firstnameError) && empty($lastnameError) && empty($emailError) && empty($passwordError) && empty($phonenumber_err)
            && empty($security_question_err) && empty($security_answer_err)    
    ) {
        // Proceed with inserting the user into the database
        $hashedPassword = md5($password);
        $sql = "INSERT INTO users (username, firstname, lastname, email, phonenumber, password, role, security_question, security_answer) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
        try {
            // add customer to 
            $role = "customer";
            $stmt = $conn->prepare($sql);
            $stmt->execute([$username, $firstname, $lastname, $email, $phonenumber, $hashedPassword, $role, $security_question, $security_answer]);
            $success = true;
            $successMsg ="Registration successful! Redirecting to login page";

            // redirect user to login page
            ob_start();
            header("refresh: 3; url=login.php");
            ob_end_flush();
        } catch (PDOException $e) {
            $success = false;
            $successMsg = "Error: " . $e->getMessage();
        }
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Register</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link" href="view_foods.php">Order Food</a></li>
            <li><a class="nav-link" href="login.php">Login</a></li>
            <li><a class="nav-link" href="#">Sign up</a></li>
        </ul>
    </header>
    <div class="form-holder">
        <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
            <div class="text-secondary" style="font-size: 25px;">Sign Up</div>
            <p style="font-size: 12px;" class='<?php echo $success ? "text-success" : "text-danger"; ?>'><?php echo $successMsg; ?></p>
            <label>Username</label> <input class="form-control" type="text" name="username" value="<?php echo htmlspecialchars($username); ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $usernameError; ?></p>

            <label>First Name</label>  <input class="form-control" type="text" name="firstname" value="<?php echo htmlspecialchars($firstname); ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $firstnameError; ?></p>

            <label>Last Name</label>  <input class="form-control" type="text" name="lastname" value="<?php echo htmlspecialchars($lastname); ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $lastnameError; ?></p>

            <label>Email</label>  <input class="form-control" type="email" name="email" value="<?php echo htmlspecialchars($email); ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $emailError; ?></p>

            <label>Phone Number</label>  <input class="form-control" type="number" name="phonenumber" value="<?php echo $phonenumber; ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $phonenumber_err; ?></p>

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

            <label>Password</label> <input class="form-control" type="password" name="password" value="<?php echo $password; ?>">
            <label>Confirm Password</label> <input class="form-control" type="password" name="c_password" value="<?php echo $c_password; ?>">
            <p class="text-danger" style="font-size: 12px;"><?php echo $passwordError; ?></p>
            <p class="text-secondary" style="font-size: 12px;">Already have an account? <a href="login.php">Login</a></p>
            <input class="btn btn-primary" type="submit" value="Register">
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
