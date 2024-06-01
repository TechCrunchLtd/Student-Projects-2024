<?php
// Include database connection
require_once 'config.php';

// Start the session
session_start();

// Check if a user is logged in and get their username. 
if (!isset($_SESSION['username'])) {
    // Redirect to login page
    header("location:login.php");
}
$username = $_SESSION['username'];

// If there's a request to cancel a reservation
if (isset($_POST['cancel_reservation_id'])) {
    $cancel_reservation_id = $_POST['cancel_reservation_id'];
    // SQL to cancel the reservation. Adjust the status value according to your application's logic
    $sql_cancel = "UPDATE `reservations` SET `status` = 'REQUEST CANCEL' WHERE `reservation_id` = :i";
    $stmt_cancel = $conn->prepare($sql_cancel);
    $stmt_cancel->bindParam(":i", $cancel_reservation_id);
    $stmt_cancel->execute();
    header("location:myreservations.php");
}

// SQL to fetch reservations for the logged-in user
$sql = "SELECT t.*, r.*, TIMESTAMPDIFF(MINUTE, NOW(), CONCAT(reservation_date, ' ', reservation_time)) AS minutes_until_reservation FROM reservations r JOIN tables t ON t.table_id = r.table_id WHERE username = :s ORDER BY reservation_date DESC, reservation_time DESC";
$stmt = $conn->prepare($sql);
$stmt->bindParam(":s", $username);
$stmt->execute();
$results = $stmt->fetchAll();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>My Reservations</title>
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
                <li class="nav-item"><a class="nav-link active" href="#">My Reservations</a></li>
                <li class="nav-item"><a class="nav-link" href="account.php">Account</a></li>
                <li class="nav-item"><a class="nav-link" href="basket.php">Basket</a></li>
                <li><a class="nav-link" href="logout.php">Log out</a></li>
            <?php else: ?>
                <li><a class="nav-link" href="login.php">Login</a></li>
                <li><a class="nav-link" href="signup.php">Sign up</a></li>
            <?php endif ?>
        </ul>
    </header>
    <div class="container foods-holder">
    <?php 
        if (count($results) > 0) {
            foreach ($results as $row) {
                
                echo "<div class='card' style='width: 18rem;padding:10px;'>";

                if ($row['minutes_until_reservation'] > 0) {
                    echo '<div style="background-color: #198754;" class="info-bar"></div>';
                } elseif ($row['minutes_until_reservation'] > -($row['duration'] * 60)) {
                    echo '<div style="background-color: #ffc107;" class="info-bar"></div>';
                } else {
                    echo '<div style="background-color: #dc3545;" class="info-bar"></div>';
                }
                
                echo "<h5 class='card-title'>Reservation for " . $row['reservation_date'] . " at " . $row['reservation_time'] . "</h5>";
                
                // Determine time status
                if ($row['minutes_until_reservation'] > 0) {
                    if ($row['minutes_until_reservation'] > 60) {
                        $time_status = floor($row['minutes_until_reservation'] / 60) . " hours " . ($row['minutes_until_reservation'] % 60) . " minutes until reservation starts";
                    } else {
                        $time_status = $row['minutes_until_reservation'] . " minutes until reservation starts";
                    }
                } elseif ($row['minutes_until_reservation'] > -($row['duration'] * 60)) {
                    $time_status = "<span class='text-secondary' style='font-size: 25px;'>Reservation is currently ongoing</span>";
                } else {
                    $time_status = "<span class='text-danger' style='font-size: 25px;'>Reservation has expired</span>";
                }
        
                echo "<p class='card-text'>" . $time_status . "</p>";
        
                // Show cancel button if reservation has not started and is within 30 minutes
                if ($row['minutes_until_reservation'] >= 30 && $row['status'] == 'Pending') {
                    echo "<form method='post'><button class='btn btn-primary' type='submit' name='cancel_reservation_id' value='" . $row['reservation_id'] . "'>Cancel Reservation</button></form>";
                } else if ($row['status'] == 'REQUEST CANCEL') {
                    echo "<p class='error'>Request to cancel submitted</p>";
                } else if ($row['status'] == 'APPROVED') {
                    echo "<p class='success'>Request to cancel approved</p>";
                } else if ($row['status'] == 'CANCELLED') {
                    echo "<p class='error'>Reservation cancelled by admin</p>";
                }
                // table

                echo "<h6 class='card-subtitle mb-2 text-muted'>Table</h3><div>".$row['table_name']."</div><div>Location: ".$row['location']."</div><div>Capacity: ".$row['capacity']."</div>";
        
                // Fetch and display ordered foods for this reservation
                $sql_foods = "SELECT f.name, f.description, c.quantity FROM foods f JOIN cart c ON f.food_id = c.food_id WHERE c.reservation_id = :i";
                $stmt_foods = $conn->prepare($sql_foods);
                $stmt_foods->bindParam(":i", $row['reservation_id']);
                $stmt_foods->execute();
                $result_foods = $stmt_foods->fetchAll();
        
                if (count($result_foods) > 0) {
                    echo "<br><h6 class='card-subtitle mb-2 text-muted'>Foods Ordered</h6><ul>";
                    foreach ($result_foods as $food) {
                        echo "<li>" . $food['quantity'] . "x " . $food['name'] . " - " . $food['description'] . "</li>";
                    }
                    echo "</ul>";
                } else {
                    echo "<p>No food ordered for this reservation.</p>";
                }
                echo "</div>";
            }
        } else {
            echo "<p>No reservations found.</p>";
        }
    ?>
    </div>
    <div class="footer-space"></div>
<footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
</body>
</html>