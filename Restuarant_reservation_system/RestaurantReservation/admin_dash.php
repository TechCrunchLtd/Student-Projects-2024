<?php 
require 'config.php';

$username = "";
session_start();
// redirect if user is not admin
if ($_SESSION['role'] !== "admin") {
    header("location:index.php");
    exit();
}

// Function to fetch reservations based on provided criteria
function fetchReservations($conn, $query, $params = []) {
    $stmt = $conn->prepare($query);
    if (!empty($params)) {
        $stmt->bind_param(...$params);
    }
    $stmt->execute();
    $results = $stmt->fetchAll();
    $reservations = [];
    
    foreach ($results as $row) {
        $reservations[] = $row;
    }
    return $reservations;
}

// Function to display reservations
function displayReservations($reservations, $conn, $buttonLabel, $newStatus) {
    echo '<div class="foods-holder">';
    foreach ($reservations as $reservation) {
        echo "<div class='card' style='width: 18rem;padding:10px;'>";
        if ($reservation['minutes_until_reservation'] > 0) {
            echo '<div style="background-color: #198754;" class="info-bar"></div>';
        } elseif ($reservation['minutes_until_reservation'] > -($reservation['duration'] * 60)) {
            echo '<div style="background-color: #ffc107;" class="info-bar"></div>';
        } else {
            echo '<div style="background-color: #dc3545;" class="info-bar"></div>';
        }
        
        echo "<h5 class='card-title'>Reservation for " . $reservation['reservation_date'] . " at " . $reservation['reservation_time'] . "</h5>";
        
        // Determine time status
        if ($reservation['minutes_until_reservation'] > 0) {
            if ($reservation['minutes_until_reservation'] > 60) {
                $time_status = floor($reservation['minutes_until_reservation'] / 60) . " hours " . ($reservation['minutes_until_reservation'] % 60) . " minutes until reservation starts";
            } else {
                $time_status = $reservation['minutes_until_reservation'] . " minutes until reservation starts";
            }
        } elseif ($reservation['minutes_until_reservation'] > -($reservation['duration'] * 60)) {
            $time_status = "<span class='text-secondary' style='font-size: 25px;'>Reservation is currently ongoing</span>";
        } else {
            $time_status = "<span class='text-danger' style='font-size: 25px;'>Reservation has expired</span>";
        }

        echo "<p class='card-text'>" . $time_status . "</p>";

        // Show cancel button if reservation has not started and is within 30 minutes
        // if ($reservation['minutes_until_reservation'] >= 30 && $reservation['status'] == 'Pending') {
        //     echo "<form method='post'><button class='btn btn-primary' type='submit' name='cancel_reservation_id' value='" . $reservation['reservation_id'] . "'>Cancel Reservation</button></form>";
        // } else if ($reservation['status'] == 'REQUEST CANCEL') {
        //     echo "<p class='error'>Request to cancel submitted</p>";
        // } else if ($reservation['status'] == 'APPROVED') {
        //     echo "<p class='success'>Request to cancel approved</p>";
        // } else if ($reservation['status'] == 'CANCELLED') {
        //     echo "<p class='error'>Reservation cancelled by admin</p>";
        // }
        // table

        echo "<h6 class='card-subtitle mb-2 text-muted'>Table</h3><div>".$reservation['table_name']."</div><div>Location: ".$reservation['location']."</div><div>Capacity: ".$reservation['capacity']."</div>";

        // Fetch and display ordered foods for this reservation
        $sql_foods = "SELECT f.name, f.description, c.quantity FROM foods f JOIN cart c ON f.food_id = c.food_id WHERE c.reservation_id = :i";
        $stmt_foods = $conn->prepare($sql_foods);
        $stmt_foods->bindParam(":i", $reservation['reservation_id']);
        $stmt_foods->execute();
        $result_foods = $stmt_foods->fetchAll();

        if (count($result_foods) > 0) {
            echo "<br><h6 class='card-subtitle mb-2 text-muted'>Foods Ordered</h6><ul>";
            foreach ($result_foods as $food) {
                echo "<li>" . $food['quantity'] . "x " . $food['name'] . " - " . $food['description'] . "</li>";
            }
            echo "</ul>";
        } else {
            echo "<br><p>No food ordered for this reservation.</p>";
        }

        echo "<br><h6 class='card-subtitle mb-2 text-muted'>User: ".$reservation['username']."</h6>";
       

        // Button for changing reservation status
        if ($buttonLabel && $newStatus) {
            echo "<form method='post'><input type='hidden' name='reservation_id' value='" . $reservation['reservation_id'] . "'><input type='hidden' name='new_status' value='" . $newStatus . "'><button type='submit' class='btn btn-primary'>" . $buttonLabel . "</button></form>";
        }

        echo "</div>";
    }
    echo "</div>";
}
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['reservation_id'], $_POST['new_status'])) {
    $reservationId = $_POST['reservation_id'];
    $newStatus = $_POST['new_status'];

    $sql_update = "UPDATE reservations SET status = ? WHERE reservation_id = ?";
    $stmt_update = $conn->prepare($sql_update);
    $stmt_update->execute([$newStatus, $reservationId]);
    header("location:admin_dash.php");

}


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
            <li class="nav-item"><a class="nav-link" href="#">Admin Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="add_items.php">Add Items</a></li>
            <li class="nav-item"><a class="nav-link" href="items.php">Tables & Food</a></li>
            <li class="nav-item"><a class="nav-link" href="users.php">Users</a></li>
            <li class="nav-item"><a class="nav-link" href="transactions.php">Transactions</a></li>
            <li class="nav-item"><a class="nav-link" href="logout.php">Log out</a></li>
        </ul>
    </header>

    <div class="container">
    <?php 
        echo "<div class='text-secondary' style='font-size: 25px;'>Cancel Requests</div>";
        $cancelRequestsQuery = "SELECT t.*, r.*, TIMESTAMPDIFF(MINUTE, NOW(), CONCAT(reservation_date, ' ', reservation_time)) AS minutes_until_reservation FROM reservations r JOIN tables t ON t.table_id = r.table_id WHERE r.status = 'REQUEST CANCEL' ORDER BY reservation_date DESC, reservation_time DESC";

        $cancelRequests = fetchReservations($conn, $cancelRequestsQuery);
        if (count($cancelRequests) > 0){
            displayReservations($cancelRequests, $conn, "Approve Cancel", "APPROVED");
        } else {
            echo "<p>No cancel requests</p>";
        }
        
        
        // 2. Upcoming reservations
        echo "<div class='text-secondary' style='font-size: 25px;'>Upcoming Reservations</div>";
        $upcomingQuery = "SELECT t.*, r.*, TIMESTAMPDIFF(MINUTE, NOW(), CONCAT(reservation_date, ' ', reservation_time)) AS minutes_until_reservation FROM reservations r JOIN tables t ON t.table_id = r.table_id WHERE r.status = 'Pending' AND CONCAT(reservation_date, ' ', reservation_time) >= NOW() ORDER BY reservation_date DESC, reservation_time DESC";
        // $upcomingQuery = "SELECT * FROM reservations WHERE ";
        $upcomingReservations = fetchReservations($conn, $upcomingQuery);
        if (count($upcomingReservations) > 0){
            displayReservations($upcomingReservations, $conn, "Cancel", "CANCELLED");
        } else {
            echo "<p>No upcoming reservations</p>";
        }

        echo "<br>";
        
        // 3. Past and cancelled reservations
        echo "<div class='text-secondary' style='font-size: 25px;'>Past and Cancelled Reservations</div>";
        $pastCancelledQuery = "SELECT t.*, r.*, TIMESTAMPDIFF(MINUTE, NOW(), CONCAT(reservation_date, ' ', reservation_time)) AS minutes_until_reservation FROM reservations r JOIN tables t ON t.table_id = r.table_id WHERE (r.status = 'Pending' AND CONCAT(reservation_date, ' ', reservation_time) < NOW()) OR r.status = 'CANCELLED' OR r.status = 'APPROVED' ORDER BY reservation_date DESC, reservation_time DESC";
        // $pastCancelledQuery = "SELECT * FROM reservations WHERE ";
        $pastCancelledReservations = fetchReservations($conn, $pastCancelledQuery);
        if (count($pastCancelledReservations) > 0){
            displayReservations($pastCancelledReservations, $conn, null, null);
        } else {
            echo "<p>No reservations yet</p>";
        }

        echo "<br>";
        
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