<?php
require 'config.php'; 

session_start();

// Fetch user information from the database
if (isset($_SESSION['username'])) {
    $username = $_SESSION['username'];
    $sql = "SELECT * FROM users WHERE username = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$username]);
    $user = $stmt->fetch();
} else {
    header("Location: login.php");
    exit;
}

// Function to update user information
function updateUser($conn, $username, $firstname, $lastname, $email, $phonenumber) {
    $sql = "UPDATE users SET firstname = COALESCE(?, firstname), lastname = COALESCE(?, lastname), email = COALESCE(?, email), phonenumber = COALESCE(?, phonenumber) WHERE username = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$firstname, $lastname, $email, $phonenumber, $username]);
}

// Process form submission
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $firstname = empty($_POST["firstname"]) ? null:$_POST["firstname"];
    $lastname = empty($_POST["lastname"]) ? null:$_POST["lastname"];
    $email = empty($_POST["email"]) ? null:$_POST["email"];
    $phonenumber = empty($_POST["phonenumber"]) ? null:$_POST["phonenumber"];

    updateUser($conn, $username, $firstname, $lastname, $email, $phonenumber);
    header("Location: account.php");
    exit;
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Account</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link" href="view_foods.php">Order Food</a></li>
            <?php if ($username): ?>
                <li class="nav-item"><a class="nav-link" href="myreservations.php">My Reservations</a></li>
                <li class="nav-item"><a class="nav-link active" href="#">Account</a></li>
                <li class="nav-item"><a class="nav-link" href="basket.php">Basket</a></li>
                <li><a class="nav-link" href="logout.php">Log out</a></li>
            <?php else: ?>
                <li><a class="nav-link" href="login.php">Login</a></li>
                <li><a class="nav-link" href="signup.php">Sign up</a></li>
            <?php endif ?>
        </ul>
    </header>
    <div class="container-acc">
        <div class="text-secondary" style="font-size: 25px;">My Account</div>
        <div class="text-secondary" style="font-size: 25px;">Welcome, <?php echo htmlspecialchars($user["username"]); ?></div>
        <p>First Name: <?php echo htmlspecialchars($user["firstname"]); ?></p>
        <p>Last Name: <?php echo htmlspecialchars($user["lastname"]); ?></p>
        <p>Email: <?php echo htmlspecialchars($user["email"]); ?></p>
        <p>Phone Number: <?php echo htmlspecialchars($user["phonenumber"]); ?></p>
        <hr>
        <br>
        <h3>Edit Information</h3>
        <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
            <div>
                <label>First Name</label>
                <input class="form-control" type="text" name="firstname" value="">
            </div>
            <div>
                <label>Last Name</label>
                <input class="form-control" type="text" name="lastname" value="">
            </div>
            <div>
                <label>Email</label>
                <input class="form-control" type="text" name="email" value="">
            </div>
            <div>
                <label>Phone Number</label>
                <input class="form-control" type="text" name="phonenumber" value="">
            </div><br>
            <div>
                <input class="btn btn-primary" type="submit" value="Update">
            </div>
        </form>
        <br>
    </div>
</body>
<div class="footer-space"></div>
<footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
</html>

<?php
// Close connection
$conn = null;
?>
