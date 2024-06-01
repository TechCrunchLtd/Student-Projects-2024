<?php
require 'config.php';
$admin_username = "";
session_start();
// redirect if user is not admin
if ($_SESSION['role'] !== "admin") {
    header("location:index.php");
    exit();
}
$admin_username = $_SESSION['username'];

// Function to make a user an admin
function makeAdmin($conn, $username) {
    $sql = "UPDATE users SET role = 'admin' WHERE username = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$username]);
}

// Function to delete a user
function deleteUser($conn, $username) {
    $sql = "DELETE FROM users WHERE username = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$username]);
}

// Retrieve users from database
$sql = "SELECT * FROM users";
$result = $conn->query($sql);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
<header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="admin_dash.php">Admin Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="add_items.php">Add Items</a></li>
            <li class="nav-item"><a class="nav-link" href="items.php">Tables & Food</a></li>
            <li class="nav-item"><a class="nav-link" href="#">Users</a></li>
            <li class="nav-item"><a class="nav-link" href="transactions.php">Transactions</a></li>
            <li class="nav-item"><a class="nav-link" href="logout.php">Log out</a></li>
        </ul>
    </header>
    <div class="container">
    <div class="text-secondary" style="font-size: 25px;">Available Users</div>
    <table class="table">
        <tr>
            <th>Username</th>
            <th>First Name</th>
            <th>Last Name</th>
            <th>Email</th>
            <th>Phone Number</th>
            <th>Role</th>   
            <th>Action</th>
        </tr>
        <?php
        while ($row = $result->fetch()) {
            echo "<tr>";
            echo "<td>" . htmlspecialchars($row["username"]) . "</td>";
            echo "<td>" . htmlspecialchars($row["firstname"]) . "</td>";
            echo "<td>" . htmlspecialchars($row["lastname"]) . "</td>";
            echo "<td>" . htmlspecialchars($row["email"]) . "</td>";
            echo "<td>" . htmlspecialchars($row["phonenumber"]) . "</td>";
            if ($row["role"] == "admin") {
                echo "<td>" . htmlspecialchars($row["role"]) . "</td>";
            } else {
                echo "<td><form method='post'><input type='hidden' name='username' value='" . htmlspecialchars($row["username"]) . "'><input class='btn btn-primary' type='submit' name='makeAdmin' value='Make Admin'></form></td>";
            }
            if ($row["username"] != $admin_username){
                echo "<td><form method='post'><input type='hidden' name='username' value='" . htmlspecialchars($row["username"]) . "'><input class='btn btn-primary' type='submit' name='deleteUser' value='Delete'></form></td>";
            } else {
                echo "<td></td>";
            }
            echo "</tr>";
        }
        ?>
    </table>
    </div>
    
    <?php
    // Process make admin action
    if (isset($_POST['makeAdmin'])) {
        $username = $_POST['username'];
        makeAdmin($conn, $username);
        header("Location: users.php");
    }

    // Process delete user action
    if (isset($_POST['deleteUser'])) {
        $username = $_POST['username'];
        deleteUser($conn, $username);
        header("Location: users.php");
    }
    ?>
<div class="footer-space"></div>
<footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
</body>
</html>